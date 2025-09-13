"""The :mod:`paper_muncher.utils.binary` module
provides utilities to locate and validate the Paper Muncher binary.
"""


import logging
import os
import subprocess
from shutil import which

from typing import Optional


_logger = logging.getLogger(__name__)

FALLBACK_BINARY = '/opt/paper-muncher/bin/paper-muncher'


def find_in_path(name):
    path = os.environ.get('PATH', os.defpath).split(os.pathsep)
    return which(name, path=os.pathsep.join(path))


def get_paper_muncher_binary() -> Optional[str]:
    """Find and validate the Paper Muncher binary

    :return: Path to the Paper Muncher binary if found and usable,
        None otherwise.
    :rtype: str or None
    """
    try:
        binary = find_in_path('paper-muncher')
    except OSError:
        _logger.debug("Cannot locate in path paper-muncher", exc_info=True)
        binary = FALLBACK_BINARY

    try:
        subprocess.run(
            [binary, '--version'],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=True,
        )
    except subprocess.CalledProcessError:
        _logger.debug("Cannot use paper-muncher", exc_info=True)
        return None

    return binary


def can_use_paper_muncher() -> bool:
    """Check if Paper Muncher binary is available and usable.

    :return: True if Paper Muncher is in debug session and available,
        False otherwise.
    :rtype: bool
    """
    return bool(get_paper_muncher_binary())
