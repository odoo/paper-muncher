#!/bin/bash

set -e

apt-get update -y --no-install-recommends
apt-get install -y --no-install-recommends build-essential git ninja-build libsdl2-dev nasm liburing-dev ccache jq
bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" llvm 19 all
