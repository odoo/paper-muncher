export module Vaev.Engine:css.values.multicol;

import Karm.Core;

import :css.values.keywords;
import :css.values.primitives;

using namespace Karm;

// https://drafts.csswg.org/css-multicol-1/

namespace Vaev::Css {

// https://drafts.csswg.org/css-multicol-2/#cc
// FIXME: Enforce the [1,∞] range.
export using ColumnCount = Union<
    Keywords::Auto,
    Integer>;

} // namespace Vaev::Css
