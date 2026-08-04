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
