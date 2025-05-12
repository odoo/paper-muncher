import asyncio

from .message import Message


class PaperMuncherRequest(Message):
    __slots__ = super().__slots__ + ("method", "path", "version")

    def __init__(self, method=None, path=None, version=None):
        """
        Initialize a PaperMuncherRequest object.

        Args:
            method (str, optional): HTTP method (e.g., "GET").
            path (str, optional): Request path (e.g., "/index.html").
            version (str, optional): HTTP version (e.g., "1.1").
        """
        super().__init__()
        self.method = method
        self.path = path
        self.version = version

    async def read_header(self, reader: asyncio.StreamReader) -> None:
        """
        Asynchronously read the HTTP request header from a stream.

        Parses the request line and headers, storing them in the object's attributes.

        Args:
            reader (asyncio.StreamReader): The stream to read the header from.

        Raises:
            ValueError: If the request line is malformed.
            EOFError: If the stream ends unexpectedly.
        """
        header_lines = await self._read_header_lines(reader)

        if not header_lines or len(
            splitted := header_lines[0].strip().split(" ")
        ) != 3:
            raise ValueError("Malformed HTTP request line.")

        self.method, self.path, self.version = splitted

        for line in header_lines[1:]:
            self._add_to_header(line)
