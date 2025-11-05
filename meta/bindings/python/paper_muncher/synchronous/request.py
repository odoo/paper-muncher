"""The :mod:`paper_muncher.synchronous.request`
module provides utilities for consuming and reading
Paper Muncher requests.
It includes functions to read the request line,
and to consume the request headers.
It also handles timeouts for reading lines from the request.
"""


import logging
import time

from typing import BinaryIO, Optional

from .io_with_timeout import readline_with_timeout

_logger = logging.getLogger(__name__)


def remaining_time(deadline: float) -> float:
    remaining = deadline - time.monotonic()
    if remaining <= 0:
        raise TimeoutError("Timeout exceeded")
    return remaining


def consume_paper_muncher_request(
    stdout: BinaryIO,
    timeout: int
) -> None:
    """Read and discard all header lines from a Paper Muncher request.

    :param BinaryIO stdout: File-like stdout stream from Paper Muncher.
    :param int timeout: Timeout in seconds for each line read.
    :return: None
    :rtype: None
    """
    deadline = time.monotonic() + timeout
    while line := readline_with_timeout(
        stdout,
        timeout=remaining_time(deadline)
    ):
        _logger.debug("Paper Muncher request line: %s", line.rstrip())
        if line == b"\r\n":
            return
        if not line:
            raise EOFError("EOF reached while reading request headers")


def read_paper_muncher_request(
    stdout: BinaryIO,
    timeout: int,
) -> Optional[str]:
    """Read the HTTP-like request line from Paper Muncher and return the path.

    :param BinaryIO stdout: File-like stdout stream from Paper Muncher.
    :param int timeout: Timeout in seconds for each line read.
    :return: The requested asset path, or ``None`` if the method is PUT.
    :rtype: str or None
    :raises EOFError: If no request line is found.
    :raises ValueError: If the request format is invalid or the method is
        unsupported.
    """
    deadline = time.monotonic() + timeout
    first_line_bytes = readline_with_timeout(
        stdout,
        timeout=remaining_time(deadline)
    )

    if not first_line_bytes:
        raise EOFError("EOF reached while reading first line from subprocess")

    first_line = first_line_bytes.decode('utf-8').rstrip('\r\n')

    _logger.debug("First Paper Muncher request line: %s", first_line)

    parts = first_line.split(' ')
    if len(parts) != 3:
        raise ValueError(
            f"Invalid HTTP request line from Paper Muncher: {first_line}")

    method, path, _ = parts
    if method == 'PUT':
        path = None
    elif method != 'GET':
        raise ValueError(
            f"Unexpected HTTP method: {method} in line: {first_line}")

    consume_paper_muncher_request(stdout, timeout=remaining_time(deadline))

    return path
