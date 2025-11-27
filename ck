#!/usr/bin/env bash

set -e

if [ "$EUID" -eq 0 ]; then
    echo "Please do not run this script as root."

    if [ "$CUTEKIT_ALLOW_ROOT" == "1" ]; then
        echo "CUTEKIT_ALLOW_ROOT is set, continuing..."
    else
        echo "If you know what you are doing, set CUTEKIT_ALLOW_ROOT=1 and try again."
        echo "Aborting."
        exit 1
    fi
fi

source ./meta/scripts/setup-any.sh
uv tool run --from 'cutekit~=0.10.3' --with markdown cutekit $@
