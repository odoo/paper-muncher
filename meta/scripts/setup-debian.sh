#!/bin/bash
set -xe
apt-get update -y --no-install-recommends
apt-get install -y --no-install-recommends build-essential git ninja-build liburing-dev jq libseccomp-dev pkg-config libseccomp-dev ca-certificates curl
