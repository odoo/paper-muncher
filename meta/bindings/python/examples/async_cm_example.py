from paper_muncher.asynchronous import rendered


html = """
<h1>Hello, Paper Muncher!</h1>
<p>This is a simple example of using Paper Muncher in an asynchronous context.</p>
"""


async def main():
    async with rendered(html, mode="print") as (pdf_stream_reader, std_err):
        pdf = await pdf_stream_reader.read()

    with open("output_async.pdf", "wb") as f:
        f.write(pdf)


if __name__ == "__main__":
    import asyncio
    asyncio.run(main())
