from pathlib import Path
from cutekit import const

TEST_REPORT = (Path(const.PROJECT_CK_DIR) / "tests" / "report").absolute()
TESTS_DIR = Path("tests")

SUPPORTED_PROPS = {
    "name": None,
    "width": "200px",
    "height": "200px",
    "flow": None,
    "margins": None,
    "scale": None,
    "features": None,
    "skip": None,
    "xfail": None,
}

SUPPORTED_CASE_PROPS = {
    "inputs": None,
    "header": None,
    "header-size": None,
    "footer": None,
    "footer-size": None,
    "help": None,
    "skip": None,
    "xfail": None,
}

class TestReference:
    def __init__(self, path: Path, imagePath: Path, image: bytes | None = None):
        self.path = path  # path to the reference document
        self.imagePath = imagePath
        self.image = image

_currentTestId = 0

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
    outputImage: bytes | None
    addInfo: list[str]


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
        self.outputImage: bytes | None = None
        self.addInfos = []
        self.panicInfo: str | None = None  # Store engine panic details
        self.id = _currentTestId
        _currentTestId += 1

        self.width = props.get("width", SUPPORTED_PROPS["width"])
        self.height = props.get("height", SUPPORTED_PROPS["height"])
        self.flow = props.get("flow", SUPPORTED_PROPS["flow"])
        self.margins = props.get("margins", SUPPORTED_PROPS["margins"])
        self.inputs = caseProps.get("inputs", SUPPORTED_CASE_PROPS["inputs"])
        self.header = caseProps.get("header", SUPPORTED_CASE_PROPS["header"])
        self.headerSize = caseProps.get("header-size", SUPPORTED_CASE_PROPS["header-size"])
        self.footer = caseProps.get("footer", SUPPORTED_CASE_PROPS["footer"])
        self.footerSize = caseProps.get("footer-size", SUPPORTED_CASE_PROPS["footer-size"])
        self.help = caseProps.get("help", SUPPORTED_CASE_PROPS["help"])
        self.skipped = caseProps.get("skip", SUPPORTED_CASE_PROPS["skip"]) == "true"
        self.scale = props.get("scale", SUPPORTED_PROPS["scale"])
        self.features = props.get("features", SUPPORTED_PROPS["features"])
        self.xfail = "true" in (props.get("xfail"), caseProps.get("xfail"))

        self.inputPath: Path = TEST_REPORT / f"{self.id}.xhtml"  # path to test case's document
        self.outputPath: Path = TEST_REPORT / f"{self.id}.bmp"  # path to the output image
        self.tag = tag  # test tag [rendering | error]
        self.testDocument = testDocument

    def render(self, paperMuncher):  #
        with self.inputPath.open("w") as f:
            if self.inputs:
                links = "".join(f'<li><a href="{TESTS_DIR / i}">{i}</a></li>' for i in self.inputs.split(","))
                f.write(f'<!DOCTYPE html>\n<html xmlns="http://www.w3.org/1999/xhtml"><body><ul>{links}</ul></body></html>')
            else:
                f.write(f"<!DOCTYPE html>\n{self.testDocument}")

        runPaperMuncher(paperMuncher, self)

        if self.panicInfo:
            self.addInfos.append("PANIC")
        elif self.outputPath.exists():
            with self.outputPath.open("rb") as imageFile:
                self.outputImage = imageFile.read()
        else:
            self.panicInfo = "Engine produced no output"
            self.addInfos.append("NO OUTPUT")

    def run(self, paperMuncher, reference: TestReference) -> bool:
        self.render(paperMuncher)
        self.ref = reference
        return areImagesIdentical(reference.image, self.outputImage) == (self.tag == "rendering")


def runPaperMuncher(executable, test: TestCase):
    command = ["--feature", "*=on", "wkhtmltopdf-quirks-*=off"]
    if test.features:
        command.extend(test.features.split())
    command.append("--quiet")

    if test.flow:
        command.extend(["--flow", test.flow])

    if test.width:
        command.extend(["--width", test.width])

    if test.height:
        command.extend(["--height", test.height])

    if test.margins:
        command.extend(["--margins", test.margins])

    if test.scale:
        command.extend(["--scale", test.scale])

    headers = test.header.split(",") if test.header else []
    footers = test.footer.split(",") if test.footer else []
    inputs = [TESTS_DIR / i for i in test.inputs.split(",")] if test.inputs else [test.inputPath]

    if len(headers) == 1:
        command.extend(["--header", TESTS_DIR / headers[0]])
        headers = []

    if len(footers) == 1:
        command.extend(["--footer", TESTS_DIR / footers[0]])
        footers = []

    if test.headerSize:
        command.extend(["--header-size", test.headerSize])

    if test.footerSize:
        command.extend(["--footer-size", test.footerSize])

    command += ["-o", test.outputPath]

    for index, input in enumerate(inputs):
        command.append(input)
        if index < len(headers) and headers[index] != "none":
            command.extend(["--header", TESTS_DIR / headers[index]])
        if index < len(footers) and footers[index] != "none":
            command.extend(["--footer", TESTS_DIR / footers[index]])

    try:
        executable.popen(*command)
    except Exception as panic_error:
        test.panicInfo = f"Process error: {str(panic_error)[:200]}"


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
