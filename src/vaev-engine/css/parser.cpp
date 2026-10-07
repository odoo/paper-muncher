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
Sst consumeComponentValue(Lexer& lex, Diag::Collector& diags);
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

export Tuple<Content, bool> consumeDeclarationValue(Lexer& lex, Diag::Collector diags) {
    Content value;

    // 3. While the next input token is a <whitespace-token>, consume the next input token.
    eatWhitespace(lex);

    // 4. As long as the next input token is anything other than an <EOF-token>,
    //    consume a component value and append it to the declaration’s value.
    while (not endedDeclarationValue(lex)) {
        value.pushBack(consumeComponentValue(lex, diags));
        eatWhitespace(lex);
    }

    // 5. If the last two non-<whitespace-token>s in the declaration’s
    //    value are a <delim-token> with the value "!" followed by an
    //    <ident-token> with a value that is an ASCII case-insensitive match
    //    for "important", remove them from the declaration’s value
    //    and set the declaration’s important flag to true.
    auto important = _stripImportant(value);
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
    Sst decl{Sst::DECL};
    decl.token = lex.next();

    // 1. While the next input token is a <whitespace-token>, consume the next input token.
    eatWhitespace(lex);

    // 2. If the next input token is anything other than a <colon-token>, this is a parse error. Return nothing.
    auto t = lex.peek();
    if (t != Token::COLON) {
        diags.emit(
            Diag::Diagnostic::error("expected colon"s)
                .withPrimaryLabel(t.span, "here"s)
        );
        return NONE;
    }

    // Otherwise, consume the next input token.
    lex.next();

    // Parse the declaration’s value.
    auto [content, important] = consumeDeclarationValue(lex, diags);
    decl.content = std::move(content);
    decl.flags.set(Sst::IMPORTANT, important);

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
    lex.next();

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
