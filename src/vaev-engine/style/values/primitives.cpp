module;

#include <karm/macros>

export module Vaev.Engine:style.values.primitives;

import Karm.Core;

using namespace Karm;

namespace Vaev::Style {

// MARK: Number ----------------------------------------------------------------
// https://drafts.csswg.org/css-values/#numbers

export f64 toComputed(f64 const& value, [[maybe_unused]] auto const& ctx = NONE) {
    return value;
}

} // namespace Vaev::Style
