#!/usr/bin/env bash

if ! command -v brew &> /dev/null
then
    echo "Homebrew is not installed. Please install Homebrew first."
    echo '/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"'
    exit 1
fi

brew install libmagic lld llvm nasm ninja pkgconf python3 sdl2
