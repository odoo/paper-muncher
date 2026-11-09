export module Vaev.Engine:style.values.inlineLayout;

import Karm.Core;

import :css.values.inlineLayout;
import :style.values.keywords;
import :style.values.numeric;

using namespace Karm;

// https://drafts.csswg.org/css-inline-3/

namespace Vaev::Style {

export using LineHeight = Union<Keywords::Normal, f64, Pixel>;

// https://www.w3.org/TR/css-inline-3/#baseline-source
export using BaselineSource = Css::BaselineSource;

// https://www.w3.org/TR/css-inline-3/#alignment-baseline-property
export using AlignmentBaseline = Css::AlignmentBaseline;

// https://www.w3.org/TR/css-inline-3/#dominant-baseline-property
export using DominantBaseline = Css::DominantBaseline;

export using BaselineShift = Union<
    PixelPercentage,
    Keywords::Sub,
    Keywords::Super,
    Keywords::Top,
    Keywords::Center,
    Keywords::Bottom>;

} // namespace Vaev::Style
