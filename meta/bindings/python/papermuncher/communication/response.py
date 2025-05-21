from http import HTTPStatus
from io import BytesIO

from .message import Message, MAX_BUFFER_SIZE


class BindingResponse(Message):
    __slots__ = super().__slots__ + ("version", "code", "body")


    def __init__(self, code: int, version="1.1", body=None):
        """
        Initialize a BindingResponse object representing an HTTP response.

        Args:
            code (int): HTTP status code (e.g., 200, 404).
            version (str): HTTP version, default is "1.1".
            body (bytes, optional): Optional response body.

        Raises:
            TypeError: If body is provided but is not in bytes.
        """
        super().__init__()
        self.version = version
        self.code = code
        if body:
            self.addBody(body)
        else:
            self.body = None

    def addHeader(self, key: str, value: str) -> None:
        """
        Add a header key-value pair to the response.

        Args:
            key (str): The header name.
            value (str): The header value.
        """
        self.headers[key] = value

    def addBody(self, body: bytes) -> None:
        """
        Set the body of the response and automatically add the Content-Length header.

        Args:
            body (bytes): The response body.

        Raises:
            TypeError: If body is not a bytes object.
        """
        if not isinstance(body, bytes) and not isinstance(body, BytesIO):
            raise TypeError("Body must be in bytes")
        self.body = body
        self.addHeader("Content-Length", len(body))

    def __iter__(self) -> bytes:
        """
        Convert the response into raw bytes suitable for sending over a socket.

        Returns:
            bytes: The full HTTP response including status line, headers, and body.
        """
        def first_line():
            return f"HTTP/{self.version} {self.code} {HTTPStatus(self.code).phrase}".encode()

        def headers():
            return (f"{key}: {value}".encode() for key, value in self.headers.items())

        if self.body and isinstance(self.body, bytes):
            response = b"\r\n".join([first_line(), *headers(), b"", self.body])
            while response:
                yield response[:MAX_BUFFER_SIZE]
                response = response[MAX_BUFFER_SIZE:]
        elif self.body and isinstance(self.body, BytesIO):
            self.body.seek(0)
            yield b"\r\n".join([first_line(), *headers(), b"", b""])
            while chunk := self.body.read(MAX_BUFFER_SIZE):
                yield chunk
        else:
            yield b"\r\n".join([first_line(), *headers(), b""])
