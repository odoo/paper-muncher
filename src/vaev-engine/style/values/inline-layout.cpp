export module Vaev.Engine:style.values.inlineLayout;

import Karm.Core;

import :css.values.inlineLayout;
import :css.values.numeric;
import :style.values.keywords;
import :style.values.numeric;
import :style.values.base;

using namespace Karm;

// https://drafts.csswg.org/css-inline-3/

namespace Vaev::Style {

// https://drafts.csswg.org/css-inline-3/#line-height-property
export using LineHeight = Union<Keywords::Normal, f64, Px>;

// https://www.w3.org/TR/css-inline-3/#baseline-source
export using BaselineSource = Css::BaselineSource;

// https://www.w3.org/TR/css-inline-3/#alignment-baseline-property
export using AlignmentBaseline = Css::AlignmentBaseline;

// https://www.w3.org/TR/css-inline-3/#dominant-baseline-property
export using DominantBaseline = Css::DominantBaseline;

// https://www.w3.org/TR/css-inline-3/#baseline-shift-property
// Computed Value: the specified keyword or a computed <length-percentage> value
export using BaselineShift = Union<
    PxPercent,
    Keywords::Sub,
    Keywords::Super,
    Keywords::Top,
    Keywords::Center,
    Keywords::Bottom>;

} // namespace Vaev::Style
