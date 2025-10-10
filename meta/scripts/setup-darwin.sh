#!/usr/bin/env bash

if ! command -v brew &> /dev/null
then
    echo "Homebrew is not installed. Please install Homebrew first."
    echo '/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"'
    exit 1
fi

brew update
brew install libmagic lld@20 llvm@20 nasm ninja pkgconf python3 sdl3 ccache jq
