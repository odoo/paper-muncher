#pragma once

#include <karm-base/res.h>

import Karm.Gc;

namespace Vaev::Script {

// https://tc39.es/ecma262/#agent
struct Agent {
    Gc::Heap& heap;
};

} // namespace Vaev::Script
