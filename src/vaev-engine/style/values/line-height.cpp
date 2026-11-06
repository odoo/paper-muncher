export module Vaev.Engine:style.values.lineHeight;

import Karm.Core;

import :css.values.calc;
import :css.values.lineHeight;
import :style.values.numeric;
import :values.unitValue;

using namespace Karm;

namespace Vaev::Style {

// NOTE: The computed value of a <length-percentage> is an absolute length,
//       so percentages are resolved against the element's own font size.

export using LineHeight = Union<Keywords::Normal, f64, Pixel>;

export LineHeight toComputed(Css::LineHeight const& value, LengthContext auto const& ctx) {
    return value.visit(
        [](Keywords::Normal const& normal) -> LineHeight {
            return normal;
        },
        [](Css::Number const& number) -> LineHeight {
            return toComputed(number);
        },
        [&](Css::LengthPercentage const& length) -> LineHeight {
            return toComputed(length, ctx, ctx.fontSize);
        }
    );
}

export Css::LineHeight toSpecified(LineHeight const& value) {
    return value.visit(
        [](Keywords::Normal const& normal) -> Css::LineHeight {
            return normal;
        },
        [](f64 const& number) -> Css::LineHeight {
            return Css::Number::fromCanonical(number);
        },
        [](Pixel const& px) -> Css::LineHeight {
            return toSpecified(PixelPercentage{px});
        }
    );
}

} // namespace Vaev::Style
