export module Vaev.Engine:values.baseline;

import Karm.Core;
import :values.calc2;
import :values.keywords;
import :values.length;
import :values.lineHeight;
import :values.percent;
import :values.unitValue;

using namespace Karm;

namespace Vaev::Style {

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

export using ComputedBaselineShift = Union<
    PixelPercentage,
    Keywords::Sub,
    Keywords::Super,
    Keywords::Top,
    Keywords::Center,
    Keywords::Bottom>;

export struct InlineProps {
    BaselineSource baselineSource = Keywords::AUTO;
    AlignmentBaseline alignmentBaseline = Keywords::BASELINE;
    DominantBaseline dominantBaseline = Keywords::AUTO;
    ComputedBaselineShift baselineShift = PixelPercentage{Pixel{}};
    ComputedLineHeight lineHeight = Keywords::NORMAL;
};

} // namespace Vaev::Style

namespace Vaev {

export Style::ComputedBaselineShift resolve(Style::BaselineShift const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> Style::ComputedBaselineShift { return resolve(v, ctx); });
}

export Style::BaselineShift toSpecified(Style::ComputedBaselineShift const& value) {
    return value.visit([](auto const& v) -> Style::BaselineShift { return toSpecified(v); });
}

} // namespace Vaev
