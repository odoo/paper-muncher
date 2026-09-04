#!/usr/bin/env bash
set -euo pipefail
ck build --release
ck test --release
ck reftests run --release
ck html5lib-tests run -s tokenizer --release
ck html5lib-tests run -s tree_construction --release
