export module Vaev.Engine:style.values.insets;

import Karm.Core;
import Karm.Math;

import :css.values.calc;
import :css.values.insets;
import :style.values.generic;
import :style.values.numeric;
import :values.unitValue;

using namespace Karm;

namespace Vaev::Style {

export template <typename T>
auto toComputed(Math::Insets<T> const& value, LengthContext auto const& ctx) -> Math::Insets<decltype(toComputed(value.top, ctx))> {
    return {toComputed(value.top, ctx), toComputed(value.end, ctx), toComputed(value.bottom, ctx), toComputed(value.start, ctx)};
}

export template <typename T>
auto toSpecified(Math::Insets<T> const& value) -> Math::Insets<decltype(toSpecified(value.top))> {
    return {toSpecified(value.top), toSpecified(value.end), toSpecified(value.bottom), toSpecified(value.start)};
}

export using Gap = Union<
    Keywords::Normal,
    PixelPercentage>;

} // namespace Vaev::Style

