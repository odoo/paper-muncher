export module Vaev.Engine:values.overflow;

import Karm.Core;

import :values.keywords;

using namespace Karm;

namespace Vaev {

// MARK: Overflow
// https://www.w3.org/TR/css-overflow/#overflow-control
export using Overflow = Union<
    Keywords::Visible,
    Keywords::Hidden,
    Keywords::Scroll,
    Keywords::Auto>;

} // namespace Vaev
