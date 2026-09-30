module;

#include <karm/macros>

export module Vaev.Engine:style.supports;

import Karm.Core;

import :values;
import :css;

using namespace Karm;

namespace Vaev::Style {

static Res<bool> _parseSupportsInParens(Cursor<Css::Sst>& c);

// https://drafts.csswg.org/css-conditional-3/#typedef-supports-condition
static Res<bool> _parseSupportsCondition(Cursor<Css::Sst>& c) {
    eatWhitespace(c);

    if (c.skip(Css::Token::ident("not"))) {
        eatWhitespace(c);
        return Ok(not try$(_parseSupportsInParens(c)));
    }

    bool value = try$(_parseSupportsInParens(c));
    eatWhitespace(c);

    if (c.ended() or (c.peek() != Css::Token::ident("and") and c.peek() != Css::Token::ident("or")))
        return Ok(value);

    auto keyword = c->token;
    while (c.skip(keyword)) {
        eatWhitespace(c);
        bool rhs = try$(_parseSupportsInParens(c));
        value = keyword == Css::Token::ident("and") ? (value and rhs) : (value or rhs);
        eatWhitespace(c);
    }

    return Ok(value);
}

// https://drafts.csswg.org/css-conditional-3/#typedef-supports-in-parens
static Res<bool> _parseSupportsInParens(Cursor<Css::Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of supports condition");

    if (c.skip(Css::Sst::FUNC))
        return Ok(false);

    if (c.peek() != Css::Sst::BLOCK or c->token != Css::Token::LEFT_PARENTHESIS)
        return Error::invalidData("expected '(' in supports condition");

    Cursor<Css::Sst> content = c.next().content;

    // ( <supports-condition> )
    Cursor<Css::Sst> condition = content;
    if (auto value = _parseSupportsCondition(condition)) {
        eatWhitespace(condition);
        if (condition.ended())
            return value;
    }

    // ( <declaration> )
    Cursor<Css::Sst> declaration = content;
    eatWhitespace(declaration);
    if (declaration.skip(Css::Token::IDENT)) {
        eatWhitespace(declaration);
        if (declaration.skip(Css::Token::COLON))
            // TODO: Evaluate the declaration against the property registry
            return Ok(true);
    }

    return Ok(false);
}

// https://drafts.csswg.org/css-conditional-3/#at-supports
export bool parseSupportsCondition(Cursor<Css::Sst>& c) {
    auto result = _parseSupportsCondition(c);
    eatWhitespace(c);
    return c.ended() and result.unwrapOr(false);
}

} // namespace Vaev::Style
