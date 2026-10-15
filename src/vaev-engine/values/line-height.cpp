export module Vaev.Engine:values.lineHeight;

import Karm.Core;

import :values.calc;
import :values.keywords;
import :values.length;
import :values.percent;
import :values.primitives;
import :values.base;

using namespace Karm;

namespace Vaev {

// https://www.w3.org/TR/css-inline-3/#line-height-property
export using LineHeight = Union<Keywords::Normal, Number, Calc<Length, Percent>>;

export template <>
struct ValueTraits<LineHeight> : UnionValueTraits<LineHeight> {
    // Computed value: the specified keyword, a number, or a computed <length> value
    using Computed = Union<Keywords::Normal, Number, Au>;

    static LineHeight specify(Vaev::Computed<LineHeight> const& value) {
        return value.visit(
            [](Keywords::Normal const& v) -> LineHeight {
                return v;
            },
            [](Number const& v) -> LineHeight {
                return v;
            },
            [](Au const& v) -> LineHeight {
                return Calc<Length, Percent>{Length{v}};
            }
        );
    }

    static Vaev::Computed<LineHeight> compute(LineHeight const& value, LengthContext auto const& ctx) {
        return value.visit(
            [](Keywords::Normal const& v) -> Vaev::Computed<LineHeight> {
                return v;
            },
            [](Number const& v) -> Vaev::Computed<LineHeight> {
                return v;
            },
            [&](Calc<Length, Percent> const& v) -> Vaev::Computed<LineHeight> {
                // Percentage: relative to the font size of the element itself.
                return Vaev::resolve(Vaev::compute(v, ctx), Au::fromFloatNearest(ctx.fontSize));
            }
        );
    }
};

} // namespace Vaev
