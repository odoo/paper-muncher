#!/usr/bin/env bash

LLVM_PREFIX="$(brew --prefix llvm)"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

export PATH="$LLVM_PREFIX/bin:$PATH"
export CC="$LLVM_PREFIX/bin/clang"
export CXX="$LLVM_PREFIX/bin/clang++"
export PKG_CONFIG_PATH="$PROJECT_ROOT/meta/pkgconfig:${PKG_CONFIG_PATH:-}"
