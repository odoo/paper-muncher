#include <karm/test>

import Vaev.Engine;
import Karm.Diag;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Css::Tests {

static String _serializeFunc(Str input) {
    Lexer lex{input};
    auto diags = Diag::Collector::ignore();
    return serialize(consumeFunc(lex, diags));
}

static String _serializeValue(Str input) {
    Lexer lex{input};
    auto diags = Diag::Collector::ignore();
    auto [value, _] = consumeDeclarationValue(lex, diags);
    return serialize(value);
}

static String _serializeDeclarations(Str input) {
    Lexer lex{input};
    auto diags = Diag::Collector::ignore();
    return serialize(consumeDeclarationList(lex, diags));
}

static String _serializeRules(Str input) {
    Lexer lex{input};
    auto diags = Diag::Collector::ignore();
    return serialize(consumeRuleList(lex, true, diags));
}

test$("vaev-css-serialize-tokens") {
    assertEq$(_serializeFunc("f(foo 12 1.5em 50% #id @media url(a.png) + , : ; <!-- -->)"), "f(foo 12 1.5em 50% #id @media url(a.png) + , : ; <!-- -->)"s);
    return Ok();
}

test$("vaev-css-serialize-string") {
    assertEq$(_serializeFunc("f(\"a\")"), "f(\"a\")"s);
    assertEq$(_serializeFunc("f('a')"), "f(\"a\")"s);
    assertEq$(_serializeFunc("f('a\"b')"), "f(\"a\\\"b\")"s);
    assertEq$(_serializeFunc("f(\"a\\\\b\")"), "f(\"a\\\\b\")"s);
    assertEq$(_serializeFunc("f(\"a\tb\")"), "f(\"a\\9 b\")"s);
    assertEq$(_serializeFunc("f(\"a\x7f"
                             "b\")"),
              "f(\"a\\7f b\")"s);
    assertEq$(_serializeFunc(Str{"f(\"a\0b\")", 8}), "f(\"a\uFFFDb\")"s);
    return Ok();
}

test$("vaev-css-serialize-bad-string") {
    assertEq$(_serializeFunc("f(\"a\n)"), "f(\"a\n\n)"s);
    assertEq$(_serializeValue("\"a\n b"), "\"a\nb"s);
    assertEq$(_serializeValue("\"a\n"), "\"a\n"s);
    return Ok();
}

test$("vaev-css-serialize-backslash-delim") {
    assertEq$(_serializeValue("a \\\n b"), "a\\\nb"s);
    assertEq$(_serializeValue("a \\\n"), "a\\\n"s);
    return Ok();
}

test$("vaev-css-serialize-blocks") {
    assertEq$(_serializeFunc("f((a) [b] {c})"), "f((a) [b] {c})"s);
    return Ok();
}

test$("vaev-css-serialize-nested-functions") {
    assertEq$(_serializeFunc("calc(1px + var(--x, 2px))"), "calc(1px + var(--x, 2px))"s);
    return Ok();
}

test$("vaev-css-serialize-comment-needed") {
    Str pairs[][2] = {
        {"foo", "bar"},
        {"foo", "bar()"},
        {"foo", "url(bar)"},
        {"foo", "-"},
        {"foo", "123"},
        {"foo", "123%"},
        {"foo", "123em"},
        {"foo", "-->"},
        {"foo", "()"},

        {"@foo", "bar"},
        {"@foo", "bar()"},
        {"@foo", "url(bar)"},
        {"@foo", "-"},
        {"@foo", "123"},
        {"@foo", "123%"},
        {"@foo", "123em"},
        {"@foo", "-->"},

        {"#foo", "bar"},
        {"#foo", "bar()"},
        {"#foo", "url(bar)"},
        {"#foo", "-"},
        {"#foo", "123"},
        {"#foo", "123%"},
        {"#foo", "123em"},
        {"#foo", "-->"},

        {"123foo", "bar"},
        {"123foo", "bar()"},
        {"123foo", "url(bar)"},
        {"123foo", "-"},
        {"123foo", "123"},
        {"123foo", "123%"},
        {"123foo", "123em"},
        {"123foo", "-->"},

        {"#", "bar"},
        {"#", "bar()"},
        {"#", "url(bar)"},
        {"#", "-"},
        {"#", "123"},
        {"#", "123%"},
        {"#", "123em"},

        {"-", "bar"},
        {"-", "bar()"},
        {"-", "url(bar)"},
        {"-", "-"},
        {"-", "123"},
        {"-", "123%"},
        {"-", "123em"},

        {"123", "bar"},
        {"123", "bar()"},
        {"123", "url(bar)"},
        {"123", "123"},
        {"123", "123%"},
        {"123", "123em"},
        {"123", "%"},

        {"@", "bar"},
        {"@", "bar()"},
        {"@", "url(bar)"},
        {"@", "-"},

        {".", "123"},
        {".", "123%"},
        {".", "123em"},

        {"+", "123"},
        {"+", "123%"},
        {"+", "123em"},

        {"/", "*"},
    };

    for (auto const& [t1, t2] : pairs)
        assertEq$(_serializeValue(Io::format("{} {}", t1, t2)), Io::format("{}/**/{}", t1, t2));

    return Ok();
}

test$("vaev-css-serialize-comment-not-needed") {
    assertEq$(_serializeValue("a , b"), "a,b"s);
    assertEq$(_serializeValue("a *"), "a*"s);
    assertEq$(_serializeValue("1 -"), "1-"s);
    assertEq$(_serializeValue(". a"), ".a"s);
    assertEq$(_serializeValue("+ a"), "+a"s);
    assertEq$(_serializeValue("@ 1"), "@1"s);
    assertEq$(_serializeValue("1 (b)"), "1(b)"s);
    return Ok();
}

test$("vaev-css-serialize-declarations") {
    assertEq$(_serializeDeclarations("color: red"), "color: red;"s);
    assertEq$(_serializeDeclarations("color: red !important"), "color: red !important;"s);
    assertEq$(_serializeDeclarations("color: red; margin: 0"), "color: red; margin: 0;"s);
    return Ok();
}

test$("vaev-css-serialize-qualified-rule") {
    assertEq$(_serializeRules(".a { color: red; margin: 0 }"), ".a { color: red; margin: 0; }"s);
    assertEq$(_serializeRules(".a {}"), ".a { }"s);
    assertEq$(_serializeRules(".a{color:red}"), ".a { color: red; }"s);
    assertEq$(_serializeRules(".a { color: red; .b { margin: 0 } padding: 0 }"), ".a {\n  color: red;\n  .b { margin: 0; }\n  padding: 0;\n}"s);
    return Ok();
}

test$("vaev-css-serialize-at-rule") {
    assertEq$(_serializeRules("@import url(a.css);"), "@import url(a.css);"s);
    assertEq$(_serializeRules("@import url(a.css) ;"), "@import url(a.css);"s);
    assertEq$(_serializeRules("@media screen { .a { color: red } .b { margin: 0 } }"), "@media screen {\n  .a { color: red; }\n  .b { margin: 0; }\n}"s);
    assertEq$(_serializeRules("@media screen {}"), "@media screen { }"s);
    return Ok();
}

test$("vaev-css-serialize-rules") {
    assertEq$(_serializeRules(".a {} .b {}"), ".a { }\n.b { }"s);
    return Ok();
}

static String _parseRules(Str input) {
    Lexer lex{input};
    auto diags = Diag::Collector::ignore();
    return Io::format("{}", Sst{consumeRuleList(lex, true, diags)});
}

test$("vaev-css-serialize-round-trip") {
    Str inputs[] = {
        "@import url(a.css);"
        "@media screen and (min-width: 100px) { .a > b:hover { color: red !important; margin: 1px 2px } }"
        "@supports (display: grid) and (not (color: foo)) { #c::before { content: \"x\\\"y\"; } }"
        ".d { width: calc(100% - var(--gap, 2px)); background: url(img.png) no-repeat }",
        ".a {color:red}@import url(a.css);@media screen {.b {margin:1px 2px}}",
        ".a { content: \"x\ny; color: red }",
        ".a { content: \"x\\\"y\n; color: red }",
    };

    for (auto input : inputs) {
        assertEq$(_parseRules(_serializeRules(input)), _parseRules(input));
        assertEq$(_serializeRules(_serializeRules(input)), _serializeRules(input));
    }

    return Ok();
}

} // namespace Vaev::Css::Tests
