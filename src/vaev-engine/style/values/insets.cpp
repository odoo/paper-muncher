export module Vaev.Engine:style.values.insets;

import Karm.Core;
import Karm.Math;

import :css.values.calc;
import :css.values.insets;
import :style.values.numeric;
import :values.unitValue;

using namespace Karm;

namespace Vaev::Style {

export Union<Keywords::Auto, PixelPercentage> toComputed(Union<Keywords::Auto, Css::LengthPercentage> const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> Union<Keywords::Auto, PixelPercentage> { return toComputed(v, ctx); });
}

export Union<Keywords::Auto, Css::LengthPercentage> toSpecified(Union<Keywords::Auto, PixelPercentage> const& value) {
    return value.visit([](auto const& v) -> Union<Keywords::Auto, Css::LengthPercentage> { return toSpecified(v); });
}

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

export Gap toComputed(Css::Gap const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> Gap { return toComputed(v, ctx); });
}

export Css::Gap toSpecified(Gap const& value) {
    return value.visit([](auto const& v) -> Css::Gap { return toSpecified(v); });
}

} // namespace Vaev::Style

