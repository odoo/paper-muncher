#!/usr/bin/env bash

# Apply patches to externs after they are fetched by `ck install`
# This script should be run after `./ck install`

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
PATCH_DIR="$PROJECT_ROOT/meta/patches"
EXTERNS_DIR="$PROJECT_ROOT/.cutekit/externs"

if [ ! -d "$PATCH_DIR" ]; then
    echo "Patch directory not found: $PATCH_DIR"
    exit 1
fi

if [ ! -d "$EXTERNS_DIR" ]; then
    echo "Externs directory not found: $EXTERNS_DIR"
    echo "Run './ck install' first"
    exit 1
fi

echo "Applying patches from $PATCH_DIR to externs in $EXTERNS_DIR..."

for patch_file in "$PATCH_DIR"/*.patch; do
    if [ ! -f "$patch_file" ]; then
        continue
    fi

    patch_name=$(basename "$patch_file")
    echo "Applying $patch_name..."

    # Apply patch from the project root using -p0 (don't strip path components)
    if git -C "$EXTERNS_DIR" apply -p0 --check "$patch_file" 2>/dev/null; then
        git -C "$EXTERNS_DIR" apply -p0 "$patch_file"
        echo "  ✓ Applied successfully"
    elif git -C "$EXTERNS_DIR" apply -p0 --check --reverse "$patch_file" 2>/dev/null; then
        echo "  ✓ Already applied (changes present)"
    else
        # Check if the changes are already present by looking at the diff
        echo "  ⚠ Patch doesn't apply cleanly - checking if changes are already present..."
        # Try to apply with --reject to see what conflicts
        if git -C "$EXTERNS_DIR" apply -p0 --reject "$patch_file" 2>&1 | grep -q "already applied"; then
            echo "  ✓ Changes already present"
        else
            echo "  ✗ Failed to apply - manual intervention may be needed"
        fi
    fi
done

echo "Done applying patches."
