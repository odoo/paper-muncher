module;

#include <karm-base/box.h>
#include <karm-base/vec.h>
#include <karm-io/emit.h>
#include <karm-logger/logger.h>

export module Vaev.Style.Css:parser;

import :lexer;

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

struct Sst {
    enum struct Type {
#define ITER(NAME) NAME,
        FOREACH_SST(ITER)
#undef ITER

            _LEN,
    };
    using enum Type;

    Type type;
    // Contains the token if type is TOKEN or the @rule name
    Token token = Token(Token::NIL);
    Opt<Box<Sst>> prefix{};
    Content content{};

    Sst(Type type) : type(type) {}

    Sst(Token token) : type(TOKEN), token(token) {}

    Sst(Content content) : type(LIST), content(content) {}

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

export Sst consumeAtRule(Lexer& lex);
export Sst consumeBlock(Lexer& lex, Token::Type term);
export Sst consumeComponentValue(Lexer& lex);
export Sst consumeFunc(Lexer& lex);
export Content consumeDeclarationBlock(Lexer& lex);
export Content consumeDeclarationList(Lexer& lex, bool topLevel = false);
export Content consumeDeclarationValue(Lexer& lex);
export Content consumeRuleList(Lexer& lex, bool topLevel);
export Content consumeSelector(Lexer& lex);
export Opt<Sst> consumeDeclaration(Lexer& lex);
export Opt<Sst> consumeRule(Lexer& lex);
export bool declarationAhead(Lexer lex);

// https://www.w3.org/TR/css-syntax-3/#consume-declaration
void eatWhitespace(Lexer& lex) {
    while (lex.peek() == Token::WHITESPACE and not lex.ended())
        lex.next();
}

// https://www.w3.org/TR/css-syntax-3/#consume-at-rule
Sst consumeAtRule(Lexer& lex) {
    Sst atRule{Sst::RULE};
    atRule.token = lex.next();
    Content prefix;

    while (true) {
        switch (lex.peek().type) {
        case Token::SEMICOLON:
            lex.next();
            atRule.prefix = prefix;
            return atRule;

        case Token::END_OF_FILE:
            logError("unexpected end of file");

            lex.next();
            atRule.prefix = prefix;
            return atRule;

        case Token::LEFT_CURLY_BRACKET:
            atRule.prefix = std::move(prefix);
            atRule.content = consumeDeclarationBlock(lex);
            return atRule;

        default:
            prefix.pushBack(consumeComponentValue(lex));
            break;
        }
    }
}

Content consumeDeclarationBlock(Lexer& lex) {
    lex.next(); // consume left curly bracket
    auto res = consumeDeclarationList(lex);
    if (lex.peek() != Token::RIGHT_CURLY_BRACKET)
        logError("expected right curly bracket");
    else
        lex.next(); // consume right curly bracket
    return res;
}

// https://www.w3.org/TR/css-syntax-3/#consume-style-block
// https://www.w3.org/TR/css-syntax-3/#consume-list-of-declarations
// NOSPEC: We unified the two functions into one for simplicity
//         and added a check for the right curly bracket
//         to avoid aving to parsing the input multiple times
Content consumeDeclarationList(Lexer& lex, bool topLevel) {
    Content block;

    while (true) {
        switch (lex.peek().type) {
        case Token::WHITESPACE:
        case Token::SEMICOLON:
        case Token::COMMENT:
            lex.next();
            break;

        case Token::END_OF_FILE:
            if (not topLevel)
                logError("unexpected end of file");
            lex.next();
            return block;

        case Token::AT_KEYWORD:
            block.pushBack(consumeAtRule(lex));
            break;

        case Token::IDENT:
            if (lex.peek().data == "&") {
                auto rule = consumeRule(lex);
                if (rule)
                    block.pushBack(*rule);
            } else if (declarationAhead(lex)) {
                auto decl = consumeDeclaration(lex);
                if (decl)
                    block.pushBack(*decl);
            } else {
                auto rule = consumeRule(lex);
                if (rule)
                    block.pushBack(*rule);
            }
            break;

        case Token::RIGHT_CURLY_BRACKET:
            return block;

        default:
            auto rule = consumeRule(lex);
            if (rule)
                block.pushBack(*rule);
            break;
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-component-value
Sst consumeComponentValue(Lexer& lex) {
    switch (lex.peek().type) {
    case Token::LEFT_SQUARE_BRACKET:
        return consumeBlock(lex, Token::RIGHT_SQUARE_BRACKET);

    case Token::LEFT_CURLY_BRACKET:
        return consumeBlock(lex, Token::RIGHT_CURLY_BRACKET);

    case Token::LEFT_PARENTHESIS:
        return consumeBlock(lex, Token::RIGHT_PARENTHESIS);

    case Token::FUNCTION:
        return consumeFunc(lex);

    default:
        return lex.next();
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-a-simple-block
Sst consumeBlock(Lexer& lex, Token::Type term) {
    Sst block = Sst::BLOCK;
    lex.next();

    while (true) {
        auto token = lex.peek();

        switch (token.type) {
        case Token::END_OF_FILE:
            logError("unexpected end of file");
            return block;

        default:
            if (token.type == term) {
                lex.next();
                return block;
            }

            block.content.emplaceBack(consumeComponentValue(lex));
            break;
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-function
Sst consumeFunc(Lexer& lex) {
    Sst fn = Sst::FUNC;
    fn.prefix = lex.next();

    while (true) {
        switch (lex.peek().type) {
        case Token::COMMENT:
            lex.next();
            break;

        case Token::END_OF_FILE:
            logError("unexpected end of file");
            return fn;

        case Token::RIGHT_PARENTHESIS:
            lex.next();
            return fn;

        default:
            fn.content.pushBack(consumeComponentValue(lex));
            break;
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-qualified-rule
Opt<Sst> consumeRule(Lexer& lex) {
    Sst rule{Sst::RULE};
    Content prefix;

    while (true) {
        switch (lex.peek().type) {
        case Token::END_OF_FILE:
            logError("unexpected end of file");
            lex.next();
            return rule;

        case Token::LEFT_CURLY_BRACKET: {
            rule.prefix = std::move(prefix);
            rule.content = consumeDeclarationBlock(lex);
            return rule;
        }

        default:
            prefix.pushBack(consumeComponentValue(lex));
            break;
        }
    }
}

// https://www.w3.org/TR/css-syntax-3/#consume-list-of-rules
Content consumeRuleList(Lexer& lex, bool topLevel) {
    Content list{};

    while (true) {
        switch (lex.peek().type) {
        case Token::COMMENT:
        case Token::WHITESPACE:
            lex.next();
            break;

        case Token::END_OF_FILE:
            lex.next();
            return list;

        case Token::CDC:
        case Token::CDO: {
            if (not topLevel) {
                auto rule = consumeRule(lex);
                if (rule)
                    list.pushBack(*rule);
            }
            break;
        }

        case Token::AT_KEYWORD: {
            list.pushBack(consumeAtRule(lex));
            break;
        }

        default: {
            auto rule = consumeRule(lex);
            if (rule)
                list.pushBack(*rule);
            break;
        }
        }
    }
}

// NOSPEC specialized parser for selectors,
// it's not used in the normal workflow but for testing purposes and querySelectors
Content consumeSelector(Lexer& lex) {
    Content value;

    while ((lex.peek() != Token::END_OF_FILE and lex.peek() != Token::SEMICOLON and lex.peek() != Token::RIGHT_CURLY_BRACKET)) {
        value.pushBack(consumeComponentValue(lex));
    }
    return value;
}

Content consumeDeclarationValue(Lexer& lex) {
    Content value;
    // 3. While the next input token is a <whitespace-token>, consume the next input token.
    eatWhitespace(lex);

    // 4. As long as the next input token is anything other than an <EOF-token>,
    //    consume a component value and append it to the declaration’s value.
    while ((lex.peek() != Token::END_OF_FILE and lex.peek() != Token::SEMICOLON and lex.peek() != Token::RIGHT_CURLY_BRACKET)) {
        value.pushBack(consumeComponentValue(lex));
        eatWhitespace(lex);
    }
    return value;
}

bool declarationAhead(Lexer lex) {
    bool res = lex.peek() == Token::IDENT;
    lex.next();
    eatWhitespace(lex);
    return res and lex.peek() == Token::COLON;
}

Opt<Sst> consumeDeclaration(Lexer& lex) {
    Sst decl{Sst::DECL};
    decl.token = lex.next();

    // 1. While the next input token is a <whitespace-token>, consume the next input token.
    eatWhitespace(lex);

    // 2. If the next input token is anything other than a <colon-token>, this is a parse error. Return nothing.
    if (lex.peek() != Token::COLON) {
        logError("expected colon");
        return NONE;
    }

    // Otherwise, consume the next input token.
    lex.next();

    // Parse the declaration’s value.
    decl.content = consumeDeclarationValue(lex);

    return decl;
}

} // namespace Vaev::Css
