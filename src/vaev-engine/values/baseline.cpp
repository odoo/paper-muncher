export module Vaev.Engine:values.baseline;

import Karm.Core;
import :values.calc2;
import :values.keywords;
import :values.length;
import :values.lineHeight;
import :values.percent;
import :values.unitValue;

using namespace Karm;

namespace Vaev::Css {

// https://www.w3.org/TR/css-inline-3/#baseline-source
export using BaselineSource = Union<Keywords::Auto, Keywords::First, Keywords::Last>;

// https://www.w3.org/TR/css-inline-3/#alignment-baseline-property
export using AlignmentBaseline = Union<
    Keywords::Baseline,
    Keywords::TextBottom,
    Keywords::Alphabetic,
    Keywords::Ideographic,
    Keywords::Middle,
    Keywords::Central,
    Keywords::Mathematical,
    Keywords::TextTop>;

// https://www.w3.org/TR/css-inline-3/#dominant-baseline-property
export using DominantBaseline = Union<
    Keywords::Auto,
    Keywords::Baseline,
    Keywords::TextBottom,
    Keywords::Alphabetic,
    Keywords::Ideographic,
    Keywords::Middle,
    Keywords::Central,
    Keywords::Mathematical,
    Keywords::TextTop>;

// https://drafts.csswg.org/css-inline/#propdef-baseline-shift
export using BaselineShift = Union<
    LengthPercentage,
    Keywords::Sub,
    Keywords::Super,
    Keywords::Top,
    Keywords::Center,
    Keywords::Bottom>;

} // namespace Vaev::Css

namespace Vaev::Style {

export using BaselineShift = Union<
    PixelPercentage,
    Keywords::Sub,
    Keywords::Super,
    Keywords::Top,
    Keywords::Center,
    Keywords::Bottom>;

export struct Inline {
    Css::BaselineSource baselineSource = Keywords::AUTO;
    Css::AlignmentBaseline alignmentBaseline = Keywords::BASELINE;
    Css::DominantBaseline dominantBaseline = Keywords::AUTO;
    BaselineShift baselineShift = PixelPercentage{Pixel{}};
    Style::LineHeight lineHeight = Keywords::NORMAL;
};

export Style::BaselineShift toComputed(Css::BaselineShift const& value, Css::LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> Style::BaselineShift { return toComputed(v, ctx); });
}

export Css::BaselineShift toSpecified(Style::BaselineShift const& value) {
    return value.visit([](auto const& v) -> Css::BaselineShift { return toSpecified(v); });
}

} // namespace Vaev::Style

