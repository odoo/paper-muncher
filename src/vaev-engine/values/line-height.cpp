export module Vaev.Engine:values.lineHeight;

import Karm.Core;

import :values.calc;
import :values.keywords;
import :values.length;
import :values.percent;
import :values.primitives;

using namespace Karm;

namespace Vaev {

// https://www.w3.org/TR/css-inline-3/#line-height-property
export using LineHeight = Union<Keywords::Normal, Number, Calc<PercentOr<Length>>>;
export using ComputedLineHeight = Union<Keywords::Normal, Number, Au>;

export ComputedLineHeight resolve(LineHeight const& value, RelativeLengthContext auto const& ctx) {
    return value.visit(
        [&](Calc<PercentOr<Length>> const& calc) -> ComputedLineHeight {
            // Percentages computed relative to '1em'
            return resolve(calc, ctx, Au{ctx.fontSize});
        },
        [](auto const& other) -> ComputedLineHeight {
            return other;
        }
    );
}

} // namespace Vaev
