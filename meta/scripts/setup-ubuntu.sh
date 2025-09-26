#!/bin/bash

set -e

apt-get update -y --no-install-recommends
apt-get install -y --no-install-recommends \
    build-essential \
    git \
    ninja-build \
    nasm \
    ccache \
    jq

# Build dependencies
apt-get install -y --no-install-recommends libseccomp-dev liburing-dev

# Install llvm
apt-get install -y --no-install-recommends wget lsb-release software-properties-common gnupg
bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" llvm 20 all

# Install UV
apt-get install -y --no-install-recommends curl

