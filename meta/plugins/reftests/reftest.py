from cutekit import shell, vt100, cli, builder, model, const
from pathlib import Path

import re
import textwrap

# Local imports
from .utils import fetchMessage
from .WebReport import WebReport

SOURCE_DIR: Path = Path(__file__).parent
TESTS_DIR: Path = SOURCE_DIR.parent.parent.parent / "tests"
TEST_REPORT = (Path(const.PROJECT_CK_DIR) / "tests" / "report").absolute()


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


def areImagesIdentical(image1: bytes, image2: bytes) -> bool:
    """
    Compare the results from the reftests by checking if the images are identical.

    This method is sensitive to any changes in the image, including compression artifacts.
    If you want to compare the images with more tolerance use a SSIM.

    Args:
        image1: The byte content of the first image.
        image2: The byte content of the second image.
    """
    return image1 == image2


def runPaperMuncher(executable, type, xsize, ysize, page, outputPath, inputPath):
    command = ["--feature", "*=on", "--quiet"]

    if type == "print":
        command.extend(["--flow", "paginate"])

    if xsize or not page:
        command.extend(["--width", (xsize or 200) + "px"])

    if ysize or not page:
        command.extend(["--height", (ysize or 200) + "px"])

    if page:
        command.extend(["--page", page])

    command += [
        "-o",
        outputPath,
        inputPath,
    ]

    executable.popen(*command)


class RefTestArgs(model.TargetArgs):
    glob: str = cli.arg("g", "glob")
    headless: bool = cli.arg(
        None, "headless", "Run the tests without opening the report."
    )
    fast: str = cli.arg(
        None, "fast", "Proceed to the next test as soon as an error occurs."
    )
    runSkipped: bool = cli.arg(None, "run-skipped", "Run the skipped tests nonetheless")


class TestRunnerContext:
    def __init__(self, args: RefTestArgs, paperMuncher: builder.ProductScope, webReport: WebReport):
        self.webReport = webReport
        self.args = args
        self.paperMuncher = paperMuncher
        self.webReport = webReport
        self.currentTestId: int = 0
        self.testFailed: str = ""


REG_INFO = re.compile(r"""(\w+)=['"]([^'"]+)['"]""")


def getInfo(txt):
    return {prop: value for prop, value in REG_INFO.findall(txt)}


def getTests(txt):
    return re.findall(
        r"""<(rendering|error)([^>]*)>([\w\W]+?)</(?:rendering|error)>""", txt
    )


def reportTestCase(context: TestRunnerContext, ok: bool, tag: str, input_path: Path, referenceImageURL: Path,
                   xsize: int, ysize: int, props, skipped: bool = False):
    add_infos = []
    if skipped:
        add_infos.append("skip flag")
    if len(add_infos) != 0:
        add_infos = " [" + ", ".join(add_infos) + "]"
    else:
        add_infos = ""

    help = props.get("help")
    context.webReport.addTestCase(context.currentTestId, ok, tag, help, input_path, referenceImageURL, xsize,
                                  ysize, add_infos)


def update_temp_file(path, container, rendering):
    # write xhtml into the temporary file
    xhtml = re.sub(r"<slot\s*/>", rendering, container) if container else rendering
    with path.open("w") as f:
        f.write(f"<!DOCTYPE html>\n{textwrap.dedent(xhtml)}")
        

def runTestCategory(context: TestRunnerContext, test_content: str, props, container, file, categorySkipped=False):
    passedCount = 0
    failedCount = 0
    skippedCount = 0

    referenceDocument = None  # Expected reference document content (HTML/XHTML)
    referenceImage: bytes | None = None
    referenceImageURL: Path = TEST_REPORT / f"{context.currentTestId}.expected.bmp"
    if props.get("name"):
        ref_image = file.parent / f"{props.get('name')}.bmp"
        if ref_image.exists():
            with ref_image.open("rb") as imageReader:
                expected_image = imageReader.read()

            with referenceImageURL.open("wb") as imageWriter:
                imageWriter.write(expected_image)

            referenceImageURL = ref_image

    tests = getTests(test_content)

    for tag, info, testDocument in tests:
        renderingProps = getInfo(info)

        testSkipped = categorySkipped or "skip" in renderingProps
        if testSkipped and not context.args.runSkipped:
            skippedCount += 1

            print(f"{vt100.YELLOW}○{vt100.RESET}", end="", flush=True)
            continue

        input_path = TEST_REPORT / f"{context.currentTestId}.xhtml"
        update_temp_file(input_path, container, testDocument)

        # generate temporary bmp
        img_path = TEST_REPORT / f"{context.currentTestId}.bmp"

        xsize = props.get("size", "200")
        ysize = xsize
        page = props.get("page")
        if props.get("size") == "full":
            xsize = "800"
            ysize = "600"

        type = props.get("type")  # the type of test [render (default) | print]
        runPaperMuncher(
            context.paperMuncher, type, xsize, ysize, page, img_path, input_path
        )

        with img_path.open("rb") as imageFile:
            output_image: bytes = imageFile.read()

        # the first template of the category is the reference document
        if not referenceDocument:
            referenceDocument = testDocument
            if not referenceImage:
                referenceImage = output_image
                with (TEST_REPORT / f"{context.currentTestId}.expected.bmp").open(
                        "wb"
                ) as imageWriter:
                    imageWriter.write(referenceImage)
                continue

        # check if the test is valid
        assert referenceImage is not None
        assert output_image is not None

        ok = areImagesIdentical(referenceImage, output_image) == (tag == "rendering")
        if ok:
            passedCount += 1
            print(f"{vt100.GREEN}●{vt100.RESET}", end="", flush=True)
        else:
            failedCount += 1
            print(f"{vt100.RED}●{vt100.RESET}", end="", flush=True)
            context.testFailed += f"""Test {context.currentTestId} failed.
            file://{input_path}
            file://{TEST_REPORT / "report.html"}#case-{context.currentTestId}
            """

        reportTestCase(
            context,
            ok,
            tag,
            input_path,
            referenceImageURL,
            int(xsize),
            int(ysize),
            props,
            skipped=testSkipped,
        )

        context.currentTestId += 1

        if context.args.fast:
            break

    context.webReport.addTestCategory(context.currentTestId, props, file, passedCount, failedCount, skippedCount)
    return passedCount, failedCount, skippedCount


def runTestFile(context: TestRunnerContext, file: Path):
    passedCount = 0
    failedCount = 0
    skippedCount = 0

    print(f"Running {file.relative_to(TESTS_DIR)}...")

    def getContainer(test_content: str) -> str | None:
        searchContainer = re.search(r"""<container>([\w\W]+?)</container>""", test)
        container = searchContainer and searchContainer.group(1)
        if not container:
            container = '<html xmlns="http://www.w3.org/1999/xhtml"><body><slot /></body></html>'
        return container

    with file.open() as f:
        content = f.read()

    for info, test in re.findall(r"""<test([^>]*)>([\w\W]+?)</test>""", content):
        props = getInfo(info)

        categorySkipped = "skip" in props

        if categorySkipped and not context.args.runSkipped:
            skippedCount += 1
            context.webReport.addSkippedFile(context.currentTestId, props)
            print(f"{vt100.YELLOW}○{vt100.RESET}", end="", flush=True)
            continue

        container = getContainer(test)

        catPassed, catFailed, catSkipped = runTestCategory(context, test, props, container, file, categorySkipped)
        passedCount += catPassed
        failedCount += catFailed
        skippedCount += catSkipped

    print()
    return context.currentTestId, passedCount, failedCount, skippedCount


def reportToCLI(manifests, failed, passed, test_failed):
    print()
    if failed:
        print(f"{vt100.BRIGHT_GREEN}// {fetchMessage(manifests, 'witty')}{vt100.RESET}")
        print(
            f"{vt100.RED}Failed {failed} tests{vt100.RESET}, {vt100.GREEN}Passed {passed} tests{vt100.RESET}"
        )
        print(f"Report: {TEST_REPORT / 'report.html'}")

        print()
        print("Failed tests details:")
        print(test_failed)
        raise RuntimeError("Some tests failed")
    else:
        print(f"{vt100.GREEN}// {fetchMessage(manifests, 'nice')}{vt100.RESET}")
        print(f"{vt100.GREEN}All tests passed{vt100.RESET}")
        print(f"Report: {TEST_REPORT / 'report.html'}")


@cli.command("reftests", "Manage the reftests")
def _(): ...  # Placeholder for the reftests command group


@cli.command("reftests/run", "Manage the reftests")
def _(args: RefTestArgs):
    paperMuncher = buildPaperMuncher(args)
    manifests = model.Registry.use(args)

    TEST_REPORT.mkdir(parents=True, exist_ok=True)
    webReport = WebReport(SOURCE_DIR, TEST_REPORT)

    passed = 0
    failed = 0
    skipped = 0

    context = TestRunnerContext(args, paperMuncher,
                                webReport)  # storing these in a context object for easier passing around
    for file in TESTS_DIR.glob(args.glob or "**/*.xhtml"):
        testId, filePassed, fileFailed, fileSkipped = runTestFile(context, file)
        passed += filePassed
        failed += fileFailed
        skipped += fileSkipped

    # Testing ended - reporting results
    if not args.headless:
        if shell.which("xdg-open"):
            shell.exec("xdg-open", str(TEST_REPORT / "report.html"))
        elif shell.which("open"):
            shell.exec("open", str(TEST_REPORT / "report.html"))

    webReport.finish(manifests, failed, passed, skipped)
    reportToCLI(manifests, failed, passed, context.testFailed)
