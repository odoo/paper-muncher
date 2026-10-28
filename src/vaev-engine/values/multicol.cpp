export module Vaev.Engine:values.multicol;

import Karm.Core;

import :values.calc;
import :values.keywords;

using namespace Karm;

namespace Vaev {

// https://drafts.csswg.org/css-multicol-2/#cw
// FIXME: Enforce the [0,∞] range.
export using ColumnWidth = Union<
    Keywords::Auto,
    Calc<Length>>;

// https://drafts.csswg.org/css-multicol-2/#ch
// FIXME: Enforce the [0,∞] range.
export using ColumnHeight = Union<
    Keywords::Auto,
    Calc<Length>>;

// https://drafts.csswg.org/css-multicol-2/#cc
// FIXME: Enforce the [1,∞] range.
export using ColumnCount = Union<
    Keywords::Auto,
    Integer>;

export struct ColumnProps {
    Union<Keywords::Auto, Au> width = Keywords::AUTO;
    Union<Keywords::Auto, Au> height = Keywords::AUTO;
    ColumnCount count = Keywords::AUTO;

    void repr(Io::Emit& e) const {
        e("(column");
        e(" width={}", width);
        e(" height={}", height);
        e(" count={}", count);
        e(")");
    }
};

} // namespace Vaev
