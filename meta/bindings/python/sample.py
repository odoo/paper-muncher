import papermuncher

with open("out.pdf", "wb") as f:
    document = """
        <p>Hello, world!</p>
    """
    f.write(
        papermuncher.print(
            document,
            paper="a4",
        )
    )
