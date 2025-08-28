"""The :mod:`paper_muncher.asynchronous` module
provides the core functionality for rendering documents
using the Paper Muncher engine.
It includes the main rendering functions and utilities
for managing the rendering process.
"""

from .interface import rendered, render
from ..binary import can_use_paper_muncher
