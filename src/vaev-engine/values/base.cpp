export module Vaev.Engine:values.base;

import Karm.Core;
import :css;

using namespace Karm;

namespace Vaev {

export void eatWhitespace(Cursor<Css::Sst>& c) {
    while (not c.ended() and c.peek() == Css::Token::WHITESPACE)
        c.next();
}

// https://www.w3.org/TR/css-values-4/#comb-comma
export bool skipOmmitableComma(Cursor<Css::Sst>& c) {
    eatWhitespace(c);
    bool res = c.skip(Css::Token::COMMA);
    eatWhitespace(c);
    return res;
}

static bool _anyNode(Slice<Css::Sst> nodes, auto const& pred) {
    for (auto const& node : nodes)
        if (pred(node) or _anyNode(node.content, pred))
            return true;
    return false;
}

static bool _isBadToken(Css::Sst const& node) {
    return node == Css::Token::BAD_STRING or node == Css::Token::BAD_URL or
           node == Css::Token::RIGHT_PARENTHESIS or node == Css::Token::RIGHT_SQUARE_BRACKET or
           node == Css::Token::RIGHT_CURLY_BRACKET;
}

// https://drafts.csswg.org/css-syntax-3/#typedef-any-value
export bool isValidAnyValue(Slice<Css::Sst> value) {
    return not _anyNode(value, _isBadToken);
}

// https://drafts.csswg.org/css-syntax-3/#typedef-declaration-value
export bool isValidDeclarationValue(Slice<Css::Sst> value) {
    return not contains(value, Css::Token::SEMICOLON) and
           not contains(value, Css::Token::delim("!")) and
           isValidAnyValue(value);
}

static bool _isVar(Css::Sst const& node) {
    return node == Css::Sst::FUNC and node.prefix == Css::Token::function("var(");
}

// https://drafts.csswg.org/css-variables-2/#typedef-var-args
static bool _isValidVar(Css::Sst const& var) {
    Slice<Css::Sst> args = var.content;
    auto comma = indexOf(args, Css::Token::COMMA);

    auto name = Css::trimTrailingWhitespace(sub(args, 0, comma.unwrapOr(args.len())));
    if (isEmpty(name) or not isValidDeclarationValue(name))
        return false;

    return not comma or isValidDeclarationValue(next(args, *comma + 1));
}

static bool _isInvalidVar(Css::Sst const& node) {
    return _isVar(node) and not _isValidVar(node);
}

export bool containsVar(Slice<Css::Sst> value) {
    return _anyNode(value, _isVar);
}

export bool containsInvalidVar(Slice<Css::Sst> value) {
    return _anyNode(value, _isInvalidVar);
}

export template <typename T>
struct ValueParser;

export template <typename T>
concept ValueParseable = requires() {
    ValueParser<T>::parse;
};

export template <typename T>
Res<T> parseValue(Cursor<Css::Sst>& c) {
    return ValueParser<T>::parse(c);
}

export template <typename T>
Res<T> parseValue(Str str) {
    Css::Lexer lex{str};
    auto diags = Diag::Collector::ignore();
    auto [sst, _] = Css::consumeDeclarationValue(lex, diags);
    Cursor<Css::Sst> content{sst};
    return ValueParser<T>::parse(content);
}

} // namespace Vaev
