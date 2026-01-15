from cutekit import model, vt100, const
from pathlib import Path

from ..utils import fetchMessage
from ..Test import TestCase
from .reporter import Reporter


class CLIReport(Reporter):
    """
    Object to abstract the generation of the cli report for the reftests.
    """

    def __init__(self, source_dir: Path, test_report: Path):
        self.test_report: Path = test_report

    def addTestCase(self, test: TestCase, passed: bool):
        if passed:
            print(f"{vt100.GREEN}●{vt100.RESET}", end="", flush=True)
        else:
            print(f"{vt100.RED}●{vt100.RESET}", end="", flush=True)

    def addTestCategory(self, props, file: Path, results):
        pass

    def addSkippedFile(self, props):
        print(f"{vt100.YELLOW}○{vt100.RESET}", end="", flush=True)

    def addSkippedCase(self, test: TestCase):
        print(f"{vt100.YELLOW}○{vt100.RESET}", end="", flush=True)

    def finish(self, manifests: model.Registry, results, context):
        print()
        if results.failed:
            print(
                f"{vt100.BRIGHT_GREEN}// {fetchMessage(manifests, 'witty')}{vt100.RESET}"
            )
            print(
                f"{vt100.RED}Failed {results.failed} tests{vt100.RESET}, {vt100.GREEN}Passed {results.passed} tests{vt100.RESET}"
            )
            print(f"Report: {self.test_report / 'report.html'}")

            print()
            print("Failed tests details:")
            print(results.failedDetails)

        else:
            print(f"{vt100.GREEN}// {fetchMessage(manifests, 'nice')}{vt100.RESET}")
            print(f"{vt100.GREEN}All tests passed{vt100.RESET}")
            print(f"Report: {self.test_report / 'report.html'}")
