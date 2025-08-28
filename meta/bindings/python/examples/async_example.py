from paper_muncher.asynchronous import render


html = """
<h1>Hello, Paper Muncher!</h1>
<p>This is a simple example of using Paper Muncher in an asynchronous context.</p>
"""


async def main():
    pdf_bytes = await render(html, mode="print")
    with open("output_async.pdf", "wb") as f:
        f.write(pdf_bytes)

    print("PDF generated and saved as output_async.pdf")


if __name__ == "__main__":
    import asyncio
    asyncio.run(main())
