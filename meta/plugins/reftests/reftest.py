from cutekit import vt100, cli, builder, model, const
from pathlib import Path
import xml.etree.ElementTree as ET
import dataclasses as dc
import copy

# Local imports
from .reporters.web import WebReport
from .reporters.cli import CLIReport
from .reporters.dispatcher import ReportDispatcher
from .reporters.base import Reporter, TestStatus
from .test import TestCase, TestReference

SOURCE_DIR: Path = Path(__file__).parent
TESTS_DIR: Path = SOURCE_DIR.parent.parent.parent / "tests"
TEST_REPORT: Path = (Path(const.PROJECT_CK_DIR) / "tests" / "report").absolute()


def buildPaperMuncher(args: model.TargetArgs) -> builder.ProductScope:
    """
    Build paper-muncher with the given target arguments for later use in reftests.

    Args:
        args: The target arguments, which define the context for the build.

    Returns:
        The ProductScope result from building paper-muncher.

    Raises:
        RuntimeError: If the "paper-muncher" component cannot be found.
    """

    scope = builder.TargetScope.use(args)
    PmComponent = scope.registry.lookup("paper-muncher", model.Component)
    if PmComponent is None:
        raise RuntimeError("paper-muncher not found")
    return builder.build(scope, PmComponent)[0]


class RefTestArgs(model.TargetArgs):
    glob: str = cli.arg("g", "glob", "Glob pattern to match test files")
    headless: bool = cli.arg(
        None, "headless", "Run the tests without opening the report."
    )
    fast: bool = cli.arg(
        None, "fast", "Proceed to the next test as soon as an error occurs."
    )
    runSkipped: bool = cli.arg(None, "run-skipped", "Run the skipped tests nonetheless")


class TestResults:
    """Tracks test execution results."""

    def __init__(self) -> None:
        self.passed: int = 0
        self.failed: int = 0
        self.skipped: int = 0
        self.failedDetails: str = ""

    def addPassed(self) -> None:
        self.passed += 1

    def addFailed(self, details: str = "") -> None:
        self.failed += 1
        if details:
            self.failedDetails += details

    def addSkipped(self) -> None:
        self.skipped += 1

    def addWith(self, other: 'TestResults') -> None:
        """add another TestResults into this one."""
        self.passed += other.passed
        self.failed += other.failed
        self.skipped += other.skipped
        self.failedDetails += other.failedDetails


class TestRunnerContext:
    """Context for test execution containing shared state and configuration."""

    def __init__(self, args: RefTestArgs, paperMuncher: builder.ProductScope) -> None:
        self.args: RefTestArgs = args
        self.paperMuncher: builder.ProductScope = paperMuncher
        self.currentTestId: int = 0
        self.results: TestResults = TestResults()

    def nextTestId(self) -> int:
        """Get the current test ID and increment for next test."""
        testId = self.currentTestId
        self.currentTestId += 1
        return testId

    def shouldRunSkipped(self) -> bool:
        """Check if skipped tests should be run."""
        return self.args.runSkipped

    def shouldStopOnFailure(self) -> bool:
        """Check if execution should stop on first failure."""
        return self.args.fast


DEFAULT_CONTAINER = '<container><html xmlns="http://www.w3.org/1999/xhtml"><body><slot /></body></html></container>'

@dc.dataclass
class TestCategory:
    path : Path
    props: dict[str, str]
    testCases: list[TestCase]

class TestParser:
    """Handles all test file parsing operations."""

    @staticmethod
    def _getNs(el):
        if el.tag.startswith("{"):
            return el.tag[1:el.tag.index("}")]
        return None

    @staticmethod
    def _setNamespace(el, ns):
        el.tag = f"{{{ns}}}{el.tag}" if not el.tag.startswith("{") else el.tag
        for child in el:
            TestParser._setNamespace(child, ns)

    @staticmethod
    def parseTestFile(path: Path) -> list[TestCategory]:
        """Parse individual test cases (rendering/error tags)."""

        tree = ET.parse(path)

        tests = tree.getroot()

        if (tests.tag != "tests"):
            raise RuntimeError(f"{path}: Root element should be <tests>")

        categories: list[TestCategory] = []

        for test in tests:
            testCases: list[TestCase] = []

            if (test.tag != "test"):
                raise RuntimeError(f"{path}: Child of <tests> is <{test.tag}> instead of <test>")

            container = test.find("container")
            if container is None:
                container = ET.fromstring(DEFAULT_CONTAINER)

            html = container.find(".//{*}html")
            if html is None:
                raise RuntimeError(f"{path}: Root of container should be <html>")

            for case in test:
                if case.tag not in ("rendering", "error"):
                    continue

                htmlCopy = copy.deepcopy(html)

                slotCopy = htmlCopy.find(".//{*}slot")
                parent = htmlCopy.find(".//{*}slot/..")
                assert parent is not None and slotCopy is not None

                ns = TestParser._getNs(parent)
                slotIndex = list(parent).index(slotCopy)
                parent.remove(slotCopy)

                for i, child in enumerate(case):
                    childCopy = copy.deepcopy(child)
                    if ns:
                        TestParser._setNamespace(childCopy, ns)
                    parent.insert(slotIndex + i, childCopy)

                doc = ET.tostring(htmlCopy, encoding="unicode")
                testCase = TestCase(test.attrib, case.tag, doc, case.attrib)
                testCases.append(testCase)

            categories.append(TestCategory(path, test.attrib, testCases))

        return categories

class TestRunner:
    """Handles test execution logic."""

    def __init__(self, context: TestRunnerContext, reporter: Reporter) -> None:
        self._context: TestRunnerContext = context
        self._reporter: Reporter = reporter
        self._parser: TestParser = TestParser()

    def _generateReferenceImage(self, testCase: TestCase) -> TestReference:
        """Generate reference image from a test case."""
        output = testCase.render(self._context.paperMuncher)
        return TestReference(
            testCase.inputPath,
            testCase.outputPath,
            output
        )

    def _runSingleTestCase(self, test: TestCase, reference: TestReference, skipped: bool = False) -> bool:
        """Run a single test case and report results."""
        if skipped:
            test.addInfos.append("skip flag")

        ok: bool = test.run(self._context.paperMuncher, reference)
        if not ok:
            failureDetails: str = f"""Test {test.id} failed.
            file://{test.inputPath}
            file://{TEST_REPORT / "report.html"}#case-{test.id}
            """
            self._context.results.addFailed(failureDetails)
        else:
            self._context.results.addPassed()

        self._reporter.addTestCase(test, TestStatus.PASSED if ok else TestStatus.FAILED)

        return ok

    def _runTestCategory(self, category: TestCategory, file: Path) -> TestResults:
        categoryResults: TestResults = TestResults()
        reference: TestReference | None = None

        for test in category.testCases:
            # First test case is the reference
            if not reference:
                reference = self._generateReferenceImage(test)
                continue

            testSkipped: bool = category.props.get("skip", None) == "true" or test.skipped

            if testSkipped and not self._context.shouldRunSkipped():
                categoryResults.addSkipped()
                self._reporter.addTestCase(test, TestStatus.SKIPPED)
                continue

            success: bool = self._runSingleTestCase(test, reference, testSkipped)
            if success:
                categoryResults.addPassed()
            else:
                categoryResults.addFailed()

            if self._context.shouldStopOnFailure():
                break

        self._reporter.addTestCategory(category.props, file, categoryResults)

        return categoryResults

    def runTestFile(self, file: Path) -> TestResults:
        """Run all tests in a file."""
        fileResults: TestResults = TestResults()
        print(f"Running {file.relative_to(TESTS_DIR)}...")

        for category in self._parser.parseTestFile(file):
            categoryResults: TestResults = self._runTestCategory(category, file)
            fileResults.addWith(categoryResults)

        print()
        return fileResults


@cli.command("reftests", "Manage the reftests")
def _() -> None:
    """Placeholder for the reftests command group."""
    ...


@cli.command("reftests/run", "Manage the reftests")
def _(args: RefTestArgs) -> None:
    """Run the reftest suite."""
    paperMuncher: builder.ProductScope = buildPaperMuncher(args)
    manifests: model.Registry = model.Registry.use(args)

    TEST_REPORT.mkdir(parents=True, exist_ok=True)

    reportDispatcher: ReportDispatcher = ReportDispatcher(SOURCE_DIR, TEST_REPORT)
    if not args.headless:
        reportDispatcher.append(WebReport(SOURCE_DIR, TEST_REPORT))
    reportDispatcher.append(CLIReport(SOURCE_DIR, TEST_REPORT))

    context: TestRunnerContext = TestRunnerContext(args, paperMuncher)
    testRunner: TestRunner = TestRunner(context, reportDispatcher)

    for file in TESTS_DIR.glob(args.glob or "**/*.xhtml"):
        fileResults: TestResults = testRunner.runTestFile(file)
        context.results.addWith(fileResults)

    reportDispatcher.finish(manifests, context.results, context)
    if context.results.failed:
        raise RuntimeError("Some tests failed")
