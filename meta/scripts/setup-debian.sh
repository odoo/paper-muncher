#!/bin/bash

set -xe

apt-get update -y --no-install-recommends
apt-get install -y --no-install-recommends lsb-release curl gnupg ca-certificates
apt-get install -y --no-install-recommends software-properties-common || true 
curl -LsSf https://apt.llvm.org/llvm.sh | bash -s -- 22 all

apt-get install -y --no-install-recommends \
    pkg-config \
    build-essential \
    git \
    ninja-build \
    jq \
    libseccomp-dev \
    libunwind-dev

if [ -z "$NO_URING" ]; then
    apt-get install -y --no-install-recommends liburing-dev
fi

# NOTE: Not every distro ships all of these, karm falls back to its builtin
#       codecs for the ones that are missing.
if [ -z "$NO_SYSTEM_CODECS" ]; then
    for pkg in libpng-dev libturbojpeg0-dev libwebp-dev libavif-dev libjxl-dev libtiff-dev; do
        apt-get install -y --no-install-recommends "$pkg" || echo "Skipping $pkg, not available"
    done
fi
