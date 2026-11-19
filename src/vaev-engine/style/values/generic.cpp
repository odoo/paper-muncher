export module Vaev.Engine:style.values.generic;

import Karm.Core;
import Karm.Math;

import :style.values.numeric;
import :style.values.primitives;
import :style.values.unitValue;
import :style.values.keywords;

using namespace Karm;

namespace Vaev::Style {

export template <typename... Ts>
auto toComputed(Union<Ts...> const& value, auto const& ctx);

export template <typename... Ts>
auto toSpecified(Union<Ts...> const& value);

export template <typename T>
auto computeValue(T const& value, auto const& ctx) {
    if constexpr (requires { toComputed(value, ctx); })
        return toComputed(value, ctx);
    else
        return toComputed(value);
}

export template <typename... Ts>
auto toComputed(Union<Ts...> const& value, auto const& ctx) {
    using Computed = Union<decltype(computeValue(Meta::declval<Ts const&>(), ctx))...>;
    return value.visit([&](auto const& v) -> Computed {
        return computeValue(v, ctx);
    });
}

export template <typename... Ts>
auto toSpecified(Union<Ts...> const& value) {
    using Specified = Union<decltype(toSpecified(Meta::declval<Ts const&>()))...>;
    return value.visit([](auto const& v) -> Specified {
        return toSpecified(v);
    });
}

export template <typename T>
auto toComputed(Math::Insets<T> const& value, LengthContext auto const& ctx) -> Math::Insets<decltype(toComputed(value.top, ctx))> {
    return {toComputed(value.top, ctx), toComputed(value.end, ctx), toComputed(value.bottom, ctx), toComputed(value.start, ctx)};
}

export template <typename T>
auto toComputed(Vec<T> const& value, auto const& ctx) -> Vec<decltype(toComputed(value[0], ctx))> {
    Vec<decltype(toComputed(value[0], ctx))> result{value.len()};
    for (auto const& v : value)
        result.pushBack(toComputed(v, ctx));
    return result;
}

export template <typename T>
auto toComputed(Math::Radii<T> const& value, LengthContext auto const& ctx) -> Math::Radii<decltype(toComputed(value.a, ctx))> {
    return {
        toComputed(value.a, ctx),
        toComputed(value.b, ctx),
        toComputed(value.c, ctx),
        toComputed(value.d, ctx),
        toComputed(value.e, ctx),
        toComputed(value.f, ctx),
        toComputed(value.g, ctx),
        toComputed(value.h, ctx),
    };
}

export template <typename T>
auto toSpecified(Vec<T> const& value) -> Vec<decltype(toSpecified(value[0]))> {
    Vec<decltype(toSpecified(value[0]))> result{value.len()};
    for (auto const& v : value)
        result.pushBack(toSpecified(v));
    return result;
}

export template <typename T>
auto toSpecified(Math::Insets<T> const& value) -> Math::Insets<decltype(toSpecified(value.top))> {
    return {toSpecified(value.top), toSpecified(value.end), toSpecified(value.bottom), toSpecified(value.start)};
}

export template <typename T>
auto toSpecified(Math::Radii<T> const& value) -> Math::Radii<decltype(toSpecified(value.a))> {
    return {
        toSpecified(value.a),
        toSpecified(value.b),
        toSpecified(value.c),
        toSpecified(value.d),
        toSpecified(value.e),
        toSpecified(value.f),
        toSpecified(value.g),
        toSpecified(value.h),
    };
}

} // namespace Vaev::Style
