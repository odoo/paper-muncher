from cutekit import shell, vt100, cli, builder, model
from pathlib import Path
import dataclasses as dt
from dataclasses_json import DataClassJsonMixin
import tempfile


def buildPaperMuncher(args: model.TargetArgs) -> builder.ProductScope:
    scope = builder.TargetScope.use(args)
    component = scope.registry.lookup("vaev-tools", model.Component)
    if component is None:
        raise RuntimeError("paper-muncher not found")
    return builder.build(scope, component)[0]


@cli.command(None, "reftests", "Manage the reftests")
def _(args: model.TargetArgs):
    paperMuncher = buildPaperMuncher(args)

    for file in shell.find("tests", ["*.html", "*.xhtml"]):
        print(f"Running reftest {file}...")
        path = Path(file)

        refPath = path.parent / ".ref" / (path.name + ".ref")
        output = paperMuncher.popen("html2pdf", "-sdlpo", "/dev/null", file)

        if not refPath.exists():
            vt100.warning(f"{refPath} not found, creating reference")
            refPath.parent.mkdir(parents=True, exist_ok=True)
            with refPath.open("x") as ref:
                ref.write(output)
            continue

        with refPath.open() as ref:
            refContent = ref.read()

            if refContent == output:
                print(f"{vt100.GREEN}Passed{vt100.RESET}")
            else:
                print(f"{vt100.RED}Failed{vt100.RESET}")
