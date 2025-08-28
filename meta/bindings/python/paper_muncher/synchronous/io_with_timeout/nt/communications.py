"""The :mod:`synchronous.io_with_timeout.nt.communications`
module provides cross-platform utilities for reading and writing data
with timeouts on Windows systems. It includes functions for reading lines
and chunks of data, as well as writing data to file-like objects
with a specified timeout.
:note: This module is specifically designed for Windows and requires
Python 3.12 or later.
"""

import sys
if sys.version_info < (3, 12):
    raise ImportError(
        "This module requires Python 3.12 or later"
    )

import logging
import os
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

    if os.get_blocking(fd):
        os.set_blocking(fd, False)

    while remaining_time(deadline):
        next_byte = os.read(fd, 1)
        if next_byte is None:
            time.sleep(0.01)
        elif not next_byte:
            raise EOFError("EOF reached while reading line")
        else:
            line_buffer += next_byte
            if next_byte == b'\n':
                break

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

    if os.get_blocking(fd):
        os.set_blocking(fd, False)

    while remaining_time(deadline):
        chunk = os.read(fd, chunk_size)
        if chunk is None:
            time.sleep(0.01)
        elif not chunk:
            break
        else:
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

    if os.get_blocking(fd):
        os.set_blocking(fd, False)

    while remaining_time(deadline):
        written = os.write(fd, data[total_written:])
        if written is None:
            time.sleep(0.01)
        elif written == 0:
            raise RuntimeError("Write operation returned zero bytes")
        else:
            total_written += written
            if total_written >= len(data):
                break
