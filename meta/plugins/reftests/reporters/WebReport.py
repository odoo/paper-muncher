from cutekit import model, shell
from pathlib import Path

from ..utils import fetchMessage
from ..Test import TestCase
from .reporter import Reporter

ICON_SUN = '<svg class="theme-icon theme-icon--light" viewBox="0 0 24 24"><path d="M12,7A5,5 0 0,1 17,12A5,5 0 0,1 12,17A5,5 0 0,1 7,12A5,5 0 0,1 12,7M12,9A3,3 0 0,0 9,12A3,3 0 0,0 12,15A3,3 0 0,0 15,12A3,3 0 0,0 12,9M12,2L14.39,5.42C13.65,5.15 12.84,5 12,5C11.16,5 10.35,5.15 9.61,5.42L12,2M3.34,7L7.5,6.65C6.9,7.16 6.36,7.78 5.94,8.5C5.5,9.24 5.25,10 5.11,10.79L3.34,7M3.36,17L5.12,13.23C5.26,14 5.53,14.78 5.95,15.5C6.37,16.24 6.91,16.86 7.5,17.37L3.36,17M20.65,7L18.88,10.79C18.74,10 18.47,9.23 18.05,8.5C17.63,7.78 17.1,7.15 16.5,6.64L20.65,7M20.64,17L16.5,17.36C17.09,16.85 17.62,16.22 18.04,15.5C18.46,14.77 18.73,14 18.87,13.21L20.64,17M12,22L9.59,18.56C10.33,18.83 11.14,19 12,19C12.82,19 13.63,18.83 14.37,18.56L12,22Z"/></svg>'
ICON_MOON = '<svg class="theme-icon theme-icon--dark" viewBox="0 0 24 24"><path d="M2 12A10 10 0 0 0 15 21.54A10 10 0 0 1 15 2.46A10 10 0 0 0 2 12Z"/></svg>'
ICON_OPEN_IN_NEW = '<svg viewBox="0 0 24 24"><path d="M14,3V5H17.59L7.76,14.83L9.17,16.24L19,6.41V10H21V3M19,19H5V5H12V3H5C3.89,3 3,3.9 3,5V19A2,2 0 0,0 5,21H19A2,2 0 0,0 21,19V12H19V19Z"/></svg>'


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
                <script src="{source_dir}/report.js" defer></script>
                <link rel="stylesheet" href="{source_dir}/report.css" />
            </head>
            <body>
                <header>
                    <img src="{source_dir}/logo.png" alt="Reftest Logo" height="40"/>
                    <h1>Reftest Report</h1>
                    <button class="theme-toggle" onclick="toggleTheme()" title="Toggle dark/light mode">
                        {ICON_SUN}
                        {ICON_MOON}
                    </button>
                </header>
                <main>
        """
        self.testHtml = ""

    def addTestCase(self, test: TestCase, passed: bool):
        addInfos = " - ".join(test.addInfos)
        self.testHtml += f"""
                <details id="case-{test.id}" class="test-case {passed and "passed" or "failed"}">
                    <summary>
                        <div class="test-header">
                            <div class="test-info">
                                <div class="test-title">
                                    <h2>{test.id} - {test.help} <span class="chip {test.tag}">{test.tag}</span> {addInfos}</h2>
                                    <a href="{test.inputPath}" class="icon-link" title="Open source">{ICON_OPEN_IN_NEW}</a>
                                </div>
                            </div>
                        </div>
                    </summary>
                    <div class="outputs">
                        <div>
                            <img class="actual" data-src="{self.test_report / f"{test.id}.bmp"}" />
                            <figcaption>Actual</figcaption>
                        </div>

                        <div>
                            <img class="expected" data-src="{test.ref.imagePath}" />
                            <figcaption>{"Reference" if (test.tag == "rendering") else "Unexpected"}</figcaption>
                        </div>

                        <div>
                            <div class="iframe-placeholder" data-src="{test.inputPath}" style="width: {test.xsize}px; height: {test.ysize}px;"></div>
                            <figcaption>Rendition</figcaption>
                        </div>
                    </div>
                </details>
                """

    def addSkippedCase(self, test: TestCase):
        pass

    def addTestCategory(self, props, file: Path, results):
        passed_chip = f'<span class="chip passed">{results.passed} passed</span>' if results.passed else ""
        failed_chip = f'<span class="chip failed">{results.failed} failed</span>' if results.failed else ""
        skipped_chip = f'<span class="chip skipped">{results.skipped} skipped</span>' if results.skipped else ""
        self.html += f"""
                <details class="wrapper">
                    <summary class="test-case {results.failed and "failed" or "passed"}">
                        <div class="test-header">
                            <div class="test-info">
                                <div class="test-title">
                                    <h1>{props.get("name")}</h1>
                                    <a href="{file}" class="icon-link" title="Open source">{ICON_OPEN_IN_NEW}</a>
                                    <span class="test-stats">
                                        {passed_chip}
                                        {failed_chip}
                                        {skipped_chip}
                                    </span>
                                </div>
                            </div>
                        </div>
                    </summary>
                    {self.testHtml}
                </details>
                """
        self.testHtml = ""

    def addSkippedFile(self, props):
        self.html += f"""
                <div class="wrapper">
                    <div class="test-case skipped">
                        <div class="test-header">
                            <div class="test-info">
                                <div class="test-title">
                                    <h1>{props.get("name") or "Unnamed"}</h1>
                                    <span class="chip skipped">skipped</span>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
                """

    def finish(self, manifests: model.Registry, results, context):
        status = "passed" if results.failed == 0 else "failed"
        status_message = "All tests passed!" if results.failed == 0 else f"{results.failed} test{'s' if results.failed > 1 else ''} failed"
        overview = f"""
                <section class="overview {status}">
                    <div>
                        <h2>{status_message}</h2>
                        <p class="witty">{fetchMessage(manifests, "witty" if results.failed != 0 else "nice")}</p>
                    </div>
                    <div class="summary">
                        <div class="stat passed">
                            <span class="stat-value">{results.passed}</span>
                            passed
                        </div>
                        <div class="stat failed">
                            <span class="stat-value">{results.failed}</span>
                            failed
                        </div>
                        <div class="stat">
                            <span class="stat-value">{results.skipped}</span>
                            skipped
                        </div>
                    </div>
                </section>
        """
        # Insert overview right after <main>
        self.html = self.html.replace("<main>", f"<main>\n{overview}", 1)
        self.html += """
        </main>
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
