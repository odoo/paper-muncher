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

export Slice<Css::Sst> trimTrailingWhitespace(Slice<Css::Sst> value) {
    while (not isEmpty(value) and last(value) == Css::Token::WHITESPACE)
        value = sub(value, 0, value.len() - 1);
    return value;
}

static Slice<Css::Sst> _trimWhitespace(Slice<Css::Sst> value) {
    while (not isEmpty(value) and first(value) == Css::Token::WHITESPACE)
        value = next(value);
    while (not isEmpty(value) and last(value) == Css::Token::WHITESPACE)
        value = sub(value, 0, value.len() - 1);
    return value;
}

static bool _anyNodeRecursive(Slice<Css::Sst> nodes, auto pred) {
    return iter(nodes) | Any([&](Css::Sst const& node) {
               return pred(node) or _anyNodeRecursive(node.content, pred);
           });
}

// https://www.w3.org/TR/css-syntax-3/#typedef-any-value
export bool validAnyValue(Slice<Css::Sst> value) {
    return not isEmpty(value) and not _anyNodeRecursive(value, [](Css::Sst const& node) {
        return node == Css::Token::BAD_STRING or node == Css::Token::BAD_URL or
               node == Css::Token::RIGHT_PARENTHESIS or node == Css::Token::RIGHT_SQUARE_BRACKET or
               node == Css::Token::RIGHT_CURLY_BRACKET;
    });
}

// https://www.w3.org/TR/css-syntax-3/#typedef-declaration-value
export bool validDeclarationValue(Slice<Css::Sst> value) {
    return not contains(value, Css::Token::SEMICOLON) and
           not contains(value, Css::Token::delim("!")) and
           validAnyValue(value);
}

// https://drafts.csswg.org/css-variables-2/#typedef-var-args
static bool _validVarArgs(Slice<Css::Sst> args) {
    auto comma = indexOf(args, Css::Token::COMMA);

    // https://drafts.csswg.org/css-values-5/#component-function-commas
    auto name = _trimWhitespace(sub(args, 0, comma.unwrapOr(args.len())));
    if (name.len() == 1 and first(name).curlyBlock())
        name = _trimWhitespace(first(name).content);
    else if (Css::containsCurlyBlock(name))
        return false;

    if (not validDeclarationValue(name))
        return false;

    if (not comma)
        return true;

    // https://drafts.csswg.org/css-values-5/#non-strict-free-form-production
    auto fallback = _trimWhitespace(next(args, *comma + 1));
    if (not isEmpty(fallback) and first(fallback).curlyBlock()) {
        if (fallback.len() > 1)
            return false;
        fallback = first(fallback).content;
    }

    return isEmpty(fallback) or validDeclarationValue(fallback);
}

export bool containsVar(Slice<Css::Sst> value) {
    return _anyNodeRecursive(value, [](Css::Sst const& node) {
        return node.var();
    });
}

export bool containsInvalidVar(Slice<Css::Sst> value) {
    return _anyNodeRecursive(value, [](Css::Sst const& node) {
        return node.var() and not _validVarArgs(node.content);
    });
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

} // namespace Vaev
