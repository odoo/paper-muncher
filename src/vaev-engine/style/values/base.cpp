export module Vaev.Engine:style.values.base;

import Karm.Core;
import Karm.Math;

using namespace Karm;

namespace Vaev::Style {

export template <typename T>
struct ValueComputer {};

export template <typename T, typename... Args>
auto computeValue(T const& value, Args const&... args) {
    if constexpr (requires { ValueComputer<T>::compute(value, args...); }) {
        return ValueComputer<T>::compute(value, args...);
    } else {
        static_assert(sizeof...(Args) <= 1);
        return ValueComputer<T>::compute(value);
    }
}

export template <typename S, typename C>
S specifyValue(C const& value) {
    if constexpr (Meta::Same<S, C>)
        return value;
    else
        return ValueComputer<S>::specify(value);
}

// MARK: Union -----------------------------------------------------------------

export template <typename... Ts>
struct ValueComputer<Union<Ts...>> {
    static auto compute(Union<Ts...> const& value, auto const& ctx) {
        using Computed = Union<decltype(computeValue(Meta::declval<Ts const&>(), ctx))...>;
        return value.visit([&](auto const& v) -> Computed {
            return computeValue(v, ctx);
        });
    }

    template <typename... Cs>
        requires(sizeof...(Cs) == sizeof...(Ts))
    static Union<Ts...> specify(Union<Cs...> const& value) {
        Opt<Union<Ts...>> result;
        ([&] {
            if (auto const& [v] = value.template is<Cs>())
                result = Some(Union<Ts...>{specifyValue<Ts>(v)});
        }(), ...);
        return result.take();
    }
};

// MARK: Vec -------------------------------------------------------------------

export template <typename T>
struct ValueComputer<Vec<T>> {
    static auto compute(Vec<T> const& value, auto const& ctx) -> Vec<decltype(computeValue(value[0], ctx))> {
        Vec<decltype(computeValue(value[0], ctx))> result{value.len()};
        for (auto const& v : value)
            result.pushBack(computeValue(v, ctx));
        return result;
    }

    template <typename C>
    static Vec<T> specify(Vec<C> const& value) {
        Vec<T> result{value.len()};
        for (auto const& v : value)
            result.pushBack(specifyValue<T>(v));
        return result;
    }
};

// MARK: Insets ----------------------------------------------------------------

export template <typename T>
struct ValueComputer<Math::Insets<T>> {
    static auto compute(Math::Insets<T> const& value, auto const& ctx) -> Math::Insets<decltype(computeValue(value.top, ctx))> {
        return {computeValue(value.top, ctx), computeValue(value.end, ctx), computeValue(value.bottom, ctx), computeValue(value.start, ctx)};
    }

    template <typename C>
    static Math::Insets<T> specify(Math::Insets<C> const& value) {
        return {specifyValue<T>(value.top), specifyValue<T>(value.end), specifyValue<T>(value.bottom), specifyValue<T>(value.start)};
    }
};

// MARK: Radii -----------------------------------------------------------------

export template <typename T>
struct ValueComputer<Math::Radii<T>> {
    static auto compute(Math::Radii<T> const& value, auto const& ctx) -> Math::Radii<decltype(computeValue(value.a, ctx))> {
        return {
            computeValue(value.a, ctx),
            computeValue(value.b, ctx),
            computeValue(value.c, ctx),
            computeValue(value.d, ctx),
            computeValue(value.e, ctx),
            computeValue(value.f, ctx),
            computeValue(value.g, ctx),
            computeValue(value.h, ctx),
        };
    }

    template <typename C>
    static Math::Radii<T> specify(Math::Radii<C> const& value) {
        return {
            specifyValue<T>(value.a),
            specifyValue<T>(value.b),
            specifyValue<T>(value.c),
            specifyValue<T>(value.d),
            specifyValue<T>(value.e),
            specifyValue<T>(value.f),
            specifyValue<T>(value.g),
            specifyValue<T>(value.h),
        };
    }
};

} // namespace Vaev::Style
