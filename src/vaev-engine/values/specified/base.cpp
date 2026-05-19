export module Vaev.Engine:values.specified.base;

import Karm.Core;
import Karm.Math;
import Karm.Gfx;

import :css;
import :style.computed;

using namespace Karm;

namespace Vaev::Experimental {

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

// FIXME: Move it somewhere else
export struct _Viewport {
    // https://drafts.csswg.org/css-values/#small-viewport-size
    Math::Rectf small;
    // https://drafts.csswg.org/css-values/#large-viewport-size
    Math::Rectf large = small;
    // https://drafts.csswg.org/css-values/#dynamic-viewport-size
    Math::Rectf dynamic = small;
};

export struct ComputationContext {
    Opt<Gfx::Font> rootFont;
    Px userFontSize;
    _Viewport viewport;
    Math::Vec2f displayArea;
};

export template <typename>
struct ValueTraits {};

export template <typename T>
Res<T> parseValue(Cursor<Css::Sst>& c) {
    return ValueTraits<T>::parse(c);
}

export template <typename T>
Res<T> parseValue(Str str) {
    Css::Lexer lex{str};
    auto diags = Diag::Collector::ignore();
    auto [sst, _] = Css::consumeDeclarationValue(lex, diags);
    Cursor<Css::Sst> content{sst};
    return ValueTraits<T>::parse(content);
}

export template <typename T>
concept ValueParseable = requires(T a, Cursor<Css::Sst> c) {
    parseValue<T>(c);
};

export template <typename T>
Res<T> parseOneOf(Cursor<Css::Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    return T::any([&]<typename E>() -> Res<T> {
        return parseValue<E>(c);
    });
}

export template <Meta::DefaultConstructible T, auto T::* F1, auto T::* F2, auto T::* F3>
Res<T> parseOneOrMoreUnordered(Cursor<Css::Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    auto result = T{};

    bool fstSet = false;
    bool sndSet = false;
    bool thrdSet = false;

    while (not c.ended()) {
        auto rollback = c.rollbackPoint();

        if (auto val = parseValue<Meta::RemoveConstVolatileRef<decltype(T{}.*F1)>>(c)) {
            if (fstSet)
                return Ok(result);

            result.*F1 = val.take();
            fstSet = true;
            rollback.disarm();
            continue;
        }

        if (auto val = parseValue<Meta::RemoveConstVolatileRef<decltype(T{}.*F2)>>(c)) {
            if (sndSet)
                return Ok(result);

            result.*F2 = val.take();
            sndSet = true;
            rollback.disarm();
            continue;
        }

        if (auto val = parseValue<Meta::RemoveConstVolatileRef<decltype(T{}.*F3)>>(c)) {
            if (thrdSet)
                return Ok(result);

            result.*F3 = val.take();
            thrdSet = true;
            rollback.disarm();
            continue;
        }

        return Error::invalidData("invalid value");
    }

    return Ok(result);
}

export template <typename T>
using Computed = typename ValueTraits<T>::ComputedType;

export template <typename T>
ValueTraits<T>::ComputedType computeValue(T const& value, ComputationContext const& ctx, Style::ComputedValues const& computed) {
    return ValueTraits<T>::compute(value, ctx, computed);
}

export template <typename T>
T valueFromComputed(Computed<T> const& computed) {
    return ValueTraits<T>::fromComputed(computed);
}

export template <typename T>
concept ValueComputable = requires(T const& a, Computed<T> const& b, ComputationContext const& ctx) {
    { computeValue(a, ctx) };
    { valueFromComputed<T>(b) };
};

export template <typename T>
concept Value = ValueParseable<T> and ValueComputable<T>;

export template <typename T>
struct DefaultValueTraits {
    using ComputedType = T;

    static ComputedType compute(T const& val, ComputationContext const&, Style::ComputedValues const&) { return val; }

    static T fromComputed(ComputedType const& computed) { return computed; }
};

} // namespace Vaev::Experimental
