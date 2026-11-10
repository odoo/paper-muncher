module;

#include <karm/macros>

export module Vaev.Engine:css.values.base;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.serializer;

using namespace Karm;

namespace Vaev::Css {

export void eatWhitespace(Cursor<Sst>& c) {
    while (not c.ended() and c.peek() == Token::WHITESPACE)
        c.next();
}

// https://www.w3.org/TR/css-values-4/#comb-comma
export bool skipOmmitableComma(Cursor<Sst>& c) {
    eatWhitespace(c);
    bool res = c.skip(Token::COMMA);
    eatWhitespace(c);
    return res;
}

export template <typename T>
struct ValueParser;

export template <typename T>
concept ValueParseable = requires() {
    ValueParser<T>::parse;
};

export template <typename T>
Res<T> parseValue(Cursor<Sst>& c) {
    return ValueParser<T>::parse(c);
}

export template <typename T>
Res<T> parseValue(Str str) {
    Lexer lex{str};
    auto diags = Diag::Collector::ignore();
    auto [sst, _] = consumeDeclarationValue(lex, diags);
    Cursor<Sst> content{sst};
    auto res = try$(ValueParser<T>::parse(content));
    if (not content.ended())
        return Error::invalidData("unexpected tokens");
    return Ok(res);
}

export template <typename T>
struct ValueSerializer;

export template <typename T>
concept ValueSerializable = requires() {
    ValueSerializer<T>::serialize;
};

export template <typename T>
Res<> serializeValue(Io::TextWriter& w, T const& value) {
    return ValueSerializer<T>::serialize(w, value);
}

} // namespace Vaev::Css
