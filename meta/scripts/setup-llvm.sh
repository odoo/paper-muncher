#!/bin/bash
set -exo pipefail

TARGET_DIR="$HOME/.cutekit/toolchain"

# Check if LLVM is already installed
if [ -x "$TARGET_DIR/bin/clang" ]; then
    echo "LLVM is already installed in $TARGET_DIR. Skipping installation."
    "$TARGET_DIR/bin/clang" --version
    exit 0
fi

ARCH=$(uname -m)
case "$ARCH" in
    x86_64) LLVM_ARCH="Linux-X64" ;;
    aarch64|arm64) LLVM_ARCH="Linux-ARM64" ;;
    *) echo "Error: Unsupported architecture $ARCH" >&2; exit 1 ;;
esac

DOWNLOAD_URL=$(curl -s https://api.github.com/repos/llvm/llvm-project/releases/latest | \
    grep -oE '"browser_download_url": "[^"]+"' | \
    cut -d'"' -f4 | \
    grep "LLVM-" | \
    grep "$LLVM_ARCH" | \
    grep -vE '(\.sig|\.jsonl)' | \
    head -n 1)

if [ -z "$DOWNLOAD_URL" ]; then
    echo "Error: Toolchain not found for $LLVM_ARCH" >&2
    exit 1
fi

echo "Installing LLVM into $TARGET_DIR..."
mkdir -p "$TARGET_DIR"
curl -L "$DOWNLOAD_URL" | tar -xJf - -C "$TARGET_DIR" --strip-components=1

echo "Verification:"
"$TARGET_DIR/bin/clang" --version

echo "Creating symlink"
MAJOR_VERSION=$("$TARGET_DIR/bin/clang" --version | head -n 1 | awk '{print $3}' | cut -d. -f1)
ln -s $TARGET_DIR/bin/clang-scan-deps $TARGET_DIR/bin/clang-scan-deps-$MAJOR_VERSION
ln -s $TARGET_DIR/bin/clang++ $TARGET_DIR/bin/clang++-$MAJOR_VERSION
