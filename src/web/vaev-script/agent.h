#pragma once

#include <karm-base/res.h>
#include <karm-gc/heap.h>

namespace Vive::Script {

// https://tc39.es/ecma262/#agent
struct Agent {
    Gc::Heap& heap;
};

} // namespace Vaev::Script
