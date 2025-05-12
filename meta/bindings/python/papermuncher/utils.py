import logging
import os
import shutil
import subprocess
import re


_logger = logging.getLogger(__name__)

MIN_PM_VERSION: tuple = (0, 1, 2)
EXECUTABLE_NAME: str = 'paper-muncher'


def locate_executable():
    executable_name = EXECUTABLE_NAME
    if os.name == 'nt':  # Windows
        executable_name += ".exe"
    path = shutil.which(executable_name)
    if path:
        return path
    else:
        raise FileNotFoundError(f"Executable '{executable_name}' not found in PATH.")

def get_executable_status(self):
    try:
        path = locate_executable()
    except FileNotFoundError:
        return 'Not Installed'

    try:
        process = subprocess.Popen(
            [path, '--version'], stdout=subprocess.PIPE, stderr=subprocess.PIPE
        )
    except OSError:
        return 'Dead'

    out, _ = process.communicate()
    version_bare = out.decode('utf-8').strip()
    version_match = re.search(r'(\d+\.\d+\.\d+)', version_bare)

    if not version_match:
        return 'Unknown Version'

    version = tuple(map(int, version_match.group(1).split('.')))

    if version < MIN_PM_VERSION:
        return 'Depreciated'

    return 'Ready'
