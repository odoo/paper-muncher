#!/usr/bin/env bash
set -euo pipefail
ck build --release
ck test --release
ck reftests run --release