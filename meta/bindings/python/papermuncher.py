import dataclasses as dc
from email.message import Message
from pathlib import Path
from email.parser import BytesParser
import subprocess
import tempfile
from typing import IO
import magic


class Loader:
    def handleRequest(
        self, url: str, headers: dict[str, str]
    ) -> tuple[int, dict[str, str], bytes]:
        return (
            404,
            {
                "mime": "text/html",
            },
            b"<html><body>404 Not Found</body></html>",
        )


@dc.dataclass
class StaticDir(Loader):
    _path: Path

    def __init__(self, path: Path):
        self._path = path

    def handleRequest(
        self, url: str, headers: dict[str, str]
    ) -> tuple[int, dict[str, str], bytes]:
        path = self._path / url
        if not path.exists():
            return (
                404,
                {
                    "mime": "text/html",
                },
                b"<html><body>404 Not Found</body></html>",
            )
        with open(path, "rb") as f:
            return (
                200,
                {
                    "mime": magic.Magic(mime=True).from_file(path),
                },
                f.read(),
            )


def _run(
    args: list[str],
    loader=Loader(),
) -> bytes:
    def _readRequest(fd: IO) -> Message[str, str] | None:
        # Read the request header from the file descriptor
        parser = BytesParser()
        return parser.parse(fd)

    def _sendResponse(fd: IO, status: int, headers: dict[str, str], body: bytes):
        fd.write(f"HTTP/2 {status}\r\n".encode())
        for key, value in headers.items():
            fd.write(f"{key}: {value}\r\n".encode())
        fd.write(b"\r\n")
        fd.write(body)

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

        while True:
            request = _readRequest(stdout)
            if request is None:
                raise ValueError("request is None")

            if request.preamble is None:
                raise ValueError("request.preamble is None")

            preamble = request.preamble.split(" ")
            if preamble[0] == b"GET":
                _sendResponse(stdin, *loader.handleRequest(preamble[1], dict(request)))
            elif preamble[0] == b"POST":
                payload = request.get_payload()
                if not isinstance(payload, bytes):
                    raise ValueError("payload is not bytes")
                proc.terminate()
                return payload
            else:
                raise ValueError("Invalid request")


def find() -> Path:
    return Path(__file__).parent / "bin"


def print(
    document: bytes | str | Path,
    mime: str = "text/html",
    loader: Loader = StaticDir(Path.cwd()),
    bin: Path = find(),
    **kwargs: str,
) -> bytes:
    extraArgs = []
    for key, value in kwargs.items():
        extraArgs.append(f"--{key}")
        extraArgs.append(str(value))

    if isinstance(document, Path):
        return _run(
            [str(bin), "print", "-i", str(document), "-o", "out.pdf"] + extraArgs,
            loader,
        )
    else:
        with tempfile.NamedTemporaryFile(delete=False) as f:
            if isinstance(document, str):
                document = document.encode()
            f.write(document)
            return _run(
                [str(bin), "print", "-i", f.name, "-o", "out.pdf"] + extraArgs,
                loader,
            )
        return b""


__all__ = ["Loader", "StaticDir", "print"]
