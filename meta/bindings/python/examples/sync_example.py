from paper_muncher.synchronous import render


html = """
<h1>Hello, Paper Muncher!</h1>
<p>This is a simple example of using Paper Muncher in a synchronous context.</p>
"""


def main():
    pdf_bytes = render(html, mode="print")
    with open("output.pdf", "wb") as f:
        f.write(pdf_bytes)
    print("PDF generated and saved as output.pdf")


if __name__ == "__main__":
    main()
