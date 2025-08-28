"""The :mod:`synchronous.io_with_timeout`
module provides cross-platform utilities for I/O operations with timeouts.
It includes functions for reading and writing data with a specified timeout,
and handles platform-specific differences in I/O behavior when possible.
"""

import os
import logging
import sys

_logger = logging.getLogger(__name__)


if os.name == 'posix':
    _logger.info("Using POSIX communications module")
    from .posix.communications import (
        read_all_with_timeout,
        readline_with_timeout,
        write_with_timeout,
    )
elif os.name == 'nt' and sys.version_info >= (3, 12):
    _logger.info("Using NT communications module")
    from .nt.communications import (
        read_all_with_timeout,
        readline_with_timeout,
        write_with_timeout,
    )
else:
    _logger.warning(
        "Using basic communications module without proper"
        " anti stalled process handling"
    )
    from .fallback.communications import (
        read_all_with_timeout,
        readline_with_timeout,
        write_with_timeout,
    )
