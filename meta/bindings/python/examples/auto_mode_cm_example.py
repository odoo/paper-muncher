from paper_muncher import rendered


html = """
<h1>Hello, Paper Muncher!</h1>
<p>This is a simple example of using Paper Muncher in an auto context.</p>
"""

def main_sync():
    with rendered(html, mode="print") as (pdf_io_stream, std_err):
        pdf = pdf_io_stream.read()

    with open("output_sync.pdf", "wb") as f:
        f.write(pdf)

    print("PDF generated and saved as output_sync.pdf")


async def main_async():
    async with rendered(html, mode="print") as (pdf_stream_reader, std_err):
        pdf = await pdf_stream_reader.read()

    with open("output_async.pdf", "wb") as f:
        f.write(pdf)

    print("PDF generated and saved as output_async.pdf")


if __name__ == "__main__":
    main_sync()
    import asyncio
    asyncio.run(main_async())
