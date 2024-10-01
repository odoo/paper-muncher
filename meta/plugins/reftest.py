from cutekit import shell, vt100, cli, builder, model
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
TEST_REPORT = TESTS_DIR / "report"


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
    for file in TESTS_DIR.glob(args.glob or "*/*.xhtml"):
        if file.suffix != ".xhtml":
            continue
        print(f"Running comparison test {file}...")

        with file.open() as f:
            content = f.read()

        Num = 0
        for info, test in re.findall(r"""<test([^>]*)>([\w\W]+?)</test>""", content):
            props = getInfo(info)
            print(f"{vt100.WHITE}Test {props.get('name')!r}{vt100.RESET}")
            if "skip" in props:
                print(f"{vt100.YELLOW}Skip test{vt100.RESET}")
                continue
            Num += 1
            temp_file_name = re.sub(r"[^\w.-]", "_", f"{file}-{props.get('id') or Num}")

            search = re.search(r"""<container>([\w\W]+?)</container>""", test)
            container = search and search.group(1)

            expected_xhtml = None
            expected_image: bytes | None = None
            expected_image_url = TEST_REPORT / f"{temp_file_name}.expected.bmp"
            if props.get("id"):
                ref_image = file.parent / f"{props.get('id')}.bmp"
                if ref_image.exists():
                    with ref_image.open("rb") as imageReader:
                        expected_image = imageReader.read()

                    with expected_image_url.open("wb") as imageWriter:
                        imageWriter.write(expected_image)

                    expected_image_url = ref_image

            num = 0
            for tag, info, rendering in re.findall(
                r"""<(rendering|error)([^>]*)>([\w\W]+?)</(?:rendering|error)>""", test
            ):
                num += 1
                renderingProps = getInfo(info)
                if "skip" in renderingProps:
                    print(f"{vt100.YELLOW}Skip test{vt100.RESET}")
                    continue

                input_path = TEST_REPORT / f"{counter}.xhtml"

                update_temp_file(input_path, container, rendering)

                # generate temporary bmp
                img_path = TEST_REPORT / f"{counter}.bmp"

                if props.get("size") == "full":
                    paperMuncher.popen("render", "-sdlpo", img_path, input_path)
                else:
                    size = props.get("size", "200")
                    paperMuncher.popen(
                        "render",
                        "--width",
                        size,
                        "--height",
                        size,
                        "-sdlpo",
                        img_path,
                        input_path,
                    )

                paperMuncher.popen(
                    "print",
                    "-sdlpo",
                    TEST_REPORT / f"{counter}.pdf",
                    input_path,
                )

                with img_path.open("rb") as imageFile:
                    output_image: bytes = imageFile.read()

                # the first template is the expected value
                if not expected_xhtml:
                    expected_xhtml = rendering
                    if not expected_image:
                        expected_image = output_image
                        with (TEST_REPORT / f"{temp_file_name}.expected.bmp").open(
                            "wb"
                        ) as imageWriter:
                            imageWriter.write(expected_image)
                        continue

                # check if the rendering is different
                assert expected_image is not None
                assert output_image is not None

                ok = compareImages(expected_image, output_image) == (tag == "rendering")

                help = renderingProps.get("help")

                if ok:
                    passed += 1
                    # img_path.unlink()
                    print(f"{counter}: {help}: {vt100.GREEN}Passed{vt100.RESET}")
                else:
                    failed += 1

                    print()
                    print(f"{counter}: {help}: {vt100.RED}Failed{vt100.RESET}")
                    # generate temporary file for debugging

                    print(f"file://{input_path}")
                    print(f"file://{TEST_REPORT / 'report.html'}#case-{counter}")
                    print()

                report += f"""
                <div id="case-{counter}" class="test-case {ok and 'passed' or 'failed'}">
                    <h2>{counter} - {tag} - {props.get('name')}</h2>
                    <p>{help}</p>
                    <div class="outputs">
                        <div>
                            <img class="expected" src="{expected_image_url}" />
                            <figcaption>{'Expected' if (tag == 'rendering') else 'Unexpected'}</figcaption>
                        </div>

                        <div>
                            <img class="actual" src="{TEST_REPORT / f'{counter}.bmp'}" />
                            <figcaption>Actual</figcaption>
                        </div>

                        <div>
                            <iframe src="{input_path}" style="background-color: white; width: 200px; height: 200px;"></iframe>
                            <figcaption>Rendition</figcaption>
                        </div>
                    </div>
                    <a href="{TEST_REPORT / f'{counter}.pdf'}">PDF</a>
                    <a href="{expected_image_url}">Expected</a>
                    <a href="{input_path}">Source</a>
                </div>
                <hr />
                """

                counter += 1

                if args.fast:
                    break

    report += """
    </body>
    <style>
        body {
            font-family: sans-serif;
            background-color: #18181b;
            color: #fafafa
        }

        .test-case {
            padding: 8px;
            border-radius: 4px;
        }

        .passed {
        }

        .failed {
            border: 2px solid #991b1b;
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
