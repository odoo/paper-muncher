export module Vaev.Engine:values.breaks;

import Karm.Core;

import :css;
import :values.base;
import :values.primitives;
import :values.keywords;

using namespace Karm;

namespace Vaev::Css {

// 3.1. Breaks Between Boxes: the break-before and break-after properties
// https://www.w3.org/TR/css-break-3/#break-between
// https://www.w3.org/TR/css-break-3/#propdef-break-after
// https://www.w3.org/TR/css-break-3/#propdef-break-before

export using BreakBetween = Union<
    Keywords::Auto,
    Keywords::Avoid,
    Keywords::AvoidPage,
    Keywords::Page,
    Keywords::Left,
    Keywords::Right,
    Keywords::Recto,
    Keywords::Verso,
    Keywords::AvoidColumn,
    Keywords::Column,
    Keywords::AvoidRegion,
    Keywords::Region>;

// 3.2. Breaks Within Boxes: the break-inside property
// https://www.w3.org/TR/css-break-3/#break-within

export using BreakInside = Union<
    Keywords::Auto,
    Keywords::Avoid,
    Keywords::AvoidPage,
    Keywords::AvoidColumn,
    Keywords::AvoidRegion>;

} // namespace Vaev::Css

namespace Vaev::Style {

// https://www.w3.org/TR/css-break-3

export struct Breaks {
    Css::BreakBetween before = Keywords::AUTO;
    Css::BreakBetween after = Keywords::AUTO;
    Css::BreakInside inside = Keywords::AUTO;

    // Breaks Between Lines: orphans, widows
    // https://www.w3.org/TR/css-break-3/#widows-orphans
    Css::Integer orphans = 2;
    Css::Integer widows = 2;
};

} // namespace Vaev::Style

