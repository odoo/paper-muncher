#!/usr/bin/env -S uv run --script

# /// script
# dependencies = [
#   "cutekit @ git+https://github.com/cute-engineering/cutekit.git@0.8.8",
#   "Markdown~=3.7",
# ]
# ///

import cutekit
import sys

sys.exit(cutekit.main())
