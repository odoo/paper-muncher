"""This module defines the base Message class,
which is used to handle Pseudo HTTP messages.
"""


import asyncio

MAX_BUFFER_SIZE = 8192


class Message:
    """
    This class is a blueprint to handle Pseudo HTTP messages.
    It provides methods to read HTTP-style headers and chunked transfer
    encoding from a stream.
    """
    __slots__ = ("headers",)
    headers: dict[str, str]


    def __init__(self):
        self.headers = {}

    async def _read_header_lines(self, reader: asyncio.StreamReader) -> list[str]:
        """
        Asynchronously read HTTP-style header lines from a stream until an
        empty line is encountered.

        Args:
            reader (asyncio.StreamReader): The stream reader to read from.

        Returns:
            list[str]: A list of header lines as decoded UTF-8 strings.

        Raises:
            EOFError: If the stream ends unexpectedly.
        """

        lines = []
        while True:
            request_line = await reader.readline()
            if not request_line:
                raise EOFError("Input stream has ended")
            request_line = request_line.decode("utf-8")
            if request_line == "\r\n":
                break
            lines.append(request_line)
        return lines

    def _add_to_header(self, header_line: str) -> None:
        """
        Parse a header line and add the key-value pair to the headers
        dictionary.

        Args:
            header_line (str): A single HTTP header line, e.g.,
            "Content-Type: text/plain".
        """

        key, value = header_line.split(":", 1)
        self.headers[key.strip()] = value.strip()

    async def _read_single_chunk(self, reader: asyncio.StreamReader) -> bytes:
        """
        Asynchronously read a single HTTP chunk from a stream.

        Args:
            reader (asyncio.StreamReader): The stream reader to read from.

        Returns:
            bytes: The raw bytes of the chunk.

        Raises:
            EOFError: If the stream ends unexpectedly while reading the chunk 
            or CRLF.
            ValueError: If the chunk is not properly terminated with CRLF.
        """

        size_line = await reader.readline()
        if not size_line:
            raise EOFError("Unexpected end of file while reading chunk size.")
        size = int(size_line.strip(), 16)
        if size == 0:
            # consume trailing empty line
            await reader.readexactly(2)
            return b""

        rem_size = size
        chunk = b""
        while rem_size > 0:
            bs = min(MAX_BUFFER_SIZE, rem_size)
            data = await reader.read(bs)
            if not data:
                raise EOFError(
                    "Unexpected end of stream while reading chunk data.")
            chunk += data
            rem_size -= len(data)

        crlf = await reader.readexactly(2)
        if crlf != b"\r\n":
            raise ValueError(f"Expected '\\r\\n' after chunk, got {crlf!r}")

        return chunk

    async def read_chunked_body(self, reader: asyncio.StreamReader) -> bytes:
        """
        Asynchronously read an entire HTTP chunked transfer-encoded body.

        Args:
            reader (asyncio.StreamReader): The stream reader to read from.

        Returns:
            bytes: The complete decoded body.
        """

        encoded_body = b""
        while True:
            chunk = await self._read_single_chunk(reader)
            if len(chunk) == 0:
                break
            encoded_body += chunk
        return encoded_body
