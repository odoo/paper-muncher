import dataclasses as dc
import glob
import json
import shutil
import subprocess
import traceback
from enum import StrEnum
from io import TextIOBase
from multiprocessing import Pool, cpu_count
from pathlib import Path

from cutekit import builder, cli, const, model, shell, vt100

from .reftests.utils import fetchMessage


HTML5LIB_TESTS_URL = "https://github.com/html5lib/html5lib-tests"
HTML5LIB_TESTS_ROOT = Path(const.PROJECT_CK_DIR) / "html5lib-tests"
HTML5LIB_TESTS_LICENSE = HTML5LIB_TESTS_ROOT / "LICENSE"

TESTS_ROOT = Path(const.PROJECT_CK_DIR) / "tests" / "html5lib"
FAILED_TESTS_ROOT = TESTS_ROOT / "failed"
PANIC_TESTS_ROOT = TESTS_ROOT / "panic"

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
    passed: bool
    reference: str
    actual: str

def _parseTestReport(data) -> TestReport:
    return TestReport(**json.loads(data))

@dc.dataclass
class TestResult:
    class Status(StrEnum):
        PASSED = "passed"
        FAILED = "failed"
        PANIC = "panic"
    
    fileName: str
    testIndex: int
    status: Status
    input: bytes
    reference: str = ""
    actual: str = ""
    error: str = ""

def _runTest(test_data) -> TestResult:
    """Run a single test and return the result"""
    fileName, testIndex, test, testRunnerPath = test_data
    
    try:
        res = subprocess.run(
            [str(testRunnerPath)],
            input=test,
            capture_output=True,
            timeout=10
        )
        
        if res.returncode == 0:
            report = _parseTestReport(res.stdout.decode("utf-8"))

            return TestResult(
                fileName=fileName,
                testIndex=testIndex,
                status=TestResult.Status.PASSED if report.passed else TestResult.Status.FAILED,
                input=test,
                reference=report.reference,
                actual=report.actual
            )
        else:
            return TestResult(
                fileName=fileName,
                testIndex=testIndex,
                status=TestResult.Status.PANIC,
                input=test,
                error=res.stderr.decode("utf-8")
            )
    except subprocess.TimeoutExpired:
        return TestResult(
            fileName=fileName,
            testIndex=testIndex,
            status=TestResult.Status.PANIC,
            input=test,
            error="Test timeout (>10s)"
        )
    except Exception:
        return TestResult(
            fileName=fileName,
            testIndex=testIndex,
            status=TestResult.Status.PANIC,
            input=test,
            error=traceback.format_exc()
        )

def _runTests(testRunner: builder.ProductScope) -> list[TestResult]:
    allTests = []
    for file in glob.glob(str(HTML5LIB_TESTS_ROOT) + "/tree-construction/*.dat"):
        file_name = Path(file).name
        with open(file, "rb") as f:
            data = f.read()
            for i, test in enumerate(data.split(b"\n\n")):
                allTests.append((file_name, i, test + b"\n", testRunner.path))
    
    totalTests = len(allTests)
    numWorkers = max(1, cpu_count() - 1)
    print(f"Running {vt100.CYAN}{totalTests}{vt100.RESET} tests using {vt100.CYAN}{numWorkers}{vt100.RESET} workers...\n")
    
    testResults: list[TestResult] = []
    passed_count = 0
    failed_count = 0
    panic_count = 0
    
    with Pool(processes=numWorkers) as pool:
        results_iter = pool.imap_unordered(
            _runTest,
            allTests,
            chunksize=10
        )
        
        for idx, result in enumerate(results_iter, 1):
            testResults.append(result)
            if result.status == TestResult.Status.PASSED:
                passed_count += 1
            elif result.status == TestResult.Status.FAILED:
                failed_count += 1
            else:  # PANIC
                panic_count += 1
            
            if idx % 100 == 0 or idx == totalTests:
                print(f"Progress: {idx}/{totalTests}")
    
    testResults.sort(key=lambda x: (x.fileName, x.testIndex))

    return testResults

def _saveTest(result: TestResult, root_dir: Path):
    testDir = root_dir / result.fileName / f"test_{result.testIndex:04d}"
    testDir.mkdir(parents=True, exist_ok=True)
    
    (testDir / "input").write_bytes(result.input)

    if result.reference:
        (testDir / "reference").write_text(result.reference)
    
    if result.actual:
        (testDir / "actual").write_text(result.actual)
    
    if result.error:
        (testDir / "error").write_text(result.error)


def _bytesToCppStringLiteral(data: bytes) -> list[str]:
    result = []

    current = []
    for b in data:
        if b == ord('"'):
            current.append('\\"')
        elif b == ord('\\'):
            current.append('\\\\')
        elif b == ord('\n'):
            current.append('\\n')
            result.append('"' + ''.join(current) + '"')
            current = []
        elif b == ord('\r'):
            current.append('\\r')
        elif b == ord('\t'):
            current.append('\\t')
        elif 32 <= b < 127:
            current.append(chr(b))
        else:
            current.append(f'\\{b:03o}')

    return result

def _importTest(result: TestResult, output: TextIOBase):
    testName = Path(result.fileName).stem
    testIndex = result.testIndex

    output.writelines(
        [
            f"test$(\"html5lib-{testName}-{testIndex}\") {{\n",
            "    auto result = try$(Html5LibTest::run(\n",
        ]
        +
        [" " * 8 + x + "\n" for x in _bytesToCppStringLiteral(result.input)]
        +
        [
            " " * 8 + "\"\"s\n",
            "    ));\n\n",

            "    expect$(result.passed);\n\n",

            "    return Ok();\n",
            "}\n\n",
        ]
    )


@cli.command("html5lib-tests", "Manage the html5lib tests")
def _(): ...


class Html5libTestsArgs(model.TargetArgs):
    pass

@cli.command("html5lib-tests/run", "Run the tests")
def _(args: Html5libTestsArgs):
    _ensureTests()

    testRunner = _buildTestRunner(args)

    shutil.rmtree(FAILED_TESTS_ROOT, ignore_errors=True)
    shutil.rmtree(PANIC_TESTS_ROOT, ignore_errors=True)
    FAILED_TESTS_ROOT.mkdir(parents=True, exist_ok=True)
    PANIC_TESTS_ROOT.mkdir(parents=True, exist_ok=True)

    testResults = _runTests(testRunner)
    
    passed = sum(1 for r in testResults if r.status == TestResult.Status.PASSED)
    failed = sum(1 for r in testResults if r.status == TestResult.Status.FAILED)
    panic = sum(1 for r in testResults if r.status == TestResult.Status.PANIC)
    
    print()
    
    if failed > 0 or panic > 0:
        print(f"{vt100.BRIGHT_GREEN}// {fetchMessage(model.Registry.use(args), 'witty')}{vt100.RESET}")
        print(f"{vt100.RED}Failed {failed} tests{vt100.RESET}, {vt100.PURPLE}Panicked {panic} tests{vt100.RESET}, {vt100.GREEN}Passed {passed} tests{vt100.RESET}")
        
        print()
        for result in testResults:
            if result.status == TestResult.Status.FAILED:
                _saveTest(result, FAILED_TESTS_ROOT)
            elif result.status == TestResult.Status.PANIC:
                _saveTest(result, PANIC_TESTS_ROOT)
        
        print(f"Test results: {TESTS_ROOT.absolute()}")
    else:
        print(f"{vt100.GREEN}// {fetchMessage(model.Registry.use(args), 'nice')}{vt100.RESET}")
        print(f"{vt100.GREEN}All tests passed{vt100.RESET}")

@cli.command("html5lib-tests/import", "Import passing tests into unit tests")
def _(args: Html5libTestsArgs):
    _ensureTests()

    testRunner = _buildTestRunner(args)

    testResults = _runTests(testRunner)

    htmlTestsDir = _htmlTestsDir(args)

    with open(htmlTestsDir / "html5lib-tests.cpp", "w") as testFile:
        testFile.write(f"// Tests imported from {HTML5LIB_TESTS_URL}\n//\n")
        testFile.writelines([f"// {l}\n" for l in HTML5LIB_TESTS_LICENSE.read_text().splitlines()] + ["\n"])

        testFile.writelines([
            "#include <karm/test>\n\n",

            "import Karm.Core;\n\n",
            "import Html5LibTest;\n\n",

            "using namespace Karm;\n\n",
            
            "namespace Vaev::Html::Tests {\n\n"
        ])

        for result in testResults:
            if result.status == TestResult.Status.PASSED:
                _importTest(result, testFile)

        testFile.writelines([
            "} // namespace Vaev::Html::Tests\n"
        ])