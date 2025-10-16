from cutekit import shell, vt100, cli, builder, model, const
from pathlib import Path
from random import randint
import re
import textwrap
import time


def buildPaperMuncher(args: model.TargetArgs) -> builder.ProductScope:
    scope = builder.TargetScope.use(args)
    component = scope.registry.lookup("paper-muncher", model.Component)
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
    message = eval(
        "[" + fetchFile(args, "karm-core", "base/defs/" + type + ".inc") + "]"
    )
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


def runPaperMuncher(executable, type, xsize, ysize, page, outputPath, inputPath):
    command = ["--feature", "*=on", "--verbose", "--unsecure", type or "render"]

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


@cli.command("reftests", "Manage the reftests")
def _(): ...


TESTS_DIR: Path = Path(__file__).parent.parent.parent / "tests"
TEST_REPORT = (Path(const.PROJECT_CK_DIR) / "tests" / "report").absolute()


@cli.command("reftests/clean", "Manage the reftests")
def _():
    for f in TEST_REPORT.glob("*.*"):
        f.unlink()
    TEST_REPORT.rmdir()
    print(f"Cleaned {TEST_REPORT}")


@cli.command("reftests/run", "Manage the reftests")
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
        <header>
            Reftest report
        </header>
"""

    def update_temp_file(path, container, rendering):
        # write xhtml into the temporary file
        xhtml = re.sub(r"<slot\s*/>", rendering, container) if container else rendering
        with path.open("w") as f:
            f.write(f"<!DOCTYPE html>\n{textwrap.dedent(xhtml)}")

    REG_INFO = re.compile(r"""(\w+)=['"]([^'"]+)['"]""")

    def getInfo(txt):
        return {prop: value for prop, value in REG_INFO.findall(txt)}

    passed = 0
    failed = 0
    skipped = 0

    counter = 0
    for file in TESTS_DIR.glob(args.glob or "**/*.xhtml"):
        if file.suffix != ".xhtml":
            continue
        print(f"Running comparison test {file}...")

        with file.open() as f:
            content = f.read()

        passCount = 0
        failCount = 0
        skippedCount = 0
        for info, test in re.findall(r"""<test([^>]*)>([\w\W]+?)</test>""", content):
            props = getInfo(info)
            print(f"{vt100.WHITE}Test {props.get('name')!r}{vt100.RESET}")

            category_skipped = "skip" in props
            type = props.get("type")  # the type of test [render (default) | print]

            if category_skipped and not args.runSkipped:
                skippedCount += 1
                skipped += 1

                report += f"""
                <div>
                    <div id="case-{counter}" class="test skipped">
                        <h2>{props.get("name") or "Unamed"}</h2>
                        <p>Test Skipped</p>
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
                test_skipped = category_skipped or "skip" in renderingProps
                if test_skipped and not args.runSkipped:
                    skippedCount += 1
                    skipped += 1

                    print(f"{vt100.YELLOW}Skip test{vt100.RESET}")
                    continue

                input_path = TEST_REPORT / f"{counter}.xhtml"

                update_temp_file(input_path, container, rendering)

                # generate temporary bmp
                img_path = TEST_REPORT / f"{counter}.bmp"

                xsize = props.get("size", "200")
                ysize = xsize
                page = props.get("page")
                if props.get("size") == "full":
                    xsize = "800"
                    ysize = "600"

                runPaperMuncher(paperMuncher, type, xsize, ysize, page, img_path, input_path)

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

                add_infos = []
                if test_skipped:
                    add_infos.append("skip flag")
                if len(add_infos) != 0:
                    add_infos = " [" + ", ".join(add_infos) + "]"
                else:
                    add_infos = ""

                test_report += f"""
                <div id="case-{counter}" class="test-case {ok and "passed" or "failed"}">
                    <div class="infoBar"></div>
                    <h2>{counter} - {tag} {add_infos}</h2>
                    <p>{help}</p>
                    <div class="outputs">
                        <div>
                            <img class="actual" src="{TEST_REPORT / f"{counter}.bmp"}" />
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

                counter += 1

                if args.fast:
                    break
            report += f"""
                <div class=wrapper>
                    <div id="test-{counter}" class="test {failCount and "failed" or "passed"}">
                        <h1>{props.get("name")}</h2>
                        <p>{props.get("help") or ""}</p>
                        <a href="{file}">Source</a>
                        <span>{passCount} passed, {failCount} failed and {skippedCount} skipped</span>
                    </div>
                    {test_report}
                </div>
                """
    report += f"""
        <footer>
        <p class="witty">{fetchMessage(args, "witty" if failed else "nice")}</p>
        <p> Failed {failed} tests, Passed {passed} tests, Skipped {skipped}</p>
        </footer>
    """

    report += """
    </body>
    <script>
    function initTheme(){
    const prefersDarkScheme = window.matchMedia("(prefers-color-scheme: dark)").matches;
    if (prefersDarkScheme){
        document.body.classList.remove("light");
        document.body.classList.add("dark");

    }else{
        document.body.classList.add("light");
        document.body.classList.remove("dark");
    }
    }
    initTheme();
    </script>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            --bg: #1b1b1c;
            --bg2: #161616;
            --font: #fafafa;
            --failed: #c52b2b;
            --passed: #74b553;
        }
        
        body.light {
            --bg: #f3eee7;
            --bg2: #f7ece7;
            --font: #090909;
            --failed: #c52b2b;
            --passed: #74b553;
        }

        header {
            padding: 8px;
            background-color: var(--bg2);
            color: #fafafa;
            z-index: 100;
        }

        footer {
            position: fixed;
            bottom: 0;
            left: 0;
            right: 0;
            padding: 8px;
            background-color: var(--bg2);
            z-index: 100;
        }

        .infoBar {
            position: absolute;
            transform: translateY(-1rem);
            height: 100%;
            width: 1rem;
            left: 0;
        }

        .failed .infoBar{
            background: var(--failed);
        }
        
        .passed .infoBar{
            background: var(--passed);
        }

        .dark a:link {
            color: #8bd3ff;
        }
        
        .dark a:visited {
            color: #8e8bff;
        }
        
        .light a:link {
            color: #267eb3;
        }
        
        .light a:visited {
            color: #267eb3;
        }
        
        body {
            font-family: sans-serif;
            background-color: var(--bg);
            color: var(--font);
            font-size: 0.9rem;
        }

        .test {
            padding: 1rem;
            background-color: var(--bg2);
            border-bottom: 1px solid #4f4f4f;
            position: sticky;
            gap: 0.2rem;
            top: 0;
            z-index: 100;
            display: flex;
            flex-direction: column;
            align-items: center;
        }

        h1 {
            font-size: 1.2rem;
            text-decoration: underline;
        }
        
        h2 {
            font-size: 1.1rem;
        }

        .wrapper{
            width:fit-content;
        }

        .test-case {
            padding: 1rem;
            padding-left: 2rem;
            border-bottom: 1px solid #333;
            width: fit-content;
            min-width: 100vw;
        }

        .passed {
        }

        .failed {
        }

        .outputs {
            margin: 1.2rem 0;
            display: flex;
            gap: 1rem;
            width: fit-content;
        }

        .outputs>div {
            display: flex;
            gap: 0.5rem;
            flex-direction: column-reverse;
            align-items: center;
        }

        .actual {
             border: 0px solid blue; 
        }

        iframe {
            border: none;
        }
    </style>

    <script>
        // Use a braodcast channel to tell other reftest instances to stop
        const id = Math.random().toString(36).substring(7);
        const channel = new BroadcastChannel('reftest');
        channel.onmessage = (event) => {
            if (event.data.id !== id && event.data.msg === 'stop') {
                window.close();
            }
        }
        channel.postMessage({from: id, msg: 'stop'});

    </script>
    </html>
    """

    with (TEST_REPORT / "report.html").open("w") as f:
        f.write(report)

    if not args.headless:
        if shell.which("xdg-open"):
            shell.exec("xdg-open", str(TEST_REPORT / "report.html"))
        elif shell.which("open"):
            shell.exec("open", str(TEST_REPORT / "report.html"))

    print()
    if failed:
        print(f"{vt100.BRIGHT_GREEN}// {fetchMessage(args, 'witty')}{vt100.RESET}")
        print(
            f"{vt100.RED}Failed {failed} tests{vt100.RESET}, {vt100.GREEN}Passed {passed} tests{vt100.RESET}"
        )
        print(f"Report: {TEST_REPORT / 'report.html'}")
        raise RuntimeError("Some tests failed")
    else:
        print(f"{vt100.GREEN}// {fetchMessage(args, 'nice')}{vt100.RESET}")
        print(f"{vt100.GREEN}All tests passed{vt100.RESET}")
        print(f"Report: {TEST_REPORT / 'report.html'}")
