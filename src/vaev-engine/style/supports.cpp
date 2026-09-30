module;

#include <karm/macros>

export module Vaev.Engine:style.supports;

import Karm.Core;

import :values;
import :css;
import :props;

using namespace Karm;

namespace Vaev::Style {

// MARK: Declarations ----------------------------------------------------------

// https://drafts.csswg.org/css-conditional-3/#typedef-supports-decl
static bool _evalSupportsDeclaration(RegisteredPropertySet& registry, Cursor<Css::Sst> c) {
    eatWhitespace(c);
    if (c.ended() or *c != Css::Token::IDENT)
        return false;
    auto const& propertyName = c.next().token.data;

    eatWhitespace(c);
    if (not c.skip(Css::Token::COLON))
        return false;
    eatWhitespace(c);

    // "--" alone is not a valid custom property.
    if (startWith(propertyName, "--"s) == Match::PARTIAL)
        return true;

    auto prop = registry.parseValue(Symbol::from(propertyName), c, RegisteredPropertySet::ALLOW_DEFAULTING);
    return prop.has();
}

// MARK: Conditions ------------------------------------------------------------

static Res<bool> _parseSupportsInParens(RegisteredPropertySet& registry, Cursor<Css::Sst>& c);

// https://drafts.csswg.org/css-conditional-3/#typedef-supports-condition
static Res<bool> _parseSupportsCondition(RegisteredPropertySet& registry, Cursor<Css::Sst>& c) {
    eatWhitespace(c);

    if (c.skip(Css::Token::ident("not"))) {
        eatWhitespace(c);
        return Ok(not try$(_parseSupportsInParens(registry, c)));
    }

    bool value = try$(_parseSupportsInParens(registry, c));
    eatWhitespace(c);

    if (c.ended() or (c.peek() != Css::Token::ident("and") and c.peek() != Css::Token::ident("or")))
        return Ok(value);

    auto keyword = c->token;
    while (c.skip(keyword)) {
        eatWhitespace(c);
        bool rhs = try$(_parseSupportsInParens(registry, c));
        value = keyword == Css::Token::ident("and") ? (value and rhs) : (value or rhs);
        eatWhitespace(c);
    }

    return Ok(value);
}

// https://drafts.csswg.org/css-conditional-3/#typedef-supports-in-parens
static Res<bool> _parseSupportsInParens(RegisteredPropertySet& registry, Cursor<Css::Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of supports condition");

    if (c.skip(Css::Sst::FUNC))
        return Ok(false);

    if (c.peek() != Css::Sst::BLOCK or c->token != Css::Token::LEFT_PARENTHESIS)
        return Error::invalidData("expected '(' in supports condition");

    Cursor<Css::Sst> content = c.next().content;

    Cursor<Css::Sst> condition = content;
    if (auto value = _parseSupportsCondition(registry, condition)) {
        eatWhitespace(condition);
        if (condition.ended())
            return value;
    }

    return Ok(_evalSupportsDeclaration(registry, content));
}

// https://drafts.csswg.org/css-conditional-3/#at-supports
export bool parseSupportsCondition(RegisteredPropertySet& registry, Cursor<Css::Sst>& c) {
    auto result = _parseSupportsCondition(registry, c);
    eatWhitespace(c);
    return c.ended() and result.unwrapOr(false);
}

} // namespace Vaev::Style
