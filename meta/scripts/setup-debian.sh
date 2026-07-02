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
    liburing-dev \
    libunwind-dev
