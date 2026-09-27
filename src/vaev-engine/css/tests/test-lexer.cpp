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
    assertEq$(t.type, Token::IDENT);
    assertEq$(t.data, "hello"s);

    t = lex("hello-world");
    assertEq$(t.type, Token::IDENT);
    assertEq$(t.data, "hello-world"s);

    t = lex("hello-world-123");
    assertEq$(t.type, Token::IDENT);
    assertEq$(t.data, "hello-world-123"s);
    return Ok();
}

test$("vaev-css-lex-function") {
    auto t = lex("func(");
    assertEq$(t.type, Token::FUNCTION);
    assertEq$(t.data, "func("s);

    return Ok();
}

test$("vaev-css-lex-at-keyword") {
    auto t = lex("@keyframes");
    assertEq$(t.type, Token::AT_KEYWORD);
    assertEq$(t.data, "@keyframes"s);

    return Ok();
}

test$("vaev-css-lex-hash") {
    auto t = lex("#foo");
    assertEq$(t.type, Token::HASH);
    assertEq$(t.data, "#foo"s);

    return Ok();
}

test$("vaev-css-lex-strings") {
    auto t = lex("''");
    assertEq$(t.type, Token::STRING);
    assertEq$(t.data, ""s);

    t = lex("\"\"");
    assertEq$(t.type, Token::STRING);
    assertEq$(t.data, ""s);

    t = lex(R"("\"")");
    assertEq$(t.type, Token::STRING);

    t = lex("\"abc\"");
    assertEq$(t.type, Token::STRING);

    t = lex("'abc'");
    assertEq$(t.type, Token::STRING);

    t = lex("' Hello World !'");
    assertEq$(t.type, Token::STRING);

    return Ok();
}

test$("vaev-css-lex-url") {
    auto t = lex("url('')");
    assertEq$(t.type, Token::FUNCTION);
    assertEq$(t.data, "url("s);

    t = lex("url('abc')");
    assertEq$(t.type, Token::FUNCTION);
    assertEq$(t.data, "url("s);

    t = lex("url(\"abc\")");
    assertEq$(t.type, Token::FUNCTION);
    assertEq$(t.data, "url("s);

    t = lex("url(abc)");
    assertEq$(t.type, Token::URL);
    assertEq$(t.data, "url(abc)"s);

    t = lex("url(http://example.com)");
    assertEq$(t.type, Token::URL);
    assertEq$(t.data, "url(http://example.com)"s);

    return Ok();
}

test$("vaev-css-lex-delim") {
    auto t = lex("!");
    assertEq$(t.type, Token::DELIM);
    assertEq$(t.data, "!"s);

    t = lex("+");
    assertEq$(t.type, Token::DELIM);
    assertEq$(t.data, "+"s);

    t = lex("-");
    assertEq$(t.type, Token::DELIM);
    assertEq$(t.data, "-"s);

    return Ok();
}

test$("vaev-css-lex-numbers") {
    auto t = lex("123");
    assertEq$(t.type, Token::NUMBER);
    assertEq$(t.data, "123"s);

    t = lex("123.456");
    assertEq$(t.type, Token::NUMBER);
    assertEq$(t.data, "123.456"s);

    t = lex("123.456e7");
    assertEq$(t.type, Token::NUMBER);
    assertEq$(t.data, "123.456e7"s);

    t = lex("123.456E7");
    assertEq$(t.type, Token::NUMBER);
    assertEq$(t.data, "123.456E7"s);

    t = lex("123.456E7");
    assertEq$(t.type, Token::NUMBER);
    assertEq$(t.data, "123.456E7"s);

    t = lex("-123.456E7");
    assertEq$(t.type, Token::NUMBER);
    assertEq$(t.data, "-123.456E7"s);

    t = lex("123.456E7");
    assertEq$(t.type, Token::NUMBER);
    assertEq$(t.data, "123.456E7"s);

    t = lex("123.456E-7");
    assertEq$(t.type, Token::NUMBER);
    assertEq$(t.data, "123.456E-7"s);

    return Ok();
}

test$("vaev-css-lex-percentage") {
    auto t = lex("123%");
    assertEq$(t.type, Token::PERCENTAGE);
    assertEq$(t.data, "123%"s);

    return Ok();
}

test$("vaev-css-lex-dimension") {
    auto t = lex("123px");
    assertEq$(t.type, Token::DIMENSION);
    assertEq$(t.data, "123px"s);

    t = lex("123.456px");
    assertEq$(t.type, Token::DIMENSION);
    assertEq$(t.data, "123.456px"s);

    t = lex("123.456e7px");
    assertEq$(t.type, Token::DIMENSION);

    t = lex("123.456E7px");
    assertEq$(t.type, Token::DIMENSION);

    t = lex("+123.456E7px");
    assertEq$(t.type, Token::DIMENSION);

    return Ok();
}

test$("vaev-css-lex-whitespace") {
    auto t = lex(" ");
    assertEq$(t.type, Token::WHITESPACE);
    assertEq$(t.data, " "s);

    t = lex("\t");
    assertEq$(t.type, Token::WHITESPACE);
    assertEq$(t.data, "\t"s);

    t = lex("\n");
    assertEq$(t.type, Token::WHITESPACE);
    assertEq$(t.data, "\n"s);

    t = lex("\r");
    assertEq$(t.type, Token::WHITESPACE);
    assertEq$(t.data, "\r"s);

    return Ok();
}

test$("vaev-css-lex-cdo-cdc") {
    auto t = lex("<!--");
    assertEq$(t.type, Token::CDO);
    assertEq$(t.data, "<!--"s);

    t = lex("-->");
    assertEq$(t.type, Token::CDC);
    assertEq$(t.data, "-->"s);

    return Ok();
}

test$("vaev-css-lex-colon") {
    auto t = lex(":");
    assertEq$(t.type, Token::COLON);
    assertEq$(t.data, ":"s);

    return Ok();
}

test$("vaev-css-lex-semicolon") {
    auto t = lex(";");
    assertEq$(t.type, Token::SEMICOLON);
    assertEq$(t.data, ";"s);

    return Ok();
}

test$("vaev-css-lex-comma") {
    auto t = lex(",");
    assertEq$(t.type, Token::COMMA);
    assertEq$(t.data, ","s);

    return Ok();
}

test$("vaev-css-lex-brackets") {
    auto t = lex("{");
    assertEq$(t.type, Token::LEFT_CURLY_BRACKET);
    assertEq$(t.data, "{"s);

    t = lex("}");
    assertEq$(t.type, Token::RIGHT_CURLY_BRACKET);
    assertEq$(t.data, "}"s);

    return Ok();
}

test$("vaev-css-lex-square-brackets") {
    auto t = lex("[");
    assertEq$(t.type, Token::LEFT_SQUARE_BRACKET);
    assertEq$(t.data, "["s);

    t = lex("]");
    assertEq$(t.type, Token::RIGHT_SQUARE_BRACKET);
    assertEq$(t.data, "]"s);

    return Ok();
}

test$("vaev-css-lex-parenthesis") {
    auto t = lex("(");
    assertEq$(t.type, Token::LEFT_PARENTHESIS);
    assertEq$(t.data, "("s);

    t = lex(")");
    assertEq$(t.type, Token::RIGHT_PARENTHESIS);
    assertEq$(t.data, ")"s);

    return Ok();
}

test$("vaev-css-lex-comment") {
    auto t = lex("/* comment */");
    assertEq$(t.type, Token::COMMENT);
    assertEq$(t.data, "/* comment */"s);

    auto t2 = lex("/* unterminated comment");
    assertEq$(t2.type, Token::COMMENT);
    assertEq$(t2.data, "/* unterminated comment"s);

    return Ok();
}

test$("vaev-css-lex-end-of-file") {
    auto t = lex("");
    assertEq$(t.type, Token::END_OF_FILE);
    assertEq$(t.data, ""s);

    return Ok();
}

}; // namespace Vaev::Css::Tests
