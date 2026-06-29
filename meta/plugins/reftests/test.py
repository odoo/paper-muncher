from pathlib import Path
from cutekit import const
from enum import Enum

TEST_REPORT = (Path(const.PROJECT_CK_DIR) / "tests" / "report").absolute()

SUPPORTED_PROPS = {
    "name": None,
    "width": "200px",
    "height": "200px",
    "flow": None,
    "margins": None,
    "skip": None,
}

SUPPORTED_CASE_PROPS = {
    "help": None,
    "skip": None,
}

class TestReference:
    def __init__(self, path: Path, imagePath: Path, image: bytes):
        self.path = path  # path to the reference document
        self.imagePath = imagePath
        self.image = image

_currentTestId = 0

class TestStatus(Enum):
    PASSED = "passed",
    FAILED = "failed",
    SKIPPED = "skipped"

class TestCase:
    id: int

    width: str | None
    height: str | None
    flow: str | None
    help: str | None
    skipped: bool

    tag: str
    testDocument: str
    inputPath: Path
    outputPath: Path
    addInfos: list[str]


    def __init__(self, props: dict[str, str], tag: str, testDocument: str,
                 caseProps: dict[str, str]):
        for p in props:
            if p not in SUPPORTED_PROPS:
                raise RuntimeError(f"Unsupported property on test: {p}")

        for p in caseProps:
            if p not in SUPPORTED_CASE_PROPS:
                raise RuntimeError(f"Unsupported property on test case: {p}")

        self.ref = None
        global _currentTestId
        self.addInfos = []
        self.id = _currentTestId
        _currentTestId += 1

        self.width = props.get("width", SUPPORTED_PROPS["width"])
        self.height = props.get("height", SUPPORTED_PROPS["height"])
        self.flow = props.get("flow", SUPPORTED_PROPS["flow"])
        self.margins = props.get("margins", SUPPORTED_PROPS["margins"])
        self.help = caseProps.get("help", SUPPORTED_CASE_PROPS["help"])
        self.skipped = caseProps.get("skip", SUPPORTED_CASE_PROPS["skip"]) == "true"

        self.inputPath = TEST_REPORT / f"{self.id}.xhtml"  # path to test case's document
        self.outputPath = TEST_REPORT / f"{self.id}.bmp"  # path to the output image
        self.tag = tag  # test tag [rendering | error]
        self.testDocument = testDocument

    def render(self, paperMuncher) -> bytes:
        with self.inputPath.open("w") as f:
            f.write(f"<!DOCTYPE html>\n{self.testDocument}")

        runPaperMuncher(paperMuncher, self)

        with self.outputPath.open("rb") as imageFile:
            return imageFile.read()

    def run(self, paperMuncher, reference: TestReference) -> bool:
        output = self.render(paperMuncher)
        self.ref = reference
        return areImagesIdentical(reference.image, output) == (self.tag == "rendering")


def runPaperMuncher(executable, test: TestCase):
    command = ["--feature", "*=on", "--quiet"]

    if test.flow:
        command.extend(["--flow", test.flow])

    if test.width:
        command.extend(["--width", test.width])

    if test.height:
        command.extend(["--height", test.height])

    if test.margins:
        command.extend(["--margins", test.margins])

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
