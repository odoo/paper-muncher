from cutekit import vt100, cli, builder, model, const
from pathlib import Path

import re

# Local imports
from .reporters.WebReport import WebReport
from .reporters.CLIReport import CLIReport
from .reporters.ReportDispatcher import ReportDispatcher
from .reporters.reporter import Reporter
from .Test import TestCase, TestReference

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


REG_INFO = re.compile(r"""(\w+)=['"]([^'"]+)['"]""")
REG_TESTS = re.compile(r"""<(rendering|error)([^>]*)>([\w\W]+?)</(?:rendering|error)>""")
REG_TEST_BLOCKS = re.compile(r"""<test([^>]*)>([\w\W]+?)</test>""")
REG_CONTAINER = re.compile(r"""<container>([\w\W]+?)</container>""")

DEFAULT_CONTAINER = '<html xmlns="http://www.w3.org/1999/xhtml"><body><slot /></body></html>'


class TestParser:
    """Handles all test file parsing operations."""

    @staticmethod
    def parseProperties(text: str) -> dict[str, str]:
        """Parse properties from XML-like attributes."""
        return {prop: value for prop, value in REG_INFO.findall(text)}

    @staticmethod
    def parseTestCases(content: str) -> list[tuple[str, str, str]]:
        """Parse individual test cases (rendering/error tags)."""
        return REG_TESTS.findall(content)

    @staticmethod
    def parseTestBlocks(content: str) -> list[tuple[str, str]]:
        """Parse test blocks from file content."""
        return REG_TEST_BLOCKS.findall(content)

    @staticmethod
    def extractContainer(content: str) -> str:
        """Extract container from test content or return default."""
        match = REG_CONTAINER.search(content)
        if match:
            return match.group(1)
        return DEFAULT_CONTAINER


class TestRunner:
    """Handles test execution logic."""

    def __init__(self, context: TestRunnerContext, reporter: Reporter) -> None:
        self._context: TestRunnerContext = context
        self._reporter: Reporter = reporter
        self._parser: TestParser = TestParser()

    def _generateReferenceImage(self, testCase: TestCase) -> TestReference:
        """Generate reference image from a test case."""
        testCase.render()
        return TestReference(
            testCase.inputPath,
            testCase.outputPath,
            testCase.outputImage
        )

    def _runSingleTestCase(self, test: TestCase, skipped: bool = False) -> bool:
        """Run a single test case and report results."""
        testId: int = self._context.currentTestId

        if skipped:
            test.addInfos.append("skip flag")

        ok: bool = test.run()
        if not ok:
            failureDetails: str = f"""Test {testId} failed.
            file://{test.inputPath}
            file://{TEST_REPORT / "report.html"}#case-{testId}
            """
            self._context.results.addFailed(failureDetails)
        else:
            self._context.results.addPassed()

        self._reporter.addTestCase(test, ok)
        self._context.currentTestId += 1

        return ok

    def _runTestCategory(self, test_content: str, props: dict[str, str],
                         container: str, file: Path, categorySkipped: bool = False) -> TestResults:
        """Run all test cases in a category."""
        categoryResults: TestResults = TestResults()
        testCases: list[tuple[str, str, str]] = self._parser.parseTestCases(test_content)
        reference: TestReference | None = None

        for tag, info, testDocument in testCases:
            caseProps: dict[str, str] = self._parser.parseProperties(info)
            inputPath: Path = TEST_REPORT / f"{self._context.currentTestId}.xhtml"
            imgPath: Path = TEST_REPORT / f"{self._context.currentTestId}.bmp"

            # First test case is the reference
            if not reference:
                test = TestCase(props, inputPath, imgPath, self._context, tag,
                                testDocument, caseProps, self._context.currentTestId,
                                container=container)
                reference = self._generateReferenceImage(test)
                self._context.currentTestId += 1
                continue

            testSkipped: bool = categorySkipped or "skip" in caseProps

            test = TestCase(props, inputPath, imgPath, self._context, tag,
                            testDocument, caseProps, self._context.currentTestId,
                            container=container, reference=reference)

            if testSkipped and not self._context.shouldRunSkipped():
                categoryResults.addSkipped()
                self._reporter.addSkippedCase(test)
                continue

            success: bool = self._runSingleTestCase(test, testSkipped)
            if success:
                categoryResults.addPassed()
            else:
                categoryResults.addFailed()

            if self._context.shouldStopOnFailure():
                break

        self._reporter.addTestCategory(props, file, categoryResults)

        return categoryResults

    def runTestFile(self, file: Path) -> TestResults:
        """Run all tests in a file."""
        fileResults: TestResults = TestResults()
        print(f"Running {file.relative_to(TESTS_DIR)}...")

        with file.open() as f:
            content: str = f.read()

        testBlocks: list[tuple[str, str]] = self._parser.parseTestBlocks(content)

        for info, test_content in testBlocks:
            props: dict[str, str] = self._parser.parseProperties(info)
            categorySkipped: bool = "skip" in props

            if categorySkipped and not self._context.shouldRunSkipped():
                fileResults.addSkipped()
                self._reporter.addSkippedFile(props)
                continue

            container: str = self._parser.extractContainer(test_content)
            categoryResults: TestResults = self._runTestCategory(
                test_content, props, container, file, categorySkipped
            )
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
