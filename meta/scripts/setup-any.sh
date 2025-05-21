#!/usr/bin/env bash

set -e

if [ "$EUID" -eq 0 ]; then
    export CUTEKIT_ELEVATOR=""
elif [ -x "$(command -v sudo)" ]; then
    export CUTEKIT_ELEVATOR="sudo"
elif [ -x "$(command -v doas)" ]; then
    export CUTEKIT_ELEVATOR="doas"
fi


function is_ubuntu() {
    if [ -f /etc/os-release ]; then
        grep -q "ubuntu" /etc/os-release
        return $?
    fi
    return 1
}

function is_debian() {
    if [ -f /etc/os-release ]; then
        grep -q "debian" /etc/os-release
        return $?
    fi
    return 1
}

function is_darwin() {
    if [ "$(uname)" == "Darwin" ]; then
        return 0
    fi
    return 1
}

function is_arch() {
    if [ -f /etc/os-release ]; then
        grep -q "Arch Linux" /etc/os-release
        return $?
    fi
    return 1
}

if [ "$1" == "tools" -a "$2" == "setup" ]; then
    if is_ubuntu; then
        $CUTEKIT_ELEVATOR ./meta/scripts/setup-ubuntu.sh
    elif is_debian; then
        $CUTEKIT_ELEVATOR ./meta/scripts/setup-debian.sh
    elif is_arch; then
        $CUTEKIT_ELEVATOR ./meta/scripts/setup-arch.sh
    elif is_darwin; then
        ./meta/scripts/setup-darwin.sh
    fi
    if [ ! -x "$(command -v uv)" ]; then
        curl -LsSf https://astral.sh/uv/install.sh | sh
    fi
    exit 0
fi

if is_ubuntu; then
    source ./meta/scripts/env-ubuntu.sh
elif is_darwin; then
    source ./meta/scripts/env-darwin.sh
fi

if [ -d "$XDG_BIN_HOME" ]; then
    export PATH="$XDG_BIN_HOME:$PATH"
fi

if [ -d "$XDG_DATA_HOME/../bin" ]; then
    export PATH="$XDG_DATA_HOME/../bin:$PATH"
fi

if [ -d "$HOME/.local/bin" ]; then
    export PATH="$HOME/.local/bin:$PATH"
fi
