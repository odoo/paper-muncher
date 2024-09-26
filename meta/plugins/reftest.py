from cutekit import shell, vt100, cli, builder, model
from pathlib import Path
import re
import textwrap
import time


def buildPaperMuncher(args: model.TargetArgs) -> builder.ProductScope:
    scope = builder.TargetScope.use(args)
    component = scope.registry.lookup("vaev-tools", model.Component)
    if component is None:
        raise RuntimeError("paper-muncher not found")
    return builder.build(scope, component)[0]


class RefTestArgs(model.TargetArgs):
    glob: str = cli.arg("g", "glob")
    fast: str = cli.arg(None, "fast", "Proceed to the next test as soon as an error occurs.")


@cli.command(None, "reftests", "Manage the reftests")
def _(args: RefTestArgs):
    paperMuncher = buildPaperMuncher(args)

    test_folder = Path(__file__).parent.parent.parent / 'tests'
    test_tmp_folder = test_folder / '_local'
    test_tmp_folder.mkdir(parents=True, exist_ok=True)
    # for temp in test_tmp_folder.glob('*.*'):
    #     temp.unlink()

    temp_file = test_tmp_folder / 'reftest.xhtml'
    def update_temp_file(container, rendering):
        # write xhtml into the temporary file
        xhtml = re.sub(r"<slot\s*/>", rendering, container) if container else rendering
        with temp_file.open("w") as f:
            f.write(f"<!DOCTYPE html>\n{textwrap.dedent(xhtml)}")

    REG_INFO = re.compile(r"""(\w+)=['"]([^'"]+)['"]""")
    def getInfo(txt):
        return {prop: value for prop, value in REG_INFO.findall(txt)}

    for file in test_folder.glob(args.glob or "*/*.xhtml"):
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
            expected_image = None
            expected_image_url = test_tmp_folder / f"{temp_file_name}.expected.bmp"
            if props.get('id'):
                ref_image = file.parent / f"{props.get('id')}.bmp"
                if ref_image.exists():
                    with ref_image.open('rb') as f:
                        expected_image = f.read()
                    with expected_image_url.open("wb") as f:
                        f.write(expected_image)
                    expected_image_url = ref_image

            num = 0
            for tag, info, rendering in re.findall(r"""<(rendering|error)([^>]*)>([\w\W]+?)</(?:rendering|error)>""", test):
                num += 1
                renderingProps = getInfo(info)
                if "skip" in renderingProps:
                    print(f"{vt100.YELLOW}Skip test{vt100.RESET}")
                    continue

                update_temp_file(container, rendering)

                # generate temporary bmp
                img_path = test_tmp_folder / f"{temp_file_name}-{num}.bmp"

                if props.get("size") == "full":
                    paperMuncher.popen("render", "-sdlpo", img_path, temp_file)
                else:
                    size = props.get("size", "200")
                    paperMuncher.popen("render", "--width", size, "--height", size, "-sdlpo", img_path, temp_file)

                with img_path.open("rb") as f:
                    output_image = f.read()

                # the first template is the expected value
                if not expected_xhtml:
                    expected_xhtml = rendering
                    if not expected_image:
                        expected_image = output_image
                        with (test_tmp_folder / f"{temp_file_name}.expected.bmp").open("wb") as f:
                            f.write(expected_image)
                        continue

                # check if the rendering is different
                if (expected_image == output_image) == (tag == "rendering"):
                    img_path.unlink()
                    print(f"{vt100.GREEN}Passed{vt100.RESET}")
                else:
                    # generate temporary file for debugging
                    paperMuncher.popen("print", "-sdlpo", test_tmp_folder / f"{temp_file_name}-{num}.pdf", temp_file)

                    help = renderingProps.get("help")
                    if tag == "error":
                        print(f"{vt100.RED}Failed {props.get('name')!r} (The result should be different){vt100.RESET}")
                        print(f"{vt100.WHITE}{expected_xhtml[1:].rstrip()}{vt100.RESET}")
                        print(f"{vt100.WHITE}{expected_image_url}{vt100.RESET}")
                        print(f"{vt100.BLUE}{rendering[1:].rstrip()}{vt100.RESET}")
                        print(f"{vt100.BLUE}{test_tmp_folder / f'{temp_file_name}-{num}.pdf'}{vt100.RESET}")
                        print(f"{vt100.BLUE}{test_tmp_folder / f'{temp_file_name}-{num}.bmp'}{vt100.RESET}")
                        if help:
                            print(f"{vt100.BLUE}{help}{vt100.RESET}")
                    else:
                        print(f"{vt100.RED}Failed {props.get('name')!r}{vt100.RESET}")
                        if expected_xhtml != rendering:
                            print(f"{vt100.WHITE}{expected_xhtml[1:].rstrip()}{vt100.RESET}")
                        print(f"{vt100.WHITE}{expected_image_url}{vt100.RESET}")
                        print(f"{vt100.BLUE}{rendering[1:].rstrip()}{vt100.RESET}")
                        print(f"{vt100.BLUE}{test_tmp_folder / f'{temp_file_name}-{num}.pdf'}{vt100.RESET}")
                        print(f"{vt100.BLUE}{test_tmp_folder / f'{temp_file_name}-{num}.bmp'}{vt100.RESET}")
                        if help:
                            print(f"{vt100.BLUE}{help}{vt100.RESET}")

                    if args.fast:
                        break

    temp_file.unlink()
