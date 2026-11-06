export module Vaev.Engine:css.values.multicol;

import Karm.Core;

import :css.values.calc2;
import :css.values.keywords;
import :values.keywords;

using namespace Karm;

namespace Vaev::Css {

// https://drafts.csswg.org/css-multicol-2/#cw
// FIXME: Enforce the [0,∞] range.
export using ColumnWidth = Union<
    Keywords::Auto,
    Length>;

// https://drafts.csswg.org/css-multicol-2/#ch
// FIXME: Enforce the [0,∞] range.
export using ColumnHeight = Union<
    Keywords::Auto,
    Length>;

} // namespace Vaev::Css
