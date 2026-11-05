export module Vaev.Engine:values.multicol;

import Karm.Core;

import :values.calc2;
import :values.keywords;

using namespace Karm;

namespace Vaev {

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

// https://drafts.csswg.org/css-multicol-2/#cc
// FIXME: Enforce the [1,∞] range.
export using ColumnCount = Union<
    Keywords::Auto,
    Integer>;

namespace Style {

export struct Columns {
    Union<Keywords::Auto, Au> width = Keywords::AUTO;
    Union<Keywords::Auto, Au> height = Keywords::AUTO;
    ColumnCount count = Keywords::AUTO;
};

} // namespace Style
} // namespace Vaev
