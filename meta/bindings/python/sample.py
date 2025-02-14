import papermuncher
from pathlib import Path

def sample(document, output_filename):
    try:
        output = papermuncher.printPM(
            document,
            "paper-muncher",
            papermuncher.StaticDir(Path.cwd()),
            "--httpipe",
            paper="a4",
        )
    except Exception as e:
        import traceback
        print(
            f'Error while running paper-muncher:\n{"".join(traceback.format_tb(e.__traceback__))}\n{e}\n'
        )

    with open(output_filename, "wb") as f:
        f.write(output)


sample(
    """
    <html>
        <head>
            <link type="text/css" rel="stylesheet" href="./404.css"/>
        </head>
        <p>Hello, world!</p>
    </html>
    """,
    "css_not_found.pdf"
)

sample(
    """
    <p>Hello, world!</p>
    """,
    "no_css.pdf"
)

sample(
    """
    <html>
        <head>
            <link type="text/css" rel="stylesheet" href="./test.css"/>
        </head>
        <p>Hello, world!</p>
    </html>
    """,
    "with_css.pdf"
)
