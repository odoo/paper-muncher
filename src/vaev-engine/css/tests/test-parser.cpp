#include <karm/test>

import Vaev.Engine;
import Karm.Diag;

using namespace Karm;

namespace Vaev::Css::Tests {

test$("vaev-css-parse-func") {
    Lexer lex{"func(1 2 3 4) not-consumed(4 3 2 1)"};
    auto diags = Diag::Collector::ignore();
    auto sst = consumeFunc(lex, diags);
    assert$(not lex.ended());

    assertEq$(sst, Sst::FUNC);

    // check the prefix
    assertEq$(sst.prefix, Sst::TOKEN);
    assertEq$(sst.prefix, Token::FUNCTION);

    // check the content
    assertEq$(sst.content.len(), 7uz);
    assertEq$(sst.content[0], Token::NUMBER);
    assertEq$(sst.content[1], Token::WHITESPACE);
    assertEq$(sst.content[2], Token::NUMBER);
    assertEq$(sst.content[3], Token::WHITESPACE);
    assertEq$(sst.content[4], Token::NUMBER);
    assertEq$(sst.content[5], Token::WHITESPACE);
    assertEq$(sst.content[6], Token::NUMBER);

    return Ok();
}

test$("vaev-css-parse-nested-rule-with-colon") {
    Lexer lex{"color: red; a:where(.x) { color: blue } .b { display: none }"};
    auto diags = Diag::Collector::ignore();
    auto content = consumeDeclarationList(lex, diags);

    assertEq$(content.len(), 3uz);
    assertEq$(content[0], Sst::DECL);
    assertEq$(content[1], Sst::RULE);
    assertEq$(content[2], Sst::RULE);

    return Ok();
}

test$("vaev-css-parse-rule-block") {
    Lexer lex{"@import url(a.css); @media screen {} .a {}"};
    auto diags = Diag::Collector::ignore();
    auto content = consumeRuleList(lex, true, diags);

    assertEq$(content.len(), 3uz);
    assert$(not content[0].flags.has(Sst::WITH_BLOCK));
    assert$(content[1].flags.has(Sst::WITH_BLOCK));
    assert$(content[2].flags.has(Sst::WITH_BLOCK));

    return Ok();
}

test$("vaev-css-parse-comments") {
    Lexer lex{"color /* x */ : red; margin: 1px /* x */ 2px; display: none /* x */"};
    auto diags = Diag::Collector::ignore();
    auto content = consumeDeclarationList(lex, diags);

    assertEq$(content.len(), 3uz);
    assertEq$(content[0].token, Token::ident("color"));
    assertEq$(content[1].content.len(), 2uz);
    assertEq$(content[2].content.len(), 1uz);

    return Ok();
}

test$("vaev-css-parse-important") {
    Lexer lex{"color: red ! important; display: none"};
    auto diags = Diag::Collector::ignore();
    auto content = consumeDeclarationList(lex, diags);

    assertEq$(content.len(), 2uz);
    assert$(content[0].flags.has(Sst::IMPORTANT));
    assertEq$(content[0].content.len(), 1uz);
    assert$(not content[1].flags.has(Sst::IMPORTANT));

    return Ok();
}

test$("vaev-css-parse-important-not-last") {
    Lexer lex{"color: red !important blue; display: none"};
    auto diags = Diag::Collector::ignore();
    auto content = consumeDeclarationList(lex, diags);

    assertEq$(content.len(), 2uz);
    assert$(not content[0].flags.has(Sst::IMPORTANT));
    assertEq$(content[0].content.len(), 4uz);
    assertEq$(content[1].token, Token::ident("display"));

    return Ok();
}

test$("vaev-css-parse-important-twice") {
    Lexer lex{"color: red !important !important; display: none"};
    auto diags = Diag::Collector::ignore();
    auto content = consumeDeclarationList(lex, diags);

    assertEq$(content.len(), 2uz);
    assert$(content[0].flags.has(Sst::IMPORTANT));
    assertEq$(content[0].content.len(), 3uz);
    assertEq$(content[1].token, Token::ident("display"));

    return Ok();
}

} // namespace Vaev::Css::Tests
