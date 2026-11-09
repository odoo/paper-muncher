export module Vaev.Engine:style.values.generic;

import Karm.Core;
import Karm.Math;

import :style.values.unitValue;
import :style.values.keywords;

using namespace Karm;

namespace Vaev::Style {

export template <typename... Ts>
auto toComputed(Union<Ts...> const& value, auto const& ctx);

export template <typename... Ts>
auto toSpecified(Union<Ts...> const& value);

template <typename T>
auto _toComputedAlternative(T const& value, auto const& ctx) {
    if constexpr (requires { toComputed(value, ctx); })
        return toComputed(value, ctx);
    else
        return toComputed(value);
}

export template <typename... Ts>
auto toComputed(Union<Ts...> const& value, auto const& ctx) {
    using Computed = Union<decltype(_toComputedAlternative(Meta::declval<Ts const&>(), ctx))...>;
    return value.visit([&](auto const& v) -> Computed {
        return _toComputedAlternative(v, ctx);
    });
}

export template <typename... Ts>
auto toSpecified(Union<Ts...> const& value) {
    using Specified = Union<decltype(toSpecified(Meta::declval<Ts const&>()))...>;
    return value.visit([](auto const& v) -> Specified {
        return toSpecified(v);
    });
}

} // namespace Vaev::Style

namespace Vaev::Style {

export template <typename T>
auto toComputed(Math::Insets<T> const& value, LengthContext auto const& ctx) -> Math::Insets<decltype(toComputed(value.top, ctx))> {
    return {toComputed(value.top, ctx), toComputed(value.end, ctx), toComputed(value.bottom, ctx), toComputed(value.start, ctx)};
}

export template <typename T>
auto toSpecified(Math::Insets<T> const& value) -> Math::Insets<decltype(toSpecified(value.top))> {
    return {toSpecified(value.top), toSpecified(value.end), toSpecified(value.bottom), toSpecified(value.start)};
}

} // namespace Vaev::Style
