import dataclasses as dc
import difflib
import glob
import json
import shutil
import subprocess
import traceback
from abc import ABC, abstractmethod
from enum import StrEnum
from multiprocessing import Pool, cpu_count
from pathlib import Path
import hashlib

from cutekit import builder, cli, const, model, shell, vt100
from typing import Any

SOURCE_DIR: Path = Path(__file__).parent
TESTS_DIR: Path = SOURCE_DIR.parent.parent / "tests"

HTML5LIB_TESTS_DIR = TESTS_DIR / "html" / "html5lib"
HTML5LIB_TESTS_URL = "https://github.com/vaev-org/html5lib-tests"
HTML5LIB_TESTS_ROOT = Path(const.PROJECT_CK_DIR) / "html5lib-tests"
HTML5LIB_FLAKES_ROOT = Path(const.PROJECT_CK_DIR) / "tests" / "html5lib"

TESTS_ROOT = Path(const.PROJECT_CK_DIR) / "tests" / "html5lib"


def _buildTestRunner(args: model.TargetArgs) -> builder.ProductScope:
    scope = builder.TargetScope.use(args)
    TestRunnerComponent = scope.registry.lookup("html5lib-tests-runner.main", model.Component)

    if TestRunnerComponent is None:
        raise RuntimeError("html5lib-tests-runner not found")
    return builder.build(scope, TestRunnerComponent)[0]


def _htmlTestsDir(args: model.TargetArgs) -> Path:
    HtmlTests = model.Registry.use(args).lookup("vaev-engine.html.tests", model.Component)

    if not HtmlTests:
        raise RuntimeError("vaev html tests not found")

    return Path(HtmlTests.dirname())


def _ensureTests():
    if not HTML5LIB_TESTS_ROOT.exists():
        try:
            print(f"Cloning html5lib-tests from {HTML5LIB_TESTS_URL}...")
            shell.exec("git", "clone", HTML5LIB_TESTS_URL, str(HTML5LIB_TESTS_ROOT), "--depth=1")
        except Exception as e:
            if HTML5LIB_TESTS_ROOT.exists():
                HTML5LIB_TESTS_ROOT.rmdir()
            raise RuntimeError(f"Failed to clone html5lib-tests: {e}")


@dc.dataclass
class TestReport:
    reference: str
    actual: str


def _parseTestReport(data) -> TestReport:
    return TestReport(**json.loads(data))


@dc.dataclass
class TestResult:
    class Status(StrEnum):
        PASS = "PASS"
        FAIL = "FAIL"
        PANIC = "PANIC"

    fileName: str
    testIndex: int
    status: Status
    input: bytes
    reference: Any = None
    actual: list[Any] = dc.field(default_factory=list)
    error: str = ""


@dc.dataclass
class TestCase:
    fileName: str
    testIndex: int
    data: bytes | str


class TestSuite(ABC):
    @property
    @abstractmethod
    def name(self) -> str: ...

    @abstractmethod
    def collect(self) -> list[TestCase]: ...

    @abstractmethod
    def serialize(self, case: TestCase) -> bytes: ...


SECTION_HEADERS = {
    b"#errors",
    b"#new-errors",
    b"#document-fragment",
    b"#script-off",
    b"#script-on",
    b"#document",
}


def _splitTreeConstructionTests(data: bytes) -> list[bytes]:
    tests = []
    current: list[bytes] = []
    in_data = False

    for line in data.split(b"\n"):
        if line == b"#data":
            if current:
                while current and current[-1] == b"":
                    current.pop()
                tests.append(b"\n".join(current) + b"\n")
            current = [b"#data"]
            in_data = True
        elif in_data and line in SECTION_HEADERS:
            in_data = False
            current.append(line)
        else:
            current.append(line)

    if current:
        while current and current[-1] == b"":
            current.pop()
        tests.append(b"\n".join(current) + b"\n")

    return tests


class TreeConstructionSuite(TestSuite):
    @property
    def name(self) -> str:
        return "tree_construction"

    def collect(self) -> list[TestCase]:
        cases = []
        for file in glob.glob(str(HTML5LIB_TESTS_ROOT) + "/tree-construction/*.dat"):
            file_name = Path(file).name
            with open(file, "rb") as f:
                data = f.read()
            for i, test in enumerate(_splitTreeConstructionTests(data)):
                cases.append(TestCase(fileName=file_name, testIndex=i, data=test))
        return cases

    def serialize(self, case: TestCase) -> bytes:
        if isinstance(case.data, bytes):
            return case.data
        return case.data.encode("utf-8")


class TokenizerSuite(TestSuite):
    @property
    def name(self) -> str:
        return "tokenizer"

    def collect(self) -> list[TestCase]:
        cases = []
        for file in glob.glob(str(HTML5LIB_TESTS_ROOT) + "/tokenizer/*.test"):
            file_name = Path(file).name
            with open(file, "rb") as f:
                data = json.load(f)
            if "tests" not in data:
                continue
            for i, test in enumerate(data["tests"]):
                cases.append(TestCase(fileName=file_name, testIndex=i, data=test))
        return cases

    def serialize(self, case: TestCase) -> bytes:
        return json.dumps(case.data).encode("utf-8")


SUITES: dict[str, TestSuite] = {
    s.name: s for s in [TreeConstructionSuite(), TokenizerSuite()]
}


def _runTest(test_data) -> TestResult:
    fileName, testIndex, serialized, testRunnerPath, suite_name = test_data

    try:
        res = subprocess.run(
            [str(testRunnerPath), "-s", suite_name],
            input=serialized,
            capture_output=True,
            timeout=10
        )

        if res.returncode == 0:
            report = _parseTestReport(res.stdout.decode("utf-8"))

            passed = len(report.actual) > 0
            for a in report.actual:
                if a != report.reference:
                    passed = False
                    break

            return TestResult(
                fileName=fileName,
                testIndex=testIndex,
                status=TestResult.Status.PASS if passed else TestResult.Status.FAIL,
                input=serialized,
                reference=report.reference,
                actual=report.actual
            )
        else:
            return TestResult(
                fileName=fileName,
                testIndex=testIndex,
                status=TestResult.Status.PANIC,
                input=serialized,
                error=res.stderr.decode("utf-8")
            )
    except subprocess.TimeoutExpired:
        return TestResult(
            fileName=fileName,
            testIndex=testIndex,
            status=TestResult.Status.PANIC,
            input=serialized,
            error="Test timeout (>10s)"
        )
    except Exception:
        return TestResult(
            fileName=fileName,
            testIndex=testIndex,
            status=TestResult.Status.PANIC,
            input=serialized,
            error=traceback.format_exc()
        )


def _runTests(testRunner: builder.ProductScope, suite: TestSuite) -> list[TestResult]:
    cases = suite.collect()
    allTests = [
        (c.fileName, c.testIndex, suite.serialize(c), testRunner.path, suite.name)
        for c in cases
    ]

    totalTests = len(allTests)
    numWorkers = max(1, cpu_count() - 1)
    print(f"Running {vt100.CYAN}{totalTests}{vt100.RESET} tests using {vt100.CYAN}{numWorkers}{vt100.RESET} workers...\n")

    testResults: list[TestResult] = []

    with Pool(processes=numWorkers) as pool:
        results_iter = pool.imap_unordered(_runTest, allTests, chunksize=10)
        for idx, result in enumerate(results_iter, 1):
            testResults.append(result)
            if idx % 100 == 0 or idx == totalTests:
                print(f"Progress: {idx}/{totalTests}")

    testResults.sort(key=lambda x: (x.fileName, x.testIndex))
    return testResults


def _saveTest(result: TestResult, root_dir: Path):
    h = hashlib.shake_256(result.input)
    testDir = root_dir / result.fileName / h.hexdigest(8)
    testDir.mkdir(parents=True, exist_ok=True)

    if isinstance(result.input, str):
        (testDir / "input").write_text(result.input)
    else:
        (testDir / "input").write_bytes(result.input)

    if result.reference:
        (testDir / "reference").write_text(json.dumps(result.reference))

    for i, a in enumerate(result.actual):
        if result.actual:
            (testDir / ("actual-" + str(i))).write_text(json.dumps(a))

    if result.error:
        (testDir / "error").write_text(result.error)


@cli.command("html5lib-tests", "Manage the html5lib tests")
def _(): ...


class Html5libTestsArgs(model.TargetArgs):
    suite: str = cli.arg("s", "suite", "The test suite to run", "tree_construction")


def _resolveSuite(args: Html5libTestsArgs) -> TestSuite:
    if args.suite not in SUITES:
        raise RuntimeError(f"Unknown test suite '{args.suite}'. Available: {', '.join(SUITES)}")
    return SUITES[args.suite]

def _resultsToLines(testResults: list[TestResult]) -> list[str]:
    lines = []
    for result in testResults:
        h = hashlib.shake_256(result.input)
        lines.append(result.fileName + "/" + h.hexdigest(8) + ": " + str(result.status) + "\n")
    lines.sort()
    return lines

def _parseResultLines(lines: list[str]) -> dict[str, str]:
    result = {}
    for line in lines:
        line = line.strip()
        if ": " in line:
            key, status = line.rsplit(": ", 1)
            result[key] = status
    return result



@cli.command("html5lib-tests/run", "Run the tests")
def _(args: Html5libTestsArgs):
    _ensureTests()

    suite = _resolveSuite(args)
    testRunner = _buildTestRunner(args)
    testResults = _runTests(testRunner, suite)

    passed = sum(1 for r in testResults if r.status == TestResult.Status.PASS)
    failed = sum(1 for r in testResults if r.status == TestResult.Status.FAIL)
    panic = sum(1 for r in testResults if r.status == TestResult.Status.PANIC)

    fail_tests_root = HTML5LIB_FLAKES_ROOT / args.suite / "fail"
    panic_tests_root = HTML5LIB_FLAKES_ROOT / args.suite / "panic"

    print()
    shutil.rmtree(fail_tests_root, ignore_errors=True)
    shutil.rmtree(panic_tests_root, ignore_errors=True)
    fail_tests_root.mkdir(parents=True, exist_ok=True)
    panic_tests_root.mkdir(parents=True, exist_ok=True)

    for result in testResults:
        if result.status == TestResult.Status.FAIL:
            _saveTest(result, fail_tests_root)
        elif result.status == TestResult.Status.PANIC:
            _saveTest(result, panic_tests_root)

    expectedFilePath = HTML5LIB_TESTS_DIR / (args.suite + "-expected.txt")

    if expectedFilePath.exists():
        actual_lines = _resultsToLines(testResults)
        expected_lines = expectedFilePath.read_text().splitlines(keepends=True)

        print(f"{vt100.RED}Failed {failed} tests{vt100.RESET}, {vt100.PURPLE}Panicked {panic} tests{vt100.RESET}, {vt100.GREEN}Passed {passed} tests{vt100.RESET}")

        if actual_lines != expected_lines:
            diff = list(difflib.unified_diff(
                expected_lines,
                actual_lines,
                fromfile="expected",
                tofile="actual",
            ))

            diffPath = HTML5LIB_FLAKES_ROOT / args.suite / "expected.diff"
            diffPath.parent.mkdir(parents=True, exist_ok=True)
            diffPath.write_text("".join(diff))

            expected_map = _parseResultLines(expected_lines)
            actual_map = _parseResultLines(actual_lines)

            new_fails   = [k for k, v in actual_map.items() if v == "FAIL"  and expected_map.get(k) != "FAIL"]
            new_passes  = [k for k, v in actual_map.items() if v == "PASS"  and expected_map.get(k) != "PASS"]
            new_panics  = [k for k, v in actual_map.items() if v == "PANIC" and expected_map.get(k) != "PANIC"]

            base = HTML5LIB_FLAKES_ROOT / args.suite
            (base / "new-fails.txt").write_text("\n".join(sorted(new_fails)))
            (base / "new-passes.txt").write_text("\n".join(sorted(new_passes)))
            (base / "new-panics.txt").write_text("\n".join(sorted(new_panics)))

            print()
            print(f"  {vt100.RED}New failures: {len(new_fails)}{vt100.RESET}")
            print(f"  {vt100.GREEN}New passes:   {len(new_passes)}{vt100.RESET}")
            print(f"  {vt100.PURPLE}New panics:   {len(new_panics)}{vt100.RESET}")

            raise RuntimeError(f"Test results differ from expected (see {diffPath.absolute()})")
    else:
        print(f"{vt100.YELLOW}No expected file found at {expectedFilePath}, skipping comparison.{vt100.RESET}")



@cli.command("html5lib-tests/bootstrap", "Generate the expected.txt file")
def _(args: Html5libTestsArgs):
    _ensureTests()

    suite = _resolveSuite(args)
    testRunner = _buildTestRunner(args)
    testResults = _runTests(testRunner, suite)

    HTML5LIB_TESTS_DIR.mkdir(parents=True, exist_ok=True)
    expectedFilePath = HTML5LIB_TESTS_DIR / (args.suite + "-expected.txt")
    print("Generated", expectedFilePath)

    with open(expectedFilePath, "w") as f:
        f.writelines(_resultsToLines(testResults))
