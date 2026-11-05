export module Vaev.Engine:values.lineHeight;

import Karm.Core;

import :values.calc2;
import :values.keywords;
import :values.length;
import :values.primitives;
import :values.unitValue;

using namespace Karm;

namespace Vaev {

// https://www.w3.org/TR/css-inline-3/#line-height-property
export using LineHeight = Union<Keywords::Normal, Number, LengthPercentage>;

// NOTE: The computed value of a <length-percentage> is an absolute length,
//       so percentages are resolved against the element's own font size.
export using ComputedLineHeight = Union<Keywords::Normal, f64, Pixel>;

export ComputedLineHeight resolve(LineHeight const& value, LengthContext auto const& ctx) {
    return value.visit(
        [](Keywords::Normal const& normal) -> ComputedLineHeight {
            return normal;
        },
        [](Number const& number) -> ComputedLineHeight {
            return resolve(number);
        },
        [&](LengthPercentage const& length) -> ComputedLineHeight {
            return resolve(length, ctx, ctx.fontSize);
        }
    );
}

export LineHeight toSpecified(ComputedLineHeight const& value) {
    return value.visit(
        [](Keywords::Normal const& normal) -> LineHeight {
            return normal;
        },
        [](f64 const& number) -> LineHeight {
            return Number::fromCanonical(number);
        },
        [](Pixel const& px) -> LineHeight {
            return toSpecified(PixelPercentage{px});
        }
    );
}

} // namespace Vaev
