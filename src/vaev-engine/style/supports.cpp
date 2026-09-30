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

// https://drafts.csswg.org/css-syntax-3/#consume-declaration
static Slice<Css::Sst> _stripImportant(Slice<Css::Sst> value) {
    if (isEmpty(value) or last(value) != Css::Token::ident("important"))
        return value;

    auto rest = trimTrailingWhitespace(sub(value, 0, value.len() - 1));
    if (isEmpty(rest) or last(rest) != Css::Token::delim("!"))
        return value;

    return trimTrailingWhitespace(sub(rest, 0, rest.len() - 1));
}

static bool _containsMixedCurlyBlock(Slice<Css::Sst> value) {
    usize count = 0;
    bool curly = false;
    for (auto const& node : value) {
        if (node == Css::Token::WHITESPACE)
            continue;
        count++;
        curly = curly or (node == Css::Sst::BLOCK and node.token == Css::Token::LEFT_CURLY_BRACKET);
    }
    return curly and count > 1;
}

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

    auto value = _stripImportant(trimTrailingWhitespace(c.next(c.rem())));
    if (not isValidDeclarationValue(value) or containsInvalidVar(value))
        return false;

    // "--" alone is not a valid custom property.
    if (startWith(propertyName, "--"s) == Match::PARTIAL)
        return true;

    if (_containsMixedCurlyBlock(value))
        return false;

    // https://drafts.csswg.org/css-values-5/#resolve-property
    if (containsVar(value))
        return true;

    auto prop = registry.parseValue(Symbol::from(propertyName), value, RegisteredPropertySet::ALLOW_DEFAULTING);
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

    if (c.peek() != Css::Sst::FUNC and (c.peek() != Css::Sst::BLOCK or c->token != Css::Token::LEFT_PARENTHESIS))
        return Error::invalidData("expected '(' in supports condition");

    auto const& node = c.next();
    if (not isValidAnyValue(node.content))
        return Error::invalidData("invalid token in supports condition");

    if (node == Css::Sst::FUNC)
        return Ok(false);

    Cursor<Css::Sst> condition = node.content;
    if (auto value = _parseSupportsCondition(registry, condition)) {
        eatWhitespace(condition);
        if (condition.ended())
            return value;
    }

    return Ok(_evalSupportsDeclaration(registry, node.content));
}

// https://drafts.csswg.org/css-conditional-3/#at-supports
export bool parseSupportsCondition(RegisteredPropertySet& registry, Cursor<Css::Sst>& c) {
    auto result = _parseSupportsCondition(registry, c);
    eatWhitespace(c);
    return c.ended() and result.unwrapOr(false);
}

} // namespace Vaev::Style
