"""
The :mod:`.paper_muncher.synchronous.interface` module provides
utilities for interacting with Paper Muncher, a subprocess used to render
HTML content into  or Image format.
"""


import logging
from asyncio import wait_for, TimeoutError as AsyncTimeoutError
from asyncio.subprocess import PIPE as APIPE
from datetime import datetime, timezone
from contextlib import asynccontextmanager
from collections.abc import Generator
from email.utils import format_datetime
from io import BytesIO
from inspect import isawaitable
from itertools import count
from typing import BinaryIO, Optional, Union

from .asyncify import asyncify_runner
from .request import (
    consume_paper_muncher_request,
    read_paper_muncher_request,
)
from .io_with_timeout import (
    read_all_with_timeout,
    write_with_timeout,
)
from .popen import Popen
from ..binary import get_paper_muncher_binary, can_use_paper_muncher

from ..typing import AsyncRunner, Runner

_logger = logging.getLogger(__name__)

AUTHORIZED_MODE = {'print', 'render'}
DEFAULT_READ_TIMEOUT = 60  # seconds
DEFAULT_READLINE_TIMEOUT = 60 * 15  # seconds (15 minutes is for the put request)
DEFAULT_WRITE_TIMEOUT = 30  # seconds
DEFAULT_CHUNK_SIZE = 4096  # bytes
DEFAULT_WAIT_TIMEOUT = 5  # seconds
NOT_RENDERABLE_OPTIONS = {
    'read_timeout',
    'readline_timeout',
    'write_timeout',
    'chunk_size',
    'wait_timeout',
}
SERVER_SOFTWARE = b'Paper Muncher (Fully Asynchronous Engine)'


@asynccontextmanager
async def rendered(
    content: BytesIO,
    mode: str = "print",
    runner: Optional[
        Union[
            AsyncRunner,
            Runner,
        ]
    ] = None,
    **options,
) -> Generator[tuple[BinaryIO], None, None]:
    """Async context manager to render HTML content using Paper Muncher.

    :param content: The HTML content to render, as a BytesIO object.
    :param mode: The rendering mode, either 'print' or 'render'.
    :param runner: Optional AsyncRunner function to handle asset requests.
    :param options: Additional options to pass to Paper Muncher.
    :return: A generator yielding the stdout and stderr streams of the
        Paper Muncher process.
    :raises RuntimeError: If Paper Muncher is not available or crashes.
    :raises ValueError: If an invalid mode is specified.
    """

    if not can_use_paper_muncher():
        raise RuntimeError(
            "Paper Muncher is not available in the current session. "
            "Ensure it is installed and available in the system PATH."
        )

    if not mode in AUTHORIZED_MODE:
        raise ValueError(
            f"Invalid mode '{mode}', must be one of {AUTHORIZED_MODE}"
        )

    readline_timeout = options.get(
        'readline_timeout',
        DEFAULT_READLINE_TIMEOUT,
    )
    write_timeout = options.get('write_timeout', DEFAULT_WRITE_TIMEOUT)
    wait_timeout = options.get('wait_timeout', DEFAULT_WAIT_TIMEOUT)

    extra_args = []
    for option, value in options.items():
        if option in NOT_RENDERABLE_OPTIONS:
            continue
        extra_args.extend([
            f'--{option}', str(value),
        ])

    if not (binary := get_paper_muncher_binary()):
        raise RuntimeError(
            "Paper Muncher binary not found or not usable. "
            "Ensure it is installed and available in the system PATH."
        )

    if runner is not None and not isawaitable(runner):
        runner = asyncify_runner(runner)

    async with Popen(
        [binary, mode, "pipe:", '-o', "pipe:"] + extra_args,
        stdin=APIPE,
        stdout=APIPE,
        stderr=APIPE,
    ) as process:
        # Phase 1: send HTML content headers and body
        try:
            await consume_paper_muncher_request(
                process.stdout,
                timeout=readline_timeout,
            )
        except EOFError as early_eof:
            raise RuntimeError(
                "Paper Muncher terminated prematurely (phase 1)"
            ) from early_eof

        if process.returncode is not None:
            raise RuntimeError(
                "Paper Muncher crashed before receiving content")

        now = datetime.now(timezone.utc)
        response_headers = (
            b"HTTP/1.1 200 OK\r\n"
            b"Content-Length: %(length)d\r\n"
            b"Content-Type: text/html\r\n"
            b"Date: %(date)s\r\n"
            b"Server: %(server)s\r\n"
            b"\r\n"
        ) % {
            b'length': len(content.encode()),
            b'date':  format_datetime(now, usegmt=True).encode(),
            b'server': SERVER_SOFTWARE,
        }

        await write_with_timeout(
            process.stdin,
            response_headers,
            timeout=write_timeout,
        )
        await write_with_timeout(
            process.stdin,
            content.encode(),
            timeout=write_timeout,
        )

        if process.returncode is not None:
            raise RuntimeError(
                "Paper Muncher crashed while sending HTML content")

        # Phase 2: serve asset requests until the rendered content is ready
        for request_no in count(start=1):
            try:
                path = await read_paper_muncher_request(
                    process.stdout, 
                    timeout=readline_timeout,
                )
            except (EOFError, TimeoutError):
                process.kill()
                await process.wait()
                raise

            if path is None:
                break

            for chunk in await runner(path):
                await write_with_timeout(
                    process.stdin,
                    chunk,
                    timeout=write_timeout
                )

            if process.returncode is not None:
                raise RuntimeError(
                    "Paper Muncher crashed while serving asset"
                    f" {request_no}: {path}"
                )

        # Phase 3: send final OK and close the process
        now = datetime.now(timezone.utc)
        final_response = (
            b"HTTP/1.1 200 OK\r\n"
            b"Date: %(date)s\r\n"
            b"Server: %(server)s\r\n"
            b"\r\n"
        ) % {
            b'date': format_datetime(now, usegmt=True).encode(),
            b'server': SERVER_SOFTWARE,
        }

        await write_with_timeout(
            process.stdin,
            final_response,
            timeout=write_timeout,
        )
        try:
            process.stdin.write_eof()
        except (NotImplementedError, AttributeError):
            process.stdin.close()
            await process.stdin.wait_closed()

        if process.returncode is not None:
            raise RuntimeError(
                "Paper Muncher crashed before returning the rendered content"
            )

        try:
            yield process.stdout, process.stderr
        finally:
            try:
                await wait_for(
                    process.wait(),
                    timeout=wait_timeout,
                )
            except AsyncTimeoutError:
                process.kill()
                await process.wait()
                _logger.warning(
                    "Paper Muncher did not terminate in time,"
                    "forcefully killed it"
                )

            if process.returncode != 0:
                _logger.warning(
                    "Paper Muncher exited with code %d",
                    process.returncode,
                )


async def render(
    content: BytesIO,
    mode: str = "print",
    runner: Optional[
        Union[
            AsyncRunner,
            Runner,
        ]
    ] = None,
    **options,
) -> bytes:
    """Render HTML content using Paper Muncher and return the rendered output.

    :param content: The HTML content to render, as a BytesIO object.
    :param mode: The rendering mode, either 'print' or 'render'.
    :param runner: Optional AsyncRunner function to handle asset requests.
    :param options: Additional options to pass to Paper Muncher.
    :return: The rendered content as bytes.
    :raises RuntimeError: If Paper Muncher is not available or crashes.
    :raises ValueError: If an invalid mode is specified.
    """

    async with rendered(
        content,
        mode=mode,
        runner=runner,
        **options,
    ) as (content_stream, error_stream):
        read_timeout = options.get('read_timeout', DEFAULT_READ_TIMEOUT)
        chunk_size = options.get('chunk_size', DEFAULT_CHUNK_SIZE)
        rendered_content = await read_all_with_timeout(
            content_stream,
            chunk_size=chunk_size,
            timeout=read_timeout,
        )
        stderr_output = await read_all_with_timeout(
            error_stream,
            chunk_size=chunk_size,
            timeout=read_timeout,
        )

        if stderr_output:
            _logger.warning(
                "Paper Muncher error output: %s",
                stderr_output.decode('utf-8', errors='replace'),
            )

        if mode == "print":
            if not rendered_content.startswith(b'%PDF-'):
                raise RuntimeError(
                    "Paper Muncher did not return valid PDF content"
                )

        return rendered_content
