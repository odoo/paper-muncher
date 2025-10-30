from cutekit import ensure

ensure((0, 10, 0))

from . import reftest, tools, wpt  # noqa E402, F401: Needed for side effect
