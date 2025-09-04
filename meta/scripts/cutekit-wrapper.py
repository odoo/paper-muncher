#!/usr/bin/env -S uv run --script

# /// script
# requires-python = ">=3.12"
# dependencies = [
#   "cutekit @ git+https://github.com/cute-engineering/cutekit.git@0.9.2",
#   "Markdown~=3.7",
# ]
# ///

import cutekit
import sys

sys.exit(cutekit.main())
