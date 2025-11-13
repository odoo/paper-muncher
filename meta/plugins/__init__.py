from cutekit import ensure

ensure((0, 10, 0))

from . import tools, wpt  # noqa E402, F401: Needed for side effect
from . import reftests  # noqa E402, F401: Needed for side effect
