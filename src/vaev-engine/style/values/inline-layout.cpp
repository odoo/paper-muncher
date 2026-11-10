export module Vaev.Engine:style.values.inlineLayout;

import Karm.Core;

import :css.values.inlineLayout;
import :css.values.numeric;
import :style.values.keywords;
import :style.values.numeric;

using namespace Karm;

// https://drafts.csswg.org/css-inline-3/

namespace Vaev::Style {

// https://drafts.csswg.org/css-inline-3/#line-height-property
// Computed value: the specified keyword, a number, or a computed <length> value
export using LineHeight = Union<Keywords::Normal, f64, Px>;

// NOTE: Percentages compute to lengths, so a length comes back as a <length-percentage>.
export Union<Keywords::Normal, Css::Number, Css::LengthPercentage> toSpecified(LineHeight const& value) {
    using Specified = Union<Keywords::Normal, Css::Number, Css::LengthPercentage>;
    return value.visit(
        [](Keywords::Normal) -> Specified {
            return Keywords::NORMAL;
        },
        [](f64 number) -> Specified {
            return toSpecified(number);
        },
        [](Px px) -> Specified {
            return toSpecified(PxPercent{px});
        }
    );
}

// https://www.w3.org/TR/css-inline-3/#baseline-source
export using BaselineSource = Css::BaselineSource;

// https://www.w3.org/TR/css-inline-3/#alignment-baseline-property
export using AlignmentBaseline = Css::AlignmentBaseline;

// https://www.w3.org/TR/css-inline-3/#dominant-baseline-property
export using DominantBaseline = Css::DominantBaseline;

export using BaselineShift = Union<
    PxPercent,
    Keywords::Sub,
    Keywords::Super,
    Keywords::Top,
    Keywords::Center,
    Keywords::Bottom>;

} // namespace Vaev::Style
