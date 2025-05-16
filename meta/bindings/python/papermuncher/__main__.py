
if __name__ == "__main__":
    # get the arguments from the command line
    import argparse
    import logging

    parser = argparse.ArgumentParser(description="Paper Muncher CLI")
    parser.add_argument(
        "--test",
        action="store_true",
        help="Run the test environment with mocked data"
    )
    parser.add_argument(
        "--test-enable",
        action="store_true",
        help="Enable run the full test suite"
    )
    # print the given arguments
    args = parser.parse_args()

    if args.test:
        from .testing import TestEnvironMocked
        from .bindings import PaperMuncher
        logger = logging.getLogger(__name__)
        logger.setLevel(logging.DEBUG)

        env = TestEnvironMocked(
            html="<html><body><h1>Hello, World!</h1></body></html>",
            data_dir={
                '/style.css': 'body { background-color: #0000ff; }',
            }
        )
        pm = PaperMuncher(env, mode='print')
        html_content = """<html>
        <head>
            <link rel="stylesheet" href="/style.css">
            <link rel="stylesheet2" href="/not_found.css">
        </head>
        <body>
            <h1>Hello, World!</h1>
        </body>
        </html>"""
        pdf_bytes = pm.to_pdf(html_content)
        with open("output.pdf", "wb") as f:
            f.write(pdf_bytes)

    elif args.test_enable:
        from .tests import run_tests
        run_tests()
