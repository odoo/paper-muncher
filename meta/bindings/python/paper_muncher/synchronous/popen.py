"""The :mod:`paper_muncher.synchronous.popen` module
provides a cross-platform context manager for
subprocess.Popen that ensures non-blocking I/O for Windows.
This is necessary to avoid deadlocks when reading from subprocess streams.
"""


import os
import subprocess
import sys


if os.name == 'nt' and sys.version_info >= (3, 12):
    from contextlib import contextmanager

    @contextmanager
    def Popen(*args, **kwargs):
        """Context manager for subprocess.Popen that sets non-blocking I/O
        for stdin, stdout, and stderr.
        This is necessary for Windows to avoid deadlocks when reading
        from subprocess streams.

        :param args: Positional arguments for subprocess.Popen.
        :param kwargs: Keyword arguments for subprocess.Popen.
        :return: A context manager that yields the subprocess.Popen object.
        """
        with subprocess.Popen(*args, **kwargs, bufsize=0) as proc:
            os.set_blocking(proc.stdout, False)
            os.set_blocking(proc.stderr, False)
            os.set_blocking(proc.stdin, False)
            yield proc
else:
    Popen = subprocess.Popen
