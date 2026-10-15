module;

#include <karm/macros>

export module Vaev.Engine:values.base;

import Karm.Core;
import :css;

using namespace Karm;

namespace Vaev {

export void eatWhitespace(Cursor<Css::Sst>& c) {
    while (not c.ended() and c.peek() == Css::Token::WHITESPACE)
        c.next();
}

// https://www.w3.org/TR/css-values-4/#comb-comma
export bool skipOmmitableComma(Cursor<Css::Sst>& c) {
    eatWhitespace(c);
    bool res = c.skip(Css::Token::COMMA);
    eatWhitespace(c);
    return res;
}

// MARK: Value Traits ----------------------------------------------------------

// Everything the engine knows about a type of CSS value, every facet is optional:
//
//  - parse(Cursor<Css::Sst>&) -> Res<T>
//  - serialize(Io::TextWriter&, T const&) -> Res<>
//  - Computed, compute(T const&, ctx) -> Computed and specify(Computed const&) -> T
//    https://drafts.csswg.org/css-cascade/#computed
//  - Resolved, resolve(T const&, ...) -> Resolved
//  - DIMENSION_TYPE, toDimension(T const&) and fromDimension(Dimension), for values that map to a dimension
//
// Values must be used through the functions bellow, a value without a computed or
// resolved facet computes and resolves to itself. Unions, enums and lists get their
// facets from UnionValueTraits, EnumValueTraits and ListValueTraits, a specialization
// for one of them must inherit the matching base and override what it needs.
export template <typename T>
struct ValueTraits;

export template <typename U>
struct UnionValueTraits;

template <typename T>
constexpr bool _isUnion = false;

template <typename... Ts>
constexpr bool _isUnion<Union<Ts...>> = true;

template <typename T>
constexpr void _checkTraits() {
    if constexpr (_isUnion<T>)
        static_assert(Meta::Derive<ValueTraits<T>, UnionValueTraits<T>>, "the traits of a union must inherit UnionValueTraits");
}

// MARK: Parsing ---------------------------------------------------------------

export template <typename T>
concept ValueParseable = requires(Cursor<Css::Sst>& c) {
    ValueTraits<T>::parse(c);
};

export template <typename T>
Res<T> parseValue(Cursor<Css::Sst>& c) {
    _checkTraits<T>();
    return ValueTraits<T>::parse(c);
}

export template <typename T>
Res<T> parseValue(Str str) {
    Css::Lexer lex{str};
    auto diags = Diag::Collector::ignore();
    auto [sst, _] = Css::consumeDeclarationValue(lex, diags);
    Cursor<Css::Sst> content{sst};
    return parseValue<T>(content);
}

// MARK: Serialization ---------------------------------------------------------

export template <typename T>
concept ValueSerializable = requires(Io::TextWriter& w, T const& value) {
    ValueTraits<T>::serialize(w, value);
};

export template <typename T>
Res<> serializeValue(Io::TextWriter& w, T const& value) {
    _checkTraits<T>();
    return ValueTraits<T>::serialize(w, value);
}

// MARK: Computed Value --------------------------------------------------------
// https://drafts.csswg.org/css-cascade/#computed

// Values made of other values are written once, parametrized by the stage
// they are in: Foo<Specified> as parsed, Foo<Computed> once computed.
export template <typename T>
using Specified = T;

template <typename T>
struct _ComputedOf {
    using Type = T;
};

template <typename T>
    requires requires { typename ValueTraits<T>::Computed; }
struct _ComputedOf<T> {
    using Type = typename ValueTraits<T>::Computed;
};

// The type of the computed value of a value.
export template <typename T>
using Computed = typename _ComputedOf<T>::Type;

export template <typename T>
Computed<T> compute(T const& value, auto const& ctx) {
    _checkTraits<T>();
    if constexpr (requires { ValueTraits<T>::compute(value, ctx); }) {
        return ValueTraits<T>::compute(value, ctx);
    } else {
        static_assert(Meta::Same<Computed<T>, T>, "value has a computed type but no compute()");
        return value;
    }
}

// Turns a computed value back into a specified value, so it can be fed back
// into the cascade (inheritance, toggle(), serialization, ...)
export template <typename T>
T specified(Computed<T> const& value) {
    _checkTraits<T>();
    if constexpr (requires { ValueTraits<T>::specify(value); }) {
        return ValueTraits<T>::specify(value);
    } else {
        static_assert(Meta::Same<Computed<T>, T>, "value has a computed type but no specify()");
        return value;
    }
}

// MARK: Resolved Value --------------------------------------------------------

template <typename T>
struct _ResolvedOf {
    using Type = T;
};

template <typename T>
    requires requires { typename ValueTraits<T>::Resolved; }
struct _ResolvedOf<T> {
    using Type = typename ValueTraits<T>::Resolved;
};

// The type of a value once resolved, ready to be used by layout and paint.
export template <typename T>
using Resolved = typename _ResolvedOf<T>::Type;

export template <typename T, typename... Args>
    requires requires(T const& value, Args const&... args) { ValueTraits<T>::resolve(value, args...); }
auto resolve(T const& value, Args const&... args) {
    return ValueTraits<T>::resolve(value, args...);
}

// MARK: Union -----------------------------------------------------------------
// https://drafts.csswg.org/css-values-4/#comb-one

export template <typename... Ts>
struct UnionValueTraits<Union<Ts...>> {
    using Computed = Union<Vaev::Computed<Ts>...>;

    static Res<Union<Ts...>> parse(Cursor<Css::Sst>& c)
        requires(ValueParseable<Ts> and ...)
    {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        return Meta::any<Ts...>([&c]<typename T>() -> Res<Union<Ts...>> {
            return Ok(try$(parseValue<T>(c)));
        });
    }

    static Res<> serialize(Io::TextWriter& w, Union<Ts...> const& value)
        requires(ValueSerializable<Ts> and ...)
    {
        return value.visit([&](auto const& inner) {
            return serializeValue(w, inner);
        });
    }

    static Computed compute(Union<Ts...> const& value, auto const& ctx) {
        return value.visit([&](auto const& inner) -> Computed {
            return Vaev::compute(inner, ctx);
        });
    }

    static Union<Ts...> specify(Computed const& value) {
        Opt<Union<Ts...>> result;
        (
            [&] {
                if (result)
                    return;
                if (auto inner = value.template is<Vaev::Computed<Ts>>())
                    result = Some(Union<Ts...>{specified<Ts>(*inner)});
            }(),
            ...);
        return result.take();
    }
};

export template <typename... Ts>
struct ValueTraits<Union<Ts...>> : UnionValueTraits<Union<Ts...>> {};

// MARK: Enum ------------------------------------------------------------------

// An enum variant is serialized as if it was a keyword.
export template <typename E>
struct EnumValueTraits {
    static Res<> serialize(Io::TextWriter& w, E const& value) {
        for (auto r : iterRunes(nameOf(value))) {
            if (r == '_')
                try$(w.writeRune('-'));
            else
                try$(w.writeRune(toAsciiLower(r)));
        }
        return Ok();
    }
};

export template <Meta::Enum E>
struct ValueTraits<E> : EnumValueTraits<E> {};

// MARK: List ------------------------------------------------------------------

// The items of a list are serialized separated by a space.
export template <typename S>
struct ListValueTraits {
    static Res<> serialize(Io::TextWriter& w, S const& value)
        requires ValueSerializable<typename S::Inner>
    {
        bool first = true;
        for (auto const& item : value) {
            if (not first)
                try$(w.writeRune(' '));
            first = false;
            try$(serializeValue(w, item));
        }
        return Ok();
    }
};

export template <Sliceable S>
struct ValueTraits<S> : ListValueTraits<S> {};

} // namespace Vaev
