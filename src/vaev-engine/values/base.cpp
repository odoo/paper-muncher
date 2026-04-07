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
    _WritingMode writingMode = _WritingMode::HORIZONTAL_TB;
    _Viewport viewport = {.small = {800, 600}}; /// Viewport of the current box
    Math::Vec2f displayArea = {800, 600};
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

export template <typename T>
using Computed = ValueTraits<T>::Computed;

export template <typename T>
Computed<T> computeValue(T const& a, ComputationContext& ctx) {
    return ValueTraits<T>::compute(a, ctx);
}

export template <typename T>
concept ValueComputable = requires(T const& a, ComputationContext& ctx) {
    { computeValue(a, ctx) };
};

export template <typename T>
concept Value = ValueParseable<T> and ValueComputable<T>;

export template <typename T>
struct DefaultValueTraits {
    using Computed = T;

    static Computed compute(T const& val, ComputationContext&) { return val; }
};

export struct ResolutionContext {
    Au inlineSize = 0_au;
    Opt<Au> blockSize = NONE;
};

export template <typename T>
struct ComputedValueTraits {
    using Resolved = T;

    static Resolved resolve(T const& val, ResolutionContext const&) { return val; }
};

export template <typename T>
using __Resolved = ComputedValueTraits<T>::Resolved;

export template <typename T>
__Resolved<T> resolveValue(T const& a, ResolutionContext const& ctx) {
    return ComputedValueTraits<T>::resolve(a, ctx);
}

export template <typename T>
concept ComputedValue = requires(T const& a, ResolutionContext const& ctx) {
    { resolveValue(a, ctx) };
};

} // namespace Vaev
