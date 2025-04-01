#pragma once

// clang-format off

#include <coroutine>
#include <compare>
#include <initializer_list>
#include <new>
#include <utility>
#include <memory>
#include <compare>
#include <stdint.h>
#include <stddef.h>


#include <math.h>

#ifdef __ck_sys_darwin__
// Darwin is poluting the global namespace with unwanted macros.
#undef UNDERFLOW
#undef DOMAIN
#undef OVERFLOW
#endif

#include <string.h>

#ifndef KARM_NO_TOP_LEVEL_USING

namespace marK {
} // namespace Karm

using namespace marK;

#endif
