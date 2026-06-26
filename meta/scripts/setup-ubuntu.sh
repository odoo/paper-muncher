#!/bin/bash

set -xe

apt-get update -y --no-install-recommends
apt-get install -y --no-install-recommends \
    pkg-config \
    build-essential \
    git \
    ninja-build \
    jq \
    ca-certificates \
    curl \
    libseccomp-dev \
    liburing-dev \
    libunwind-dev

