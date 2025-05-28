#pragma once

#include <vaev-css/parser.h>

namespace Vaev {

static inline void eatWhitespace(Cursor<Css::Sst>& c) {
    while (not c.ended() and c.peek() == Css::Token::WHITESPACE)
        c.next();
}

template <typename T>
struct ValueParser;

template <typename T>
always_inline static Res<T> parseValue(Cursor<Css::Sst>& c) {
    return ValueParser<T>::parse(c);
}

template <typename T>
always_inline static Res<T> parseValue(Str str) {
    Css::Lexer lex{str};
    auto content = Css::consumeDeclarationValue(lex);
    Cursor<Css::Sst> c{content};
    return ValueParser<T>::parse(c);
}

template <typename T>
concept ValueParseable = requires(T a, Cursor<Css::Sst> c) {
    parseValue<T>(c);
};

} // namespace Vaev
