export module Vaev.Engine:values.multicol;

import Karm.Core;

import :values.keywords;
import :values.primitives;

using namespace Karm;

namespace Vaev {

// https://drafts.csswg.org/css-multicol-2/#cc
// FIXME: Enforce the [1,∞] range.
export using ColumnCount = Union<
    Keywords::Auto,
    Integer>;

} // namespace Vaev
