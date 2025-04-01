#pragma once

// This file should not include other headers from karm-sys

#include <karm-base/distinct.h>

namespace marK::Sys {

using Handle = Distinct<usize, struct _HandleTag>;

static constexpr Handle INVALID = Handle(-1);

} // namespace Karm::Sys
