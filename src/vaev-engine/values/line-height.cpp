export module Vaev.Engine:values.lineHeight;

import Karm.Core;

import :values.calc2;
import :values.keywords;
import :values.length;
import :values.primitives;
import :values.unitValue;

using namespace Karm;

namespace Vaev::Css {

// https://www.w3.org/TR/css-inline-3/#line-height-property
export using LineHeight = Union<Keywords::Normal, Number, LengthPercentage>;

} // namespace Vaev::Css

namespace Vaev::Style {

// NOTE: The computed value of a <length-percentage> is an absolute length,
//       so percentages are resolved against the element's own font size.

export using LineHeight = Union<Keywords::Normal, f64, Pixel>;

export Style::LineHeight resolve(Css::LineHeight const& value, Css::LengthContext auto const& ctx) {
    return value.visit(
        [](Keywords::Normal const& normal) -> Style::LineHeight {
            return normal;
        },
        [](Css::Number const& number) -> Style::LineHeight {
            return resolve(number);
        },
        [&](Css::LengthPercentage const& length) -> Style::LineHeight {
            return resolve(length, ctx, ctx.fontSize);
        }
    );
}

export Css::LineHeight toSpecified(Style::LineHeight const& value) {
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

