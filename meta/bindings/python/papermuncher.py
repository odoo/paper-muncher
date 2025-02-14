import dataclasses as dc
import io
from pathlib import Path
import subprocess
import tempfile
import magic
import os
import logging

_logger = logging.getLogger(__name__)

class Loader:
    def handleRequest(self, url: str) -> tuple[dict[str, str], bytes]:
        raise NotImplementedError()

@dc.dataclass
class StaticDir(Loader):
    _path: Path

    def __init__(self, path: Path):
        self._path = path

    def handleRequest(self, url: str) -> tuple[dict[str, str], bytes]:
        # Http path starts with '/' what ends up being interpreted as the root of the FS when appending, so we rmv it
        path = self._path / url[1:]

        if not path.exists():
            raise FileNotFoundError()
        with open(path, "rb") as f:
            return (
                {
                    "mime": magic.Magic(mime=True).from_file(path),
                },
                f.read(),
            )


MAX_BUFFER_SIZE = 1024
class HttpMessage():

    def __init__(self):
        self.headers = {}

    def _readHeaderLines(self, reader: io.TextIOWrapper) -> list[str]:
        lines = []

        while True:
            request_line = reader.readline().decode('utf-8')

            if len(request_line) == 0:
                raise EOFError("Input stream has ended")

            if request_line == "\r\n":
                break

            lines.append(request_line)

        return lines

    def _addToHeader(self, header_line: str) -> None:
        key, value = header_line.split(':')
        self.headers[key.strip()] = value.strip()

    def readHeader(self, reader: io.TextIOWrapper) -> None:
        raise NotImplementedError()

    def _readSingleChunk(self, reader: io.TextIOWrapper) -> bytes:
        def read_chunk_content(rem_size):
            chunk = b""

            while rem_size > 0:
                bs = min(MAX_BUFFER_SIZE, rem_size)
                byte = reader.read(bs)
                chunk += byte

                rem_size -= bs
            return chunk

        size = int(reader.readline()[:-2])
        chunk = read_chunk_content(size)

        reader.read(2)

        return chunk

    def readChunkedBody(self, reader: io.TextIOWrapper) -> bytes:
        encoded_body = b""
        while True:
            chunk = self._readSingleChunk(reader)

            if chunk is None:
                return None

            if len(chunk) == 0:
                break

            encoded_body += chunk

        return encoded_body

class HttpRequest(HttpMessage):

    def __init__(self, method=None, path=None, version=None):
        super().__init__()
        self.method = method
        self.path = path
        self.version = version

    def readHeader(self, reader: io.TextIOWrapper) -> None:
        header_lines = self._readHeaderLines(reader)
        self.method, self.path, self.version = header_lines[0].split(' ')

        for line in header_lines[1:]:
            self._addToHeader(line)


RESPONSE_MESSAGES = {
    200: 'OK',
    404: 'Not Found'
}

class HttpResponse(HttpMessage):

    def __init__(self, code: int, headers: dict[str, str] = {}, version="1.1"):
        super().__init__()
        self.headers |= headers
        self.version = version
        self.code = code
        self.body = None

    def addHeader(self, key: str, value: str) -> None:
        self.headers[key] = value

    def addBody(self, body: bytes) -> None:
        if not isinstance(body, bytes):
            raise ValueError("Body must be in bytes")
        self.body = body
        self.addHeader("Content-Length", len(body))

    def __bytes__(self) -> bytes:
        def firstLine():
            return f"HTTP/{self.version} {self.code} {RESPONSE_MESSAGES.get(self.code, 'No Message')}".encode()

        def headers():
            return (f"{key}: {value}".encode() for key, value in self.headers.items())

        return b"\r\n".join([firstLine(), *headers(), b"", self.body or b""])


def _run(
    args: list[str],
    loader=Loader(),
) -> bytes:

    def sendResponse(stdin: io.TextIOWrapper, response: HttpResponse):
        stdin.write(bytes(response))
        stdin.flush()

    with subprocess.Popen(
        args,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    ) as proc:
        stdout = proc.stdout
        if stdout is None:
            raise ValueError("stdout is None")

        stderr = proc.stderr
        if stderr is None:
            raise ValueError("stderr is None")

        stdin = proc.stdin
        if stdin is None:
            raise ValueError("stdin is None")

        # The only exception we are recovering from for now is FileNotFound, which is implemented in PM's HttPipe flow
        try:
            while True:
                request = HttpRequest()
                request.readHeader(stdout)

                if request.method == "GET":
                    try:
                        headers, asset = loader.handleRequest(request.path)
                    except FileNotFoundError:
                        response = HttpResponse(404)
                    else:
                        response = HttpResponse(200, headers)
                        response.addBody(asset)

                    sendResponse(stdin, response)
                elif request.method == "POST":
                    payload = request.readChunkedBody(stdout)
                    proc.terminate()
                    return payload
                else:
                    raise ValueError("Invalid request")
        except Exception as e:
            proc.terminate()
            _logger.debug(stderr.read().decode('utf-8'))
            raise e


def printPM(
    document: bytes | str | Path,
    bin: str,
    loader: Loader = StaticDir(Path.cwd()),
    *args: str,
    **kwargs: str,
) -> bytes:

    extraArgs = list(args)
    for key, value in kwargs.items():
        extraArgs.append(f"--{key}")
        extraArgs.append(str(value))

    if isinstance(document, Path):
        return _run(
            [bin, "print", str(document)] + extraArgs,
            loader,
        )
    else:
        with tempfile.NamedTemporaryFile(dir=loader._path) as f:
            if isinstance(document, str):
                document = document.encode()
            f.write(document)
            f.flush()

            return _run(
                [str(bin), "print", os.path.basename(f.name)] + extraArgs,
                loader,
            )


__all__ = ["Loader", "StaticDir", "printPM"]
