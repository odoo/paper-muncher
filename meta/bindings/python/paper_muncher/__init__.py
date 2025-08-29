

import logging
import os

IS_FULLY_SYNC = os.getenv("USE_SYNC", "0") == "1"

if IS_FULLY_SYNC:
    ...
