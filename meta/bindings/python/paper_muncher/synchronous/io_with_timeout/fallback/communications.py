"""The :mod:`synchronous.io_with_timeout.fallback.communications`
module provides fallback implementations for reading and writing data
with timeouts in a file-like object.
This means it does not support timeouts and may lead to stalled processes.
"""

import logging

from typing import BinaryIO

_logger = logging.getLogger(__name__)


def readline_with_timeout(
    file_object: BinaryIO,
    timeout: int,
) -> bytes:
    """Read a full line ending with '\\n' from a file-like object.

    :param BinaryIO file_object: File-like object to read from
        (must be in binary mode).
    :param int timeout: UNUSED timeout parameter.
        (Fallback implementation does not support timeouts)
    :return: A line of bytes ending in '\\n'.
    :rtype: bytes
    """
    _logger.warning(
        "Using fallback readline_with_timeout. "
        "This may lead to stalled processes."
    )
    return file_object.readline()


def read_all_with_timeout(
    file_object: BinaryIO,
    timeout: int,
) -> bytes:
    """Read all data from a file-like object until EOF.

    :param BinaryIO file_object: File-like object to read from.
    :param int timeout: UNUSED timeout parameter.
        (Fallback implementation does not support timeouts)
    :return: All bytes read from the file-like object.
    :rtype: bytes
    """
    _logger.warning(
        "Using fallback readlines_with_timeout. "
        "This may lead to stalled processes."
    )
    return file_object.read()


def write_with_timeout(
    file_object: BinaryIO,
    data: bytes,
    timeout: int,
) -> None:
    """Write data to a file-like object.

    :param BinaryIO file_object: File-like object to write to.
    :param bytes data: Data to write.
    :param int timeout: UNUSED timeout parameter.
        (Fallback implementation does not support timeouts)
    """
    _logger.warning(
        "Using fallback write_with_timeout. "
        "This may lead to stalled processes."
    )
    file_object.write(data)
    file_object.flush()
