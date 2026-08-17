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

export template <typename T>
struct ValueTraits;

template <typename... Ts>
using Resolved = decltype(resolve(Meta::declref<Ts>()...));

template <typename T>
using CanonicalUnit = typename ValueTraits<T>::CanonicalUnit;

template <typename T>
concept Canonical = Meta::Same<T, CanonicalUnit<T>>;

export template <typename T>
concept Parseable = requires() {
    ValueTraits<T>::parse;
};

export auto resolve(Canonical auto value, ...) {
    return value;
}

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

} // namespace Vaev
