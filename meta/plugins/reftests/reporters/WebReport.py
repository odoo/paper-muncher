from cutekit import model, shell
from pathlib import Path

from ..utils import fetchMessage
from ..Test import TestCase
from .reporter import Reporter


class WebReport(Reporter):
    """
    Object to abstract the generation of the web report for the reftests.
    """

    def __init__(self, source_dir: Path, test_report: Path):
        self.test_report: Path = test_report
        self.html = f"""
            <!DOCTYPE html>
            <html>
            <head>
                <title>Reftest</title>
                <script src="{source_dir}/report.js"></script>
                <link rel="stylesheet" href="{source_dir}/report.css" />
            </head>
            <body>
                <header>
                    Reftest report
                </header>
        """
        self.testHtml = ""

    def addTestCase(self, test: TestCase, passed: bool):

        addInfos = " - ".join(test.addInfos)
        self.testHtml += f"""
                <div id="case-{test.id}" class="test-case {passed and "passed" or "failed"}">
                    <div class="infoBar"></div>
                    <h2>{test.id} - {test.tag} {addInfos}</h2>
                    <p>{test.help}</p>
                    <div class="outputs">
                        <div>
                            <img class="actual" src="{self.test_report / f"{test.id}.bmp"}" />
                            <figcaption>Actual</figcaption>
                        </div>

                        <div>
                            <img class="expected" src="{test.ref.imagePath}" />
                            <figcaption>{"Reference" if (test.tag == "rendering") else "Unexpected"}</figcaption>
                        </div>

                        <div>
                            <iframe src="{test.inputPath}" style="background-color: white; width: {test.xsize}px; height: {test.ysize}px;"></iframe>
                            <figcaption>Rendition</figcaption>
                        </div>
                    </div>
                    <a href="{test.ref.path}">Reference</a>
                    <a href="{test.inputPath}">Source</a>
                </div>
                """

    def addSkippedCase(self, test: TestCase):
        pass

    def addTestCategory(self, props, file: Path, results):
        self.html += f"""
                <div class=wrapper>
                    <div id="test-" class="test {results.failed and "failed" or "passed"}">
                        <h1>{props.get("name")}</h1>
                        <p>{props.get("help") or ""}</p>
                        <a href="{file}">Source</a>
                        <span>{results.passed} passed, {results.failed} failed and {results.skipped} skipped</span>
                    </div>
                    {self.testHtml}
                </div>
                """
        self.testHtml = ""

    def addSkippedFile(self, props):
        self.html += f"""
                <div>
                    <div id="case-" class="test skipped">
                        <h2>{props.get("name") or "Unnamed"}</h2>
                        <p>Test Skipped</p>
                    </div>
                </div>
                """

    def finish(self, manifests: model.Registry, results, context):
        self.html += f"""
        <footer>
        <p class="witty">{fetchMessage(manifests, "witty" if results.failed != 0 else "nice")}</p>
        <p> Failed {results.failed} tests, Passed {results.passed} tests, Skipped {results.skipped}</p>
        </footer>

        </body>
        </html>
        """
        with (self.test_report / "report.html").open("w") as f:
            f.write(self.html)

        # Automatically open the report in the default browser
        if shell.which("xdg-open"):
            shell.exec("xdg-open", str(self.test_report / "report.html"))
        elif shell.which("open"):
            shell.exec("open", str(self.test_report / "report.html"))
