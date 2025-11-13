#!/bin/bash

set -e

apt-get update -y --no-install-recommends
apt-get install -y --no-install-recommends build-essential git ninja-build nasm liburing-dev ccache jq libseccomp-dev

# Install llvm
apt-get install -y --no-install-recommends wget lsb-release software-properties-common gnupg
bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" llvm 21 all

# Install UV
apt-get install -y --no-install-recommends curl
