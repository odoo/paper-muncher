"""The :mod:`synchronous.io_with_timeout.common`
module provides utilities for managing timeouts in I/O operations.
It includes a function to calculate the remaining time until a deadline,
and raises a `TimeoutError` if the deadline has already passed.
"""

import inspect
import time


def remaining_time(deadline: float) -> float:
    """Calculate the remaining time until a deadline.

    :param float deadline: The deadline timestamp.
    :return: Remaining time in seconds.
    :rtype: float
    :raises TimeoutError: If the deadline has already passed.
    """
    remaining = deadline - time.monotonic()
    if remaining <= 0:
        caller_frame = inspect.currentframe().f_back
        raise TimeoutError(
            "Timeout exceeded in function %(function)s at line %(line)d"
            " in file %(file)s" % {
            'function': caller_frame.f_code.co_name,
            'line': caller_frame.f_lineno,
            'file': caller_frame.f_code.co_filename,
        })
    return remaining
