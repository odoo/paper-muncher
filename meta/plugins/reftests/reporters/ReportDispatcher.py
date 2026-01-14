from cutekit import model, vt100, const
from pathlib import Path

from ..Test import TestCase
from .reporter import Reporter


class ReportDispatcher(Reporter):
    """Dispatch all reporting operations to multiple reporters."""

    def __init__(self, source_dir: Path, test_report: Path) -> None:
        self._reporters: list[Reporter] = []

    def append(self, reporter: Reporter) -> None:
        """Append a reporter to the list of reporters to use."""
        self._reporters.append(reporter)

    def addTestCase(self, test: TestCase, passed: bool) -> None:
        """Report a single test case result."""
        for reporter in self._reporters:
            reporter.addTestCase(test, passed)

    def addSkippedCase(self, test: TestCase) -> None:
        """Report a skipped test case."""
        for reporter in self._reporters:
            reporter.addSkippedCase(test)

    def addSkippedFile(self, props: dict[str, str]) -> None:
        """Report a skipped test file."""
        for reporter in self._reporters:
            reporter.addSkippedFile(props)

    def addTestCategory(self, props: dict[str, str], file: Path, results) -> None:
        """Report results for a test category."""
        for reporter in self._reporters:
            reporter.addTestCategory(props, file, results)

    def finish(self, manifests: model.Registry, results, context) -> None:
        """Finish reporting and display final results."""
        for reporter in self._reporters:
            reporter.finish(manifests, results, context)
