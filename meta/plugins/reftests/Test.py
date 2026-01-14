from pathlib import Path
from cutekit import const
import textwrap
import re

TEST_REPORT = (Path(const.PROJECT_CK_DIR) / "tests" / "report").absolute()


class TestReference:
    def __init__(self, path: Path, imagePath: Path, image: bytes | None = None):
        self.path = path  # path to the reference document
        self.imagePath = imagePath
        self.image = image


_currentTestId = 0


class TestCase:
    def __init__(self, props: dict[str, str], tag: str, testDocument: str,
                 caseProps: dict[str, str], container=None):
        self.ref = None
        global _currentTestId
        self.outputImage: bytes | None = None
        self.addInfos = []
        self.id = _currentTestId
        _currentTestId += 1

        self.type = props.get("type")  # the type of test [render (default) | print]
        self.xsize = props.get("size", "200")
        self.ysize = self.xsize

        if props.get("size") == "full":
            self.xsize = "800"
            self.ysize = "600"

        self.page = props.get("page")  # page size
        self.inputPath: Path = TEST_REPORT / f"{self.id}.xhtml"  # path to test case's document
        self.outputPath: Path = TEST_REPORT / f"{self.id}.bmp"  # path to the output image
        self.help = caseProps.get("help", "")
        self.skipped = caseProps.get("skip", False)
        self.tag = tag  # test tag [rendering | error]

        if not container:
            container = '<html xmlns="http://www.w3.org/1999/xhtml"><body><slot /></body></html>'

        self.container = container
        self.testDocument = testDocument

    def render(self, paperMuncher):  #
        def updateTempFile(path, rendering):
            # write xhtml into the temporary file
            xhtml = re.sub(r"<slot\s*/>", rendering, self.container) if self.container else rendering
            with path.open("w") as f:
                f.write(f"<!DOCTYPE html>\n{textwrap.dedent(xhtml)}")

        updateTempFile(self.inputPath, self.testDocument)

        runPaperMuncher(paperMuncher, self)

        with self.outputPath.open("rb") as imageFile:
            self.outputImage = imageFile.read()

    def run(self, paperMuncher, reference: TestReference) -> bool:
        self.render(paperMuncher)
        self.ref = reference
        return areImagesIdentical(reference.image, self.outputImage) == (self.tag == "rendering")


def runPaperMuncher(executable, test: TestCase):
    command = ["--feature", "*=on", "--quiet"]

    if test.type == "print":
        command.extend(["--flow", "paginate"])

    if test.xsize or not test.page:
        command.extend(["--width", (test.xsize or 200) + "px"])

    if test.ysize or not test.page:
        command.extend(["--height", (test.ysize or 200) + "px"])

    if test.page:
        command.extend(["--page", test.page])

    command += [
        "-o",
        test.outputPath,
        test.inputPath,
    ]

    executable.popen(*command)


def areImagesIdentical(image1: bytes, image2: bytes) -> bool:
    """
    Compare the results from the reftests by checking if the images are identical.

    This method is sensitive to any changes in the image, including compression artifacts.
    If you want to compare the images with more tolerance use a SSIM.

    Args:
        image1: The byte content of the first image.
        image2: The byte content of the second image.

    Returns:
        True if the images are identical (byte-for-byte), False otherwise.
    """
    return image1 == image2
