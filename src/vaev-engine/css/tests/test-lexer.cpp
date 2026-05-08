#include <karm/test>

import Vaev.Engine;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Css::Tests {

Token lex(Str input) {
    return Lexer{input}.peek();
}

test$("vaev-css-lex-ident") {
    auto t = lex("hello");
    expectEq$(t.type, Token::IDENT);
    expectEq$(t.data, "hello"s);

    t = lex("hello-world");
    expectEq$(t.type, Token::IDENT);
    expectEq$(t.data, "hello-world"s);

    t = lex("hello-world-123");
    expectEq$(t.type, Token::IDENT);
    expectEq$(t.data, "hello-world-123"s);
    return Ok();
}

test$("vaev-css-lex-function") {
    auto t = lex("func(");
    expectEq$(t.type, Token::FUNCTION);
    expectEq$(t.data, "func("s);

    return Ok();
}

test$("vaev-css-lex-at-keyword") {
    auto t = lex("@keyframes");
    expectEq$(t.type, Token::AT_KEYWORD);
    expectEq$(t.data, "@keyframes"s);

    return Ok();
}

test$("vaev-css-lex-hash") {
    auto t = lex("#foo");
    expectEq$(t.type, Token::HASH);
    expectEq$(t.data, "#foo"s);

    return Ok();
}

test$("vaev-css-lex-strings") {
    auto t = lex("''");
    expectEq$(t.type, Token::STRING);
    expectEq$(t.data, ""s);

    t = lex("\"\"");
    expectEq$(t.type, Token::STRING);
    expectEq$(t.data, ""s);

    t = lex(R"("\"")");
    expectEq$(t.type, Token::STRING);

    t = lex("\"abc\"");
    expectEq$(t.type, Token::STRING);

    t = lex("'abc'");
    expectEq$(t.type, Token::STRING);

    t = lex("' Hello World !'");
    expectEq$(t.type, Token::STRING);

    return Ok();
}

test$("vaev-css-lex-url") {
    auto t = lex("url('')");
    expectEq$(t.type, Token::FUNCTION);
    expectEq$(t.data, "url("s);

    t = lex("url('abc')");
    expectEq$(t.type, Token::FUNCTION);
    expectEq$(t.data, "url("s);

    t = lex("url(\"abc\")");
    expectEq$(t.type, Token::FUNCTION);
    expectEq$(t.data, "url("s);

    t = lex("url(abc)");
    expectEq$(t.type, Token::URL);
    expectEq$(t.data, "url(abc)"s);

    t = lex("url(http://example.com)");
    expectEq$(t.type, Token::URL);
    expectEq$(t.data, "url(http://example.com)"s);

    return Ok();
}

test$("vaev-css-lex-delim") {
    auto t = lex("!");
    expectEq$(t.type, Token::DELIM);
    expectEq$(t.data, "!"s);

    t = lex("+");
    expectEq$(t.type, Token::DELIM);
    expectEq$(t.data, "+"s);

    t = lex("-");
    expectEq$(t.type, Token::DELIM);
    expectEq$(t.data, "-"s);

    return Ok();
}

test$("vaev-css-lex-numbers") {
    auto t = lex("123");
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "123"s);

    t = lex("123.456");
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "123.456"s);

    t = lex("123.456e7");
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "123.456e7"s);

    t = lex("123.456E7");
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "123.456E7"s);

    t = lex("123.456E7");
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "123.456E7"s);

    t = lex("-123.456E7");
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "-123.456E7"s);

    t = lex("123.456E7");
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "123.456E7"s);

    t = lex("123.456E-7");
    expectEq$(t.type, Token::NUMBER);
    expectEq$(t.data, "123.456E-7"s);

    return Ok();
}

test$("vaev-css-lex-percentage") {
    auto t = lex("123%");
    expectEq$(t.type, Token::PERCENTAGE);
    expectEq$(t.data, "123%"s);

    return Ok();
}

test$("vaev-css-lex-dimension") {
    auto t = lex("123px");
    expectEq$(t.type, Token::DIMENSION);
    expectEq$(t.data, "123px"s);

    t = lex("123.456px");
    expectEq$(t.type, Token::DIMENSION);
    expectEq$(t.data, "123.456px"s);

    t = lex("123.456e7px");
    expectEq$(t.type, Token::DIMENSION);

    t = lex("123.456E7px");
    expectEq$(t.type, Token::DIMENSION);

    t = lex("+123.456E7px");
    expectEq$(t.type, Token::DIMENSION);

    return Ok();
}

test$("vaev-css-lex-whitespace") {
    auto t = lex(" ");
    expectEq$(t.type, Token::WHITESPACE);
    expectEq$(t.data, " "s);

    t = lex("\t");
    expectEq$(t.type, Token::WHITESPACE);
    expectEq$(t.data, "\t"s);

    t = lex("\n");
    expectEq$(t.type, Token::WHITESPACE);
    expectEq$(t.data, "\n"s);

    t = lex("\r");
    expectEq$(t.type, Token::WHITESPACE);
    expectEq$(t.data, "\r"s);

    return Ok();
}

test$("vaev-css-lex-cdo-cdc") {
    auto t = lex("<!--");
    expectEq$(t.type, Token::CDO);
    expectEq$(t.data, "<!--"s);

    t = lex("-->");
    expectEq$(t.type, Token::CDC);
    expectEq$(t.data, "-->"s);

    return Ok();
}

test$("vaev-css-lex-colon") {
    auto t = lex(":");
    expectEq$(t.type, Token::COLON);
    expectEq$(t.data, ":"s);

    return Ok();
}

test$("vaev-css-lex-semicolon") {
    auto t = lex(";");
    expectEq$(t.type, Token::SEMICOLON);
    expectEq$(t.data, ";"s);

    return Ok();
}

test$("vaev-css-lex-comma") {
    auto t = lex(",");
    expectEq$(t.type, Token::COMMA);
    expectEq$(t.data, ","s);

    return Ok();
}

test$("vaev-css-lex-brackets") {
    auto t = lex("{");
    expectEq$(t.type, Token::LEFT_CURLY_BRACKET);
    expectEq$(t.data, "{"s);

    t = lex("}");
    expectEq$(t.type, Token::RIGHT_CURLY_BRACKET);
    expectEq$(t.data, "}"s);

    return Ok();
}

test$("vaev-css-lex-square-brackets") {
    auto t = lex("[");
    expectEq$(t.type, Token::LEFT_SQUARE_BRACKET);
    expectEq$(t.data, "["s);

    t = lex("]");
    expectEq$(t.type, Token::RIGHT_SQUARE_BRACKET);
    expectEq$(t.data, "]"s);

    return Ok();
}

test$("vaev-css-lex-parenthesis") {
    auto t = lex("(");
    expectEq$(t.type, Token::LEFT_PARENTHESIS);
    expectEq$(t.data, "("s);

    t = lex(")");
    expectEq$(t.type, Token::RIGHT_PARENTHESIS);
    expectEq$(t.data, ")"s);

    return Ok();
}

test$("vaev-css-lex-comment") {
    auto t = lex("/* comment */");
    expectEq$(t.type, Token::COMMENT);
    expectEq$(t.data, "/* comment */"s);

    auto t2 = lex("/* unterminated comment");
    expectEq$(t2.type, Token::COMMENT);
    expectEq$(t2.data, "/* unterminated comment"s);

    return Ok();
}

test$("vaev-css-lex-end-of-file") {
    auto t = lex("");
    expectEq$(t.type, Token::END_OF_FILE);
    expectEq$(t.data, ""s);

    return Ok();
}

}; // namespace Vaev::Css::Tests
