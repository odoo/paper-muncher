from cutekit import shell, vt100, cli, builder, model, const
from pathlib import Path
from random import randint
import re
import textwrap
import time


def buildPaperMuncher(args: model.TargetArgs) -> builder.ProductScope:
    scope = builder.TargetScope.use(args)
    component = scope.registry.lookup("vaev-tools", model.Component)
    if component is None:
        raise RuntimeError("paper-muncher not found")
    return builder.build(scope, component)[0]


def fetchFile(args: model.TargetArgs, component: str, path: str) -> str:
    r = model.Registry.use(args)
    c = r.lookup(component, model.Component)
    assert c is not None
    p = Path(c.dirname()) / path
    with p.open() as f:
        return f.read()


def fetchMessage(args: model.TargetArgs, type: str) -> str:
    message = eval("[" + fetchFile(args, "karm-base", "defs/" + type + ".inc") + "]")
    return message[randint(0, len(message) - 1)]


def compareImages(
    lhs: bytes,
    rhs: bytes,
    lowEpsilon: float = 0.05,
    highEpsilon: float = 0.1,
    strict=False,
) -> bool:
    if strict:
        return lhs == rhs

    if len(lhs) != len(rhs):
        return False

    if lhs == rhs:
        return True

    errorSum = 0
    for i in range(len(lhs)):
        diff = abs(lhs[i] - rhs[i]) / 255
        if diff > highEpsilon:
            # print(f"Image rejected with diff = {diff}")
            return False
        errorSum += diff > lowEpsilon

    if errorSum > len(lhs) // 100:
        # print(f"Image reject with errorSum = {errorSum}")
        return False

    return True


class RefTestArgs(model.TargetArgs):
    glob: str = cli.arg("g", "glob")
    fast: str = cli.arg(
        None, "fast", "Proceed to the next test as soon as an error occurs."
    )


@cli.command(None, "reftests", "Manage the reftests")
def _(): ...


TESTS_DIR: Path = Path(__file__).parent.parent.parent / "tests"
TEST_REPORT = (Path(const.PROJECT_CK_DIR) / "tests" / "report").absolute()


@cli.command(None, "reftests/clean", "Manage the reftests")
def _(args: RefTestArgs):
    for f in TEST_REPORT.glob("*.*"):
        f.unlink()
    TEST_REPORT.rmdir()
    print(f"Cleaned {TEST_REPORT}")


@cli.command(None, "reftests/run", "Manage the reftests")
def _(args: RefTestArgs):
    paperMuncher = buildPaperMuncher(args)

    TEST_REPORT.mkdir(parents=True, exist_ok=True)
    report = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>Reftest</title>
    </head>
    <body>
"""

    def update_temp_file(path, container, rendering):
        # write xhtml into the temporary file
        xhtml = re.sub(r"<slot\s*/>", rendering, container) if container else rendering
        with path.open("w") as f:
            f.write(f"<!DOCTYPE html>\n{textwrap.dedent(xhtml)}")

    REG_INFO = re.compile(r"""(\w+)=['"]([^'"]+)['"]""")

    def getInfo(txt):
        return {prop: value for prop, value in REG_INFO.findall(txt)}

    passed, failed = 0, 0

    counter = 0
    for file in TESTS_DIR.glob(args.glob or "**/*.xhtml"):
        if file.suffix != ".xhtml":
            continue
        print(f"Running comparison test {file}...")

        with file.open() as f:
            content = f.read()

        passCount = 0
        failCount = 0
        for info, test in re.findall(r"""<test([^>]*)>([\w\W]+?)</test>""", content):
            props = getInfo(info)
            print(f"{vt100.WHITE}Test {props.get('name')!r}{vt100.RESET}")
            if "skip" in props:
                report += f"""
                <div>
                    <div id="case-{counter}" class="test skipped">
                        <h2>{props.get('name') or "Unamed"}</h2>
                        <p>Skipped</p>
                    </div>
                <div>
                """
                print(f"{vt100.YELLOW}Skip test{vt100.RESET}")
                continue

            test_report = ""

            search = re.search(r"""<container>([\w\W]+?)</container>""", test)
            container = search and search.group(1)
            if not container:
                container = '<html xmlns="http://www.w3.org/1999/xhtml"><body><slot /></body></html>'

            expected_xhtml = None
            expected_image: bytes | None = None
            expected_image_url = TEST_REPORT / f"{counter}.expected.bmp"
            if props.get("name"):
                ref_image = file.parent / f"{props.get('name')}.bmp"
                if ref_image.exists():
                    with ref_image.open("rb") as imageReader:
                        expected_image = imageReader.read()

                    with expected_image_url.open("wb") as imageWriter:
                        imageWriter.write(expected_image)

                    expected_image_url = ref_image

            for tag, info, rendering in re.findall(
                r"""<(rendering|error)([^>]*)>([\w\W]+?)</(?:rendering|error)>""", test
            ):
                renderingProps = getInfo(info)
                if "skip" in renderingProps:
                    print(f"{vt100.YELLOW}Skip test{vt100.RESET}")
                    continue

                input_path = TEST_REPORT / f"{counter}.xhtml"

                update_temp_file(input_path, container, rendering)

                # generate temporary bmp
                img_path = TEST_REPORT / f"{counter}.bmp"

                xsize = props.get("size", "200")
                ysize = xsize
                if props.get("size") == "full":
                    xsize = "800"
                    ysize = "600"

                paperMuncher.popen(
                    "render",
                    "--width",
                    xsize + "px",
                    "--height",
                    ysize + "px",
                    "-o",
                    img_path,
                    input_path,
                )

                with img_path.open("rb") as imageFile:
                    output_image: bytes = imageFile.read()

                # the first template is the expected value
                if not expected_xhtml:
                    expected_xhtml = rendering
                    if not expected_image:
                        expected_image = output_image
                        with (TEST_REPORT / f"{counter}.expected.bmp").open(
                            "wb"
                        ) as imageWriter:
                            imageWriter.write(expected_image)
                        continue

                # check if the rendering is different
                assert expected_image is not None
                assert output_image is not None

                ok = compareImages(expected_image, output_image) == (tag == "rendering")
                if ok:
                    passCount += 1
                else:
                    failCount += 1

                help = renderingProps.get("help")

                if ok:
                    passed += 1
                    print(f"{counter}: {help}: {vt100.GREEN}Passed{vt100.RESET}")
                else:
                    failed += 1

                    print()
                    print(f"{counter}: {help}: {vt100.RED}Failed{vt100.RESET}")

                    print(f"file://{input_path}")
                    print(f"file://{TEST_REPORT / 'report.html'}#case-{counter}")
                    print()

                test_report += f"""
                <div id="case-{counter}" class="test-case {ok and 'passed' or 'failed'}">
                    <h2>{counter} - {tag}</h2>
                    <p>{help}</p>
                    <div class="outputs">
                        <div>
                            <img class="actual" src="{TEST_REPORT / f'{counter}.bmp'}" />
                            <figcaption>Actual</figcaption>
                        </div>

                        <div>
                            <img class="expected" src="{expected_image_url}" />
                            <figcaption>{'Reference' if (tag == 'rendering') else 'Unexpected'}</figcaption>
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

                counter += 1

                if args.fast:
                    break
            report += f"""
                <div>
                    <div id="test-{counter}" class="test {failCount and 'failed' or 'passed'}">
                        <h1>{props.get('name')}</h2>
                        <p>{props.get('help') or ""}</p>
                        <a href="{file}">Source</a>
                        <span>{passCount} passed, {failCount} failed</span>
                    </div>
                    {test_report}
                </div>
                """

    report += """
    </body>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: sans-serif;
            background-color: #09090b;
            color: #fafafa
        }

        .test {
            padding: 8px;
            background-color: #18181b;
            border-bottom: 1px solid #27272a;
            position: sticky;
            top: 0;
            z-index: 100;
        }

        .test-case {
            padding: 8px;
            border-bottom: 1px solid #333;
        }

        .passed {
        }

        .failed {
            background-color: #450a0a;
        }

        .outputs {
            display: flex;
            gap: 8px;
        }

        .actual {
            border: 1px solid blue;
        }

        iframe {
            border: none;
        }
    </style>
    </html>
    """

    with (TEST_REPORT / "report.html").open("w") as f:
        f.write(report)

    print()
    if failed:
        print(f"{vt100.BRIGHT_GREEN}// {fetchMessage(args, 'witty')}{vt100.RESET}")
        print(
            f"{vt100.RED}Failed {failed} tests{vt100.RESET}, {vt100.GREEN}Passed {passed} tests{vt100.RESET}"
        )
    else:
        print(f"{vt100.GREEN}// {fetchMessage(args, 'nice')}{vt100.RESET}")
        print(f"{vt100.GREEN}All tests passed{vt100.RESET}")
    print(f"Report: {TEST_REPORT / 'report.html'}")

    if shell.which("xdg-open"):
        shell.exec("xdg-open", str(TEST_REPORT / "report.html"))
    elif shell.which("open"):
        shell.exec("open", str(TEST_REPORT / "report.html"))
