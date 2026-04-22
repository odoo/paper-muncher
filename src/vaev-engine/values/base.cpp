module;

#include <karm/macros>

export module Vaev.Engine:values.base;

import Karm.Core;
import Karm.Math;
import Karm.Gfx;

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

// FIXME: Those types should be extracted to a place where they can't cause recursive imports.
export struct _Viewport {
    // https://drafts.csswg.org/css-values/#small-viewport-size
    Math::Rectf small;
    // https://drafts.csswg.org/css-values/#large-viewport-size
    Math::Rectf large = small;
    // https://drafts.csswg.org/css-values/#dynamic-viewport-size
    Math::Rectf dynamic = small;
};

export enum struct _WritingMode : u8 {
    HORIZONTAL_TB,
    VERTICAL_RL,
    VERTICAL_LR,
};

struct ComputationContext {
    // FIXME: Fonts are not optional but are wrapped in Opt<> due to the lack of a default constructor.
    //        They also require mutability for caching stuff which forces ComputationContext to be passed
    //        by mut reference. Ideally they should be replaced by a richer version of
    Opt<Gfx::Font> rootFont = NONE;
    Opt<Gfx::Font> font = NONE;
    Gfx::Color currentColor = Gfx::BLACK;
    _WritingMode writingMode = _WritingMode::HORIZONTAL_TB;
    _Viewport viewport = {.small = {800, 600}}; /// Viewport of the current box
    Math::Vec2f displayArea = {800, 600};

    ComputationContext withCurrentColor(Gfx::Color currentColor) {
        ComputationContext newCtx = *this;
        newCtx.currentColor = currentColor;
        return newCtx;
    }
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

export template <ValueParseable T>
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
        if (auto val = parseValue<Meta::RemoveConstVolatileRef<decltype(T{}.*F1)>>(c)) {
            if (fstSet)
                return Error::invalidData("setting value twice");

            result.*F1 = val.take();
            fstSet = true;
            continue;
        }

        if (auto val = parseValue<Meta::RemoveConstVolatileRef<decltype(T{}.*F2)>>(c)) {
            if (sndSet)
                return Error::invalidData("setting value twice");

            result.*F2 = val.take();
            sndSet = true;
            continue;
        }

        if (auto val = parseValue<Meta::RemoveConstVolatileRef<decltype(T{}.*F3)>>(c)) {
            if (thrdSet)
                return Error::invalidData("setting value twice");

            result.*F3 = val.take();
            thrdSet = true;
            continue;
        }

        return Error::invalidData("invalid value");
    }

    return Ok(result);
}

export template <typename T>
using Computed = typename ValueTraits<T>::ComputedType;

export template <typename T>
ValueTraits<T>::ComputedType computeValue(T const& value, ComputationContext const& ctx) {
    return ValueTraits<T>::compute(value, ctx);
}

export template <typename T>
T valueFromComputed(Computed<T> const& computed) {
    return ValueTraits<T>::fromComputed(computed);
}

export template <typename T>
concept ValueComputable = requires(T const& a, ComputationContext const& ctx) {
    { computeValue(a, ctx) };
};

export template <typename T>
concept ValueFromComputed = requires(Computed<T> const& a) {
    { valueFromComputed<T>(a) };
};

export template <typename T>
concept Value = ValueParseable<T> and ValueComputable<T> and ValueFromComputed<T>;

export template <typename T>
struct DefaultValueTraits {
    using ComputedType = T;

    static ComputedType compute(T const& val, ComputationContext const&) { return val; }

    static T fromComputed(ComputedType const& computed) { return computed; }
};

// FIXME: Move somewhere in layout
export template <typename T>
concept PercentValue = requires(T const& a, Au relative) {
    { resolvePercent(a, relative) };
};

} // namespace Vaev
