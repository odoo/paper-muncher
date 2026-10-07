export module Vaev.Engine:css.parser;

import Karm.Logger;
import Karm.Diag;

import :css.lexer;

namespace Vaev::Css {

// MARK: Sst -------------------------------------------------------------------

// The SST (Skeleton Syntax Tree) is an intermediary representation of the CSS used to build the real syntaxic tree
// We have all the block and declarations here but didn't interpreted it because we lacked context in the previous parse step
// when we have this representation we can parse it a last time and interpret the different blocks and functions to build the CSSOM
// The name come from: https://people.csail.mit.edu/jrb/Projects/dexprs.pdf chapter 3.4

export struct Sst;

export using Content = Vec<Sst>;

#define FOREACH_SST(SST) \
    SST(RULE)            \
    SST(FUNC)            \
    SST(DECL)            \
    SST(LIST)            \
    SST(TOKEN)           \
    SST(BLOCK)

export struct Sst {
    enum struct Type {
#define ITER(NAME) NAME,
        FOREACH_SST(ITER)
#undef ITER

            _LEN,
    };
    using enum Type;

    enum struct Options : u8 {
        IMPORTANT = 1 << 0,
        WITH_BLOCK = 1 << 1,
    };
    using enum Options;

    Type type;
    // Contains the token if type is TOKEN or the @rule name
    Token token = Token(Token::NIL);
    Opt<Box<Sst>> prefix{};
    Content content{};
    Flags<Options> flags = {};

    Sst(Type type) : type(type) {}

    Sst(Token token) : type(TOKEN), token(token) {}

    Sst(Content content) : type(LIST), content(content) {}

    bool statement() const {
        return type == DECL or type == RULE;
    }

    bool curlyBlock() const {
        return type == BLOCK and token == Token::LEFT_CURLY_BRACKET;
    }

    // https://drafts.csswg.org/css-variables-2/#guaranteed-invalid
    static Sst guaranteedInvalid() {
        return Token::guaranteedInvalid("");
    }

    void repr(Io::Emit& e) const {
        if (type == TOKEN) {
            e("{}", token);
            return;
        }

        e("({} ", type);
        if (token)
            e("token={}", token);
        e.indent();

        if (prefix) {
            e.newline();
            e("prefix=");
            (*prefix)->repr(e);
        }

        if (content) {
            e.newline();
            e("content=[");
            e.indentNewline();
            for (auto& child : content) {
                child.repr(e);
                e.newline();
            }
            e.deindent();
            e("]");
            e.newline();
        }
        e.deindent();
        e(")");
    }

    bool operator==(Type type) const {
        return this->type == type;
    }

    bool operator==(Token::Type const& type) const {
        return this->type == TOKEN and
               token.type == type;
    }

    bool operator==(Token const& other) const {
        return type == TOKEN and
               token == other;
    }
};

// MARK: Parser ----------------------------------------------------------------

Sst consumeAtRule(Lexer& lex, Diag::Collector& diags);
export Content consumeDeclarationList(Lexer& lex, Diag::Collector& diags, bool topLevel = true);
Content consumeDeclarationBlock(Lexer& lex, Diag::Collector& diags);
export Sst consumeComponentValue(Lexer& lex, Diag::Collector& diags);
Sst consumeBlock(Lexer& lex, Diag::Collector& diags, Token::Type endingToken);
export Opt<Sst> consumeDeclaration(Lexer& lex, Diag::Collector& diags);

// https://www.w3.org/TR/css-syntax-3/#consume-qualified-rule
Opt<Sst> consumeRule(Lexer& lex, Diag::Collector& diags) {
    Sst rule{Sst::RULE};
    Content prefix;

    while (true) {
        auto t = lex.peek();
        switch (t.type) {
        case Token::END_OF_FILE:
            diags.emit(
                Diag::Diagnostic::error("unexpected end of file"s)
                    .withPrimaryLabel(t.span, "here"s)
            );
            return NONE;

        case Token::LEFT_CURLY_BRACKET: {
            rule.prefix = Some(std::move(prefix));
            rule.content = consumeDeclarationBlock(lex, diags);
            rule.flags.set(Sst::WITH_BLOCK);
            return Some(rule);
        }

        default:
            prefix.pushBack(consumeComponentValue(lex, diags));
            break;
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-list-of-rules
export Content consumeRuleList(Lexer& lex, bool topLevel, Diag::Collector& diags) {
    Content list{};

    while (true) {
        switch (lex.peek().type) {
        case Token::WHITESPACE:
            lex.next();
            break;

        case Token::END_OF_FILE:
            lex.next();
            return list;

        case Token::CDC:
        case Token::CDO: {
            if (topLevel) {
                lex.next();
            } else {
                auto rule = consumeRule(lex, diags);
                if (rule)
                    list.pushBack(*rule);
            }
            break;
        }

        case Token::AT_KEYWORD: {
            list.pushBack(consumeAtRule(lex, diags));
            break;
        }

        default: {
            auto rule = consumeRule(lex, diags);
            if (rule)
                list.pushBack(*rule);
            break;
        }
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-at-rule
Sst consumeAtRule(Lexer& lex, Diag::Collector& diags) {
    Sst atRule{Sst::RULE};
    atRule.token = lex.next();
    Content prefix;

    while (true) {
        auto t = lex.peek();
        switch (t.type) {
        case Token::SEMICOLON:
            lex.next();
            atRule.prefix = Some(std::move(prefix));
            return atRule;

        case Token::END_OF_FILE:
            diags.emit(
                Diag::Diagnostic::error("unexpected end of file"s)
                    .withPrimaryLabel(t.span, "here"s)
            );

            lex.next();
            atRule.prefix = Some(std::move(prefix));
            return atRule;

        case Token::LEFT_CURLY_BRACKET:
            atRule.prefix = Some(std::move(prefix));
            atRule.content = consumeDeclarationBlock(lex, diags);
            atRule.flags.set(Sst::WITH_BLOCK);
            return atRule;

        default:
            prefix.pushBack(consumeComponentValue(lex, diags));
            break;
        }
    }
}

static bool _stripImportant(Content& value) {
    if (value.len() < 2 or
        last(value) != Token::ident("important") or
        value[value.len() - 2] != Token::delim("!"))
        return false;

    value.trunc(value.len() - 2);
    return true;
}

export bool endedDeclarationValue(Lexer& lex) {
    return lex.peek() == Token::END_OF_FILE or
           lex.peek() == Token::SEMICOLON or
           lex.peek() == Token::RIGHT_CURLY_BRACKET;
}

// https://www.w3.org/TR/css-syntax-3/#consume-declaration
export Tuple<Content, bool> consumeDeclarationValue(Lexer& lex, Diag::Collector diags) {
    Content value;

    // 4. Discard whitespace from input.
    eatWhitespace(lex);

    // 5. Consume a list of component values from input, with nested, and with <semicolon-token> as the stop token, and
    //    set decl’s value to the result.
    // NOSPEC: The whitespace between component values is dropped.
    while (not endedDeclarationValue(lex)) {
        value.pushBack(consumeComponentValue(lex, diags));
        eatWhitespace(lex);
    }

    // 6. If the last two non-<whitespace-token>s in decl’s value are a <delim-token> with the value "!" followed by an
    //    <ident-token> with a value that is an ASCII case-insensitive match for "important", remove them from decl’s
    //    value and set decl’s important flag.
    auto important = _stripImportant(value);

    // 7. While the last item in decl’s value is a <whitespace-token>, remove that token.
    // NOTE: The whitespace was already dropped in step 5.
    return {std::move(value), important};
}

static void _skipComponentValue(Lexer& lex);
static void _skipBlock(Lexer& lex, Token::Type endingToken);

// https://www.w3.org/TR/css-syntax-3/#consume-function
static void _skipFunc(Lexer& lex) {
    lex.next();
    while (true) {
        auto t = lex.peek();
        switch (t.type) {
        case Token::END_OF_FILE:
            return;

        case Token::RIGHT_PARENTHESIS:
            lex.next();
            return;

        default:
            _skipComponentValue(lex);
            break;
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-component-value
static void _skipComponentValue(Lexer& lex) {
    switch (lex.peek().type) {
    case Token::LEFT_SQUARE_BRACKET:
        _skipBlock(lex, Token::RIGHT_SQUARE_BRACKET);
        return;

    case Token::LEFT_CURLY_BRACKET:
        _skipBlock(lex, Token::RIGHT_CURLY_BRACKET);
        return;

    case Token::LEFT_PARENTHESIS:
        _skipBlock(lex, Token::RIGHT_PARENTHESIS);
        return;

    case Token::FUNCTION:
        _skipFunc(lex);
        return;

    default:
        lex.next();
        return;
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-a-simple-block
static void _skipBlock(Lexer& lex, Token::Type endingToken) {
    lex.next();

    while (true) {
        auto t = lex.peek();

        switch (t.type) {
        case Token::END_OF_FILE:
            return;

        default:
            if (t.type == endingToken) {
                lex.next();
                return;
            }
            _skipComponentValue(lex);
            break;
        }
    }
}

export bool containsCurlyBlock(Slice<Sst> value) {
    return iter(value) | Any([](Sst const& node) {
               return node.curlyBlock();
           });
}

static bool _declarationAhead(Lexer lex) {
    auto name = lex.next();
    if (name != Token::IDENT)
        return false;

    eatWhitespace(lex);
    if (lex.peek() != Token::COLON)
        return false;
    lex.next();

    if (startWith(name.data.str(), "--"s) != Match::NO)
        return true;

    // https://drafts.csswg.org/css-syntax-3/#consume-declaration
    while (not endedDeclarationValue(lex)) {
        auto t = lex.peek().type;
        if (t == Token::LEFT_CURLY_BRACKET)
            return false;
        _skipComponentValue(lex);
    }

    return true;
}

// https://www.w3.org/TR/css-syntax-3/#consume-style-block
// https://www.w3.org/TR/css-syntax-3/#consume-list-of-declarations
// NOSPEC: We unified the two functions into one for simplicity
//         since they mostly do the same thing.
Content consumeDeclarationList(Lexer& lex, Diag::Collector& diags, bool topLevel) {
    Content block;

    // Repeatedly consume the next input token:
    while (true) {
        auto t = lex.peek();
        switch (t.type) {
        case Token::WHITESPACE:
        case Token::SEMICOLON:
            // Do nothing.
            lex.next();
            break;

        case Token::END_OF_FILE:
            if (not topLevel) {
                diags.emit(
                    Diag::Diagnostic::error("unexpected end of file"s)
                        .withPrimaryLabel(t.span, "here"s)
                );
            }
            lex.next();
            return block;

        case Token::AT_KEYWORD:
            block.pushBack(consumeAtRule(lex, diags));
            break;

        case Token::IDENT:
            // NOSPEC: Normally the spec would requires us to consume component values
            //         and then parse the content of an rule in a second pass,
            //         but we are "smarter" than this and try to consume nested rule directly.
            if (lex.peek().data == "&" or not _declarationAhead(lex)) {
                auto rule = consumeRule(lex, diags);
                if (rule)
                    block.pushBack(*rule);
            } else {
                auto decl = consumeDeclaration(lex, diags);
                if (decl)
                    block.pushBack(*decl);
            }
            break;

        case Token::RIGHT_CURLY_BRACKET:
            return block;

        default:
            auto rule = consumeRule(lex, diags);
            if (rule)
                block.pushBack(*rule);
            break;
        }
    }
}

Content consumeDeclarationBlock(Lexer& lex, Diag::Collector& diags) {
    auto opening = lex.next(); // consume left curly bracket
    auto res = consumeDeclarationList(lex, diags);
    auto t = lex.peek();
    if (t != Token::RIGHT_CURLY_BRACKET) {
        diags.emit(
            Diag::Diagnostic::error("expected '}' at the end of declaration block"s)
                .withSecondaryLabel(opening.span, "to match this '{'")
        );
    } else
        lex.next(); // consume right curly bracket
    return res;
}

// https://www.w3.org/TR/css-syntax-3/#consume-declaration
export Opt<Sst> consumeDeclaration(Lexer& lex, Diag::Collector& diags) {
    // Let decl be a new declaration, with an initially empty name and a value set to an empty list.
    Sst decl{Sst::DECL};

    // 1. If the next token is an <ident-token>, consume a token from input and set decl's name to the token’s value.
    //    Otherwise, consume the remnants of a bad declaration from input, with nested, and return nothing.
    // NOSPEC: The remnants of the bad declaration are left to the caller.
    if (lex.peek() != Token::IDENT) {
        diags.emit(
            Diag::Diagnostic::error("expected identifier"s)
                .withPrimaryLabel(lex.peek().span, "here"s)
        );
        return NONE;
    }
    decl.token = lex.next();

    // 2. Discard whitespace from input.
    eatWhitespace(lex);

    // 3. If the next token is a <colon-token>, discard a token from input.
    //    Otherwise, consume the remnants of a bad declaration from input, with nested, and return nothing.
    // NOSPEC: The remnants of the bad declaration are left to the caller.
    auto t = lex.peek();
    if (t != Token::COLON) {
        diags.emit(
            Diag::Diagnostic::error("expected colon"s)
                .withPrimaryLabel(t.span, "here"s)
        );
        return NONE;
    }
    lex.next();

    // NOTE: Steps 4 to 7 are done by consumeDeclarationValue().
    auto [content, important] = consumeDeclarationValue(lex, diags);
    decl.content = std::move(content);
    decl.flags.set(Sst::IMPORTANT, important);

    // 8. If decl’s name is a custom property name string, then set decl’s original text to the segment of the original
    //    source text string corresponding to the tokens of decl’s value.
    //    Otherwise, if decl’s value contains a top-level simple block with an associated token of <{-token>, and also
    //    contains any other non-<whitespace-token> value, return nothing.
    //    (That is, a top-level {}-block is only allowed as the entire value of a non-custom property.)
    //    Otherwise, if decl’s name is an ASCII case-insensitive match for "unicode-range", consume the value of a
    //    unicode-range descriptor from the segment of the original source text string corresponding to the tokens
    //    returned by the consume a list of component values call, and replace decl’s value with the result.
    // NOSPEC: The original text isn't kept and unicode-range isn't consumed again.
    if (startWith(decl.token.data.str(), "--"s) == Match::NO and
        decl.content.len() > 1 and
        containsCurlyBlock(decl.content))
        return NONE;

    // 9. If decl is valid in the current context, return it; otherwise return nothing.
    // NOTE: The validity is checked by the caller.
    return Some(decl);
}

// https://www.w3.org/TR/css-syntax-3/#consume-function
export Sst consumeFunc(Lexer& lex, Diag::Collector& diags) {
    Sst fn = Sst::FUNC;
    fn.prefix = Some(lex.next());

    while (true) {
        auto t = lex.peek();
        switch (t.type) {
        case Token::END_OF_FILE:
            diags.emit(
                Diag::Diagnostic::error("unexpected end of file"s)
                    .withPrimaryLabel(t.span, "here"s)
            );
            return fn;

        case Token::RIGHT_PARENTHESIS:
            lex.next();
            return fn;

        default:
            fn.content.pushBack(consumeComponentValue(lex, diags));
            break;
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-component-value
Sst consumeComponentValue(Lexer& lex, Diag::Collector& diags) {
    switch (lex.peek().type) {
    case Token::LEFT_SQUARE_BRACKET:
        return consumeBlock(lex, diags, Token::RIGHT_SQUARE_BRACKET);

    case Token::LEFT_CURLY_BRACKET:
        return consumeBlock(lex, diags, Token::RIGHT_CURLY_BRACKET);

    case Token::LEFT_PARENTHESIS:
        return consumeBlock(lex, diags, Token::RIGHT_PARENTHESIS);

    case Token::FUNCTION:
        return consumeFunc(lex, diags);

    default:
        return lex.next();
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-a-simple-block
Sst consumeBlock(Lexer& lex, Diag::Collector& diags, Token::Type endingToken) {
    Sst block = Sst::BLOCK;
    block.token = lex.next();

    while (true) {
        auto t = lex.peek();

        switch (t.type) {
        case Token::END_OF_FILE:
            diags.emit(
                Diag::Diagnostic::error("unexpected end of file"s)
                    .withPrimaryLabel(t.span, "here"s)
            );
            return block;

        default:
            if (t.type == endingToken) {
                lex.next();
                return block;
            }

            block.content.emplaceBack(consumeComponentValue(lex, diags));
            break;
        }
    }
}

// NOSPEC: specialized parser for selectors,
// it's not used in the normal workflow but for testing purposes and querySelectors
export Content consumeSelector(Lexer& lex, Diag::Collector& diags) {
    Content value;

    while (lex.peek() != Token::END_OF_FILE and
           lex.peek() != Token::SEMICOLON and
           lex.peek() != Token::RIGHT_CURLY_BRACKET) {
        value.pushBack(consumeComponentValue(lex, diags));
    }
    return value;
}

} // namespace Vaev::Css
