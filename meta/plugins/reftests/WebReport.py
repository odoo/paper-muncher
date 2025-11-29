from cutekit import model
from pathlib import Path

from .utils import fetchMessage


class WebReport:
    """
    Object to abstract the generation of the web report for the reftests.
    """

    def __init__(self, SOURCE_DIR: Path, TEST_REPORT: Path):
        self.TEST_REPORT: Path = TEST_REPORT
        self.html = f"""
            <!DOCTYPE html>
            <html>
            <head>
                <title>Reftest</title>
                <script src="{SOURCE_DIR}/report.js"></script>
                <link rel="stylesheet" href="{SOURCE_DIR}/report.css" />
            </head>
            <body>
                <header>
                    Reftest report
                </header>
        """
        self.testHtml = ""

    def addTestCase(self, testId: int, passed: bool, tag: str, help: str, input_path: Path, expected_image_url: Path,
                    xsize: int, ysize: int, add_infos: str):
        self.testHtml += f"""
                <div id="case-{testId}" class="test-case {passed and "passed" or "failed"}">
                    <div class="infoBar"></div>
                    <h2>{testId} - {tag} {add_infos}</h2>
                    <p>{help}</p>
                    <div class="outputs">
                        <div>
                            <img class="actual" src="{self.TEST_REPORT / f"{testId}.bmp"}" />
                            <figcaption>Actual</figcaption>
                        </div>

                        <div>
                            <img class="expected" src="{expected_image_url}" />
                            <figcaption>{"Reference" if (tag == "rendering") else "Unexpected"}</figcaption>
                        </div>

                        <div>
                            <iframe src="{input_path}" style="background-color: white; width: {xsize}px; height: {ysize}px;"></iframe>
                            <figcaption>Rendition</figcaption>
                        </div>
                    </div>
                    <a href="{expected_image_url}">Reference</a>
                    <a href="{input_path}">Source</a>
                </div>
                """

    def addTestCategory(self, testId: int, props, file: Path, passCount: int, failCount: int, skippedCount: int):
        self.html += f"""
                <div class=wrapper>
                    <div id="test-{testId}" class="test {failCount and "failed" or "passed"}">
                        <h1>{props.get("name")}</h2>
                        <p>{props.get("help") or ""}</p>
                        <a href="{file}">Source</a>
                        <span>{passCount} passed, {failCount} failed and {skippedCount} skipped</span>
                    </div>
                    {self.testHtml}
                </div>
                """
        self.testHtml = ""

    def addSkippedFile(self, testId: int, props):
        self.html += f"""
                <div>
                    <div id="case-{testId}" class="test skipped">
                        <h2>{props.get("name") or "Unamed"}</h2>
                        <p>Test Skipped</p>
                    </div>
                <div>
                """

    def finish(self, manifests: model.Registry, totalFailed: int, totalPassed: int, totalSkipped: int):
        self.html += f"""
        <footer>
        <p class="witty">{fetchMessage(manifests, "witty" if totalFailed != 0 else "nice")}</p>
        <p> Failed {totalFailed} tests, Passed {totalPassed} tests, Skipped {totalSkipped}</p>
        </footer>

        </body>
        </html>
        """
        with (self.TEST_REPORT / "report.html").open("w") as f:
            f.write(self.html)
