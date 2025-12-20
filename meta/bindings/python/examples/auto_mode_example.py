from paper_muncher import render


html = """
<h1>Hello, Paper Muncher!</h1>
<p>This is a simple example of using Paper Muncher in an auto context.</p>
"""

async def main_async():
    pdf_bytes = await render(html, mode="print")
    with open("output_async.pdf", "wb") as f:
        f.write(pdf_bytes)

    print("PDF generated and saved as output_async.pdf")


def main_sync():
    pdf_bytes = render(html, mode="print")
    with open("output_sync.pdf", "wb") as f:
        f.write(pdf_bytes)

    print("PDF generated and saved as output_sync.pdf")


if __name__ == "__main__":
    main_sync()
    import asyncio
    asyncio.run(main_async())
