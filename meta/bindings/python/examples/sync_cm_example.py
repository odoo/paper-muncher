from paper_muncher.synchronous import rendered


html = """
<h1>Hello, Paper Muncher!</h1>
<p>This is a simple example of using Paper Muncher in a synchronous context.</p>
"""


def main():
    with rendered(html, mode="print") as (pdf_io_stream, std_err):
        pdf = pdf_io_stream.read()

    with open("output_sync.pdf", "wb") as f:
        f.write(pdf)


if __name__ == "__main__":
    main()
