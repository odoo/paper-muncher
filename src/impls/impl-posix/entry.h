#pragma once

#include <karm-sys/context.h>

int __entryPointWraper(int argc, char const** argv, Sys::EntryPointAsync* entry);

int main(int argc, char const** argv) {
    return __entryPointWraper(argc, argv, entryPointAsync);
}
