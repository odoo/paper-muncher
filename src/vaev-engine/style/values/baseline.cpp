export module Vaev.Engine:style.values.baseline;

import Karm.Core;

import :css.values.baseline;
import :style.values.numeric;
import :values.unitValue;

using namespace Karm;

namespace Vaev::Style {

export using BaselineShift = Union<
    PixelPercentage,
    Keywords::Sub,
    Keywords::Super,
    Keywords::Top,
    Keywords::Center,
    Keywords::Bottom>;

export BaselineShift toComputed(Css::BaselineShift const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> BaselineShift { return toComputed(v, ctx); });
}

export Css::BaselineShift toSpecified(BaselineShift const& value) {
    return value.visit([](auto const& v) -> Css::BaselineShift { return toSpecified(v); });
}

} // namespace Vaev::Style
