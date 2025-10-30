"""The :mod:`synchronous.io_with_timeout.posix.communications`
module provides POSIX-specific implementations for reading and writing data
with timeouts in a file-like object.
This module uses the `selectors` module to handle I/O operations
in a non-blocking manner, allowing for timeouts on read and write operations.
"""

import logging
import os
import selectors
import time

from typing import BinaryIO

from ..common import remaining_time

_logger = logging.getLogger(__name__)


def readline_with_timeout(
    file_object: BinaryIO,
    timeout: int,
) -> bytes:
    """Read a full line ending with '\\n' from a file-like object within a
    timeout.

    :param BinaryIO file_object: File-like object to read from
        (must be in binary mode).
    :param int timeout: Max seconds to wait for line data.
    :return: A line of bytes ending in '\\n'.
    :rtype: bytes
    :raises TimeoutError: If timeout is reached before a line is read.
    :raises EOFError: If EOF is reached before a line is read.
    """
    fd = file_object.fileno()
    deadline = time.monotonic() + timeout
    line_buffer = bytearray()

    with selectors.DefaultSelector() as selector:
        selector.register(fd, selectors.EVENT_READ)

        while selector.select(timeout=remaining_time(deadline)):
            next_byte = os.read(fd, 1)
            if not next_byte:
                raise EOFError("EOF reached while reading line")

            line_buffer += next_byte
            if next_byte == b'\n':
                break

    _logger.debug(
        "Elapsed time reading line: %.3f seconds",
        time.monotonic() - (deadline - timeout)
    )
    return bytes(line_buffer)


def read_all_with_timeout(
    file_object: BinaryIO,
    timeout: int,
    chunk_size: int,
) -> bytes:
    """Read all data from a file-like object until EOF, with a timeout per
    chunk.

    :param BinaryIO file_object: File-like object to read from.
    :param int timeout: Timeout in seconds for the entire read operation.
    :param int chunk_size: Number of bytes to read per chunk.
    :return: All bytes read until EOF.
    :rtype: bytes
    :raises TimeoutError: If no data is read within the timeout period.
    """
    fd = file_object.fileno()
    data = bytearray()
    deadline = time.monotonic() + timeout

    with selectors.DefaultSelector() as selector:
        selector.register(fd, selectors.EVENT_READ)
        while selector.select(timeout=remaining_time(deadline)):
            chunk = os.read(fd, chunk_size)
            if not chunk:
                break
            data.extend(chunk)

    _logger.debug(
        "Elapsed time reading: %.3f seconds",
        time.monotonic() - (deadline - timeout)
    )
    return bytes(data)


def write_with_timeout(
    file_object: BinaryIO,
    data: bytes,
    timeout: int,
) -> None:
    """Write all data to a file-like object within a timeout, using selectors.

    :param BinaryIO file_object: File-like object to write to.
    :param bytes data: Bytes to write.
    :param int timeout: Max seconds to wait for write readiness.
    :raises TimeoutError: If writing cannot complete within timeout.
    """
    fd = file_object.fileno()
    total_written = 0
    deadline = time.monotonic() + timeout

    with selectors.DefaultSelector() as selector:
        selector.register(fd, selectors.EVENT_WRITE)

        while total_written < len(data):
            events = selector.select(timeout=remaining_time(deadline))
            if not events:
                raise TimeoutError(
                    "Timeout exceeded while writing to subprocess"
                )

            written = os.write(fd, data[total_written:])
            if written == 0:
                raise RuntimeError("Write returned zero bytes")
            total_written += written

    _logger.debug(
        "Elapsed time writing: %.3f seconds",
        time.monotonic() - (deadline - timeout)
    )
