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

# Setup python3.11
apt-get install -y --no-install-recommends python3.11 python3.11-distutils python3.11-venv

# Build dependencies
apt-get install -y --no-install-recommends libseccomp-dev liburing-dev libsdl2-dev

# Install llvm
apt-get install -y --no-install-recommends wget lsb-release software-properties-common gnupg
bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" llvm 20 all
