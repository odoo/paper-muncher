export module Vaev.Engine:style.values.generic;

import Karm.Core;

import :style.values.numeric;
import :values.keywords;

using namespace Karm;

namespace Vaev::Style {

// MARK: Unions ----------------------------------------------------------------
// A union computes alternative by alternative, so the computed value of
// Union<Ts...> is Union<Computed<Ts>...>, and back. Unions that need more than
// that (e.g. line-height, color) have their own, more specialized, overloads.
//
// NOTE: Overloads for the alternatives are found by ordinary lookup (the
//       partitions imported above) and by argument-dependent lookup on the
//       computation context, which lives in Vaev::Style.

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
