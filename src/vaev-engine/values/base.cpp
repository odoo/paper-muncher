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
    RectAu small;
    // https://drafts.csswg.org/css-values/#large-viewport-size
    RectAu large = small;
    // https://drafts.csswg.org/css-values/#dynamic-viewport-size
    RectAu dynamic = small;
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
    _Viewport viewport = {.small = {800_au, 600_au}}; /// Viewport of the current box
    Vec2Au displayArea = {800_au, 600_au};
};

export template <typename T>
struct ValueTraits {
    using Computed = T;

    static Computed compute(T const& val, [[maybe_unused]] ComputationContext& ctx) {
        return val;
    }
};

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

} // namespace Vaev
