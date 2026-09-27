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

} // namespace Vaev::Css::Tests
