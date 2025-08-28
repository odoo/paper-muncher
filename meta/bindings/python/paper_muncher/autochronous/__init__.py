"""The :mod:`paper_muncher.autochronous` module
provides rendering capabilities that automatically
choose between synchronous and asynchronous execution
based on the context.
"""

from ..binary import can_use_paper_muncher
from .interface import render, rendered
