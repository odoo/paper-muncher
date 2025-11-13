#!/usr/bin/env -S uv run --script

# /// script
# requires-python = ">=3.12"
# dependencies = [
#   "cutekit~=0.10.0",
#   "Markdown~=3.7",
# ]
# ///

import cutekit
import sys

sys.exit(cutekit.main())
