export module Vaev.Engine:values.lineHeight;

import Karm.Core;

import :values.calc;
import :values.keywords;
import :values.length;
import :values.percent;
import :values.primitives;
import :values.resolved;

using namespace Karm;

namespace Vaev {

// https://www.w3.org/TR/css-inline-3/#line-height-property
export using LineHeight = Union<Keywords::Normal, Number, Calc<Length, Percent>>;

export template <>
struct _Computed<LineHeight> {
    using Type = Union<Keywords::Normal, Number, Au>;
};

// Computed value: the specified keyword, a number, or a computed <length> value
export Computed<LineHeight> compute(LineHeight const& value, LengthContext auto const& ctx) {
    return value.visit(
        [](Keywords::Normal const& v) -> Computed<LineHeight> {
            return v;
        },
        [](Number const& v) -> Computed<LineHeight> {
            return v;
        },
        [&](Calc<Length, Percent> const& v) -> Computed<LineHeight> {
            // Percentage: relative to the font size of the element itself.
            return resolve(compute(v, ctx), Au::fromFloatNearest(ctx.fontSize));
        }
    );
}

export template <>
struct _Specify<LineHeight> {
    static LineHeight from(Computed<LineHeight> const& value) {
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
};

} // namespace Vaev
