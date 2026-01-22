export module Vaev.Engine:css.lexer;

import Karm.Core;
import Karm.Logger;

using namespace Karm;

namespace Vaev::Css {

#define FOREACH_TOKEN(TOKEN)                                         \
    TOKEN(NIL, nil)                                 /* no a token */ \
    TOKEN(IDENT, ident)                             /* foo */        \
    TOKEN(FUNCTION, function)                       /* calc( */      \
    TOKEN(AT_KEYWORD, atKeyword)                    /* @import */    \
    TOKEN(HASH, hash)                               /* #foo */       \
    TOKEN(STRING, string)                           /* "foo" */      \
    TOKEN(BAD_STRING, badString)                    /* "foo */       \
    TOKEN(URL, url)                                 /* url(foo) */   \
    TOKEN(BAD_URL, badUrl)                          /* url(foo */    \
    TOKEN(DELIM, delim)                             /* !, +, - */    \
    TOKEN(NUMBER, number)                           /* 123 */        \
    TOKEN(PERCENTAGE, percentage)                   /* 123% */       \
    TOKEN(DIMENSION, dimension)                     /* 123px */      \
    TOKEN(WHITESPACE, whitespace)                   /* ' ' */        \
    TOKEN(CDO, cdo)                                 /* <!-- */       \
    TOKEN(CDC, cdc)                                 /* --> */        \
    TOKEN(COLON, colon)                             /* : */          \
    TOKEN(SEMICOLON, semicolon)                     /* ; */          \
    TOKEN(COMMA, comma)                             /* , */          \
    TOKEN(LEFT_CURLY_BRACKET, leftCurlyBracket)     /* { */          \
    TOKEN(RIGHT_CURLY_BRACKET, rightCurlyBracket)   /* } */          \
    TOKEN(LEFT_SQUARE_BRACKET, leftSquareBracket)   /* [ */          \
    TOKEN(RIGHT_SQUARE_BRACKET, rightSquareBracket) /* ] */          \
    TOKEN(LEFT_PARENTHESIS, leftParenthesis)        /* ( */          \
    TOKEN(RIGHT_PARENTHESIS, rightParenthesis)      /* ) */          \
    TOKEN(COMMENT, comment)                         /* */            \
    TOKEN(END_OF_FILE, endOfFile)                   /* EOF */

export struct Token {
    enum struct Type {
#define ITER(NAME, ...) NAME,
        FOREACH_TOKEN(ITER)
#undef ITER

            _LEN,
    };

    using enum Type;

    Type type;
    String data;

#define ITER(ID, NAME) \
    static Token NAME(Str data = "") { return {ID, data}; }
    FOREACH_TOKEN(ITER)
#undef ITER

    Token() : type(NIL) {}

    Token(Type type, String data = ""s)
        : type(type), data(data) {}

    explicit operator bool() const {
        return type != NIL;
    }

    void repr(Io::Emit& e) const {
        if (not *this) {
            e("nil");
            return;
        }

        e("({} {#})", type, data);
    }

    bool operator==(Type type) const {
        return this->type == type;
    }

    bool operator==(Token const& other) const {
        return type == other.type and data == other.data;
    }
};

static auto const RE_BRACKET_OPEN = Re::single('{');
static auto const RE_BRACKET_CLOSE = Re::single('}');
static auto const RE_PARENTHESIS_OPEN = Re::single('(');
static auto const RE_PARENTHESIS_CLOSE = Re::single(')');
static auto const RE_SQUARE_BRACKET_OPEN = Re::single('[');
[[maybe_unused]] static auto const RE_SQUARE_BRACKET_CLOSE = Re::single(']');
static auto const RE_SEMICOLON = Re::single(';');
static auto const RE_COLON = Re::single(':');
static auto const RE_COMMA = Re::single(',');
static auto const RE_QUOTES = Re::single('"', '\'');

static auto const RE_NEWLINE = Re::either(Re::single('\n', '\r', '\f'), Re::word("\r\n"));
static auto const RE_ASCII = Re::range(0x00, 0x7f);
static auto const RE_WHITESPACE = Re::either(Re::space(), Re::single('\t'), RE_NEWLINE);
static auto const RE_WHITESPACE_TOKEN = Re::oneOrMore(Re::either(Re::space(), Re::single('\t'), RE_NEWLINE));

static auto const RE_ESCAPE = Re::chain(
    '\\'_re,
    Re::either(
        Re::oneOrMore(
            Re::negate(
                Re::either(
                    Re::xdigit(),
                    RE_NEWLINE
                )
            )
        ),
        Re::chain(
            Re::xdigit(),
            Re::atMost(5, Re::xdigit()),
            Re::zeroOrOne(RE_WHITESPACE)
        )
    )
);

[[maybe_unused]] static auto const RE_UNICODE = Re::chain(Re::single('U', 'u'), Re::oneOrMore(Re::xdigit()));

static auto const RE_IDENTIFIER = Re::chain(
    Re::either(
        Re::word("--"),
        Re::chain(
            Re::zeroOrOne(
                Re::single('-')
            ),
            Re::either(
                RE_ESCAPE,
                Re::either(
                    Re::alpha(),
                    Re::single('_'),
                    Re::negate(RE_ASCII)
                )
            )
        )
    ),
    Re::zeroOrOne(
        Re::oneOrMore(
            Re::either(
                RE_ESCAPE,
                Re::either(
                    Re::alnum(),
                    Re::single('_', '-'),
                    Re::negate(RE_ASCII)
                )
            )
        )
    )
);

static auto const RE_DIGIT = Re::oneOrMore(Re::digit());
static auto const RE_OPERATOR = Re::either(Re::single('-', '+'), Re::nothing());
static auto const RE_AT_KEYWORD = Re::chain('@'_re, RE_IDENTIFIER);

// non printable token https://www.w3.org/TR/css-syntax-3/#non-printable-code-point
static auto const RE_URL = Re::chain(
    Re::zeroOrOne(RE_WHITESPACE_TOKEN),
    Re::zeroOrMore(
        Re::either(
            RE_ESCAPE,
            Re::negate(
                Re::single('"', '\'', '(', ')', '\\', 0x007F, 0x000B) |
                RE_WHITESPACE | Re::range(0x0000, 0x0008) |
                Re::range(0x000E, 0x001F)
            )
        )
    ),
    Re::zeroOrOne(RE_WHITESPACE_TOKEN),
    RE_PARENTHESIS_CLOSE
);

static auto const RE_HASH = Re::chain(
    Re::single('#'),
    Re::oneOrMore(
        Re::either(
            RE_ESCAPE,
            Re::either(
                Re::alnum(),
                Re::single('-', '_'),
                Re::negate(RE_ASCII)
            )
        )
    )
);

// https://www.w3.org/TR/css-syntax-3/#consume-number
static auto const RE_NUMBER = Re::chain(
    RE_OPERATOR,
    Re::either(
        Re::chain(
            RE_DIGIT, Re::zeroOrOne(Re::chain(Re::single('.'), RE_DIGIT))
        ),
        Re::chain(Re::single('.'), RE_DIGIT)
    ),
    Re::zeroOrOne(
        Re::chain(
            Re::either(
                Re::single('e'),
                Re::single('E')
            ),
            RE_OPERATOR, RE_DIGIT
        )
    )
);

export struct Lexer {
    Io::SScan _scan;

    Lexer(Str text) : _scan(text) {
    }

    Lexer(Io::SScan const& scan)
        : _scan(scan) {
    }

    Token peek() const {
        Io::SScan scan = _scan;
        return _next(scan);
    }

    Token _nextIdent(Io::SScan& s) const {
        if (not s.skip('('))
            return {Token::IDENT, s.end()};

        if (eqCi(s.end(), "url("s)) {
            if (s.ahead(Re::zeroOrMore(RE_WHITESPACE) & RE_QUOTES)) {
                return {Token::FUNCTION, s.end()};
            }
            s.skip(RE_URL);
            return {Token::URL, s.end()};
        }

        return {Token::FUNCTION, s.end()};
    }

    // https://www.w3.org/TR/css-syntax-3/#check-if-two-code-points-are-a-valid-escape
    bool _checkValidEscape(Io::SScan& s) {
        if (s.rem() < 2)
            return false;
        // If the first code point is not U+005C REVERSE SOLIDUS (\), return false.
        if (s.peek() != '\\')
            return false;
        // Otherwise, if the second code point is a newline, return false.
        else if (s.ahead('\\'_re & RE_NEWLINE))
            return false;
        // Otherwise, return true.
        return true;
    }

    // https://www.w3.org/TR/css-syntax-3/#consume-an-escaped-code-point
    Rune _consumeEscapeCodepoint(Io::SScan& s) const {
        if (not s.skip('\\'))
            return U'�';

        // hex digit
        if (auto hex = s.token(Re::nOrN(1, 5, Re::xdigit()))) {
            // Consume as many hex digits as possible, but no more than 5.
            // NOTE: This means 1-6 hex digits have been consumed in total.

            // If the next input code point is whitespace, consume it as well.
            s.skip(RE_WHITESPACE);

            // Interpret the hex digits as a hexadecimal number.
            auto num = Io::atou(hex, {.base = 16}).unwrap();

            // If this number is zero, or is for a surrogate, or is greater than the maximum allowed code point
            if (0xD800 <= num and num <= 0xDFFF)
                return U'�';
            if (num == 0 or num > 0x10FFFF)
                // return U+FFFD REPLACEMENT CHARACTER (�).
                return U'�';

            // Otherwise, return the code point with that value.
            return num;
        }
        // EOF
        else if (s.ended()) {
            // This is a parse error. Return U+FFFD REPLACEMENT CHARACTER (�).
            logWarn("Unexpected EOF");
            return U'�';
        }

        // anything else
        else {
            return s.next();
        }
    }

    // https://www.w3.org/TR/css-syntax-3/#consume-string-token
    Token _consumeStringToken(Io::SScan& s, Opt<Rune> endingCodepoint = NONE) const {
        // Initially create a <string-token> with its value set to the empty string.
        StringBuilder sb;
        if (not endingCodepoint)
            endingCodepoint = s.next();

        // Repeatedly consume the next input code point from the stream:
        while (not s.ended()) {
            // ending code point
            if (s.peek() == endingCodepoint) {
                // Return the <string-token>.
                s.next();
                return {Token::STRING, sb.take()};
            }
            // newline
            else if (s.ahead(RE_NEWLINE)) {
                // This is a parse error. Reconsume the current input code point, create a <bad-string-token>, and return it.
                logWarn("tokenizing bad string due to newline");
                return {Token::BAD_STRING, sb.take()};
            }
            // U+005C REVERSE SOLIDUS (\)
            else if (s.peek() == '\\') {
                // If the next input code point is EOF, do nothing.
                if (s.rem() == 1)
                    s.next();
                // Otherwise, if the next input code point is a newline, consume it.
                else if (s.skip('\\'_re & RE_NEWLINE))
                    /* consumed by skip() */;
                // Otherwise, (the stream starts with a valid escape) consume an escaped code point and append the returned code point to the <string-token>’s value.
                else
                    sb.append(_consumeEscapeCodepoint(s));
            }
            // anything else
            else {
                // Append the current input code point to the <string-token>’s value.
                sb.append(s.next());
            }
        }

        // EOF
        // This is a parse error. Return the <string-token>.
        logWarn("unexpected EOF");
        return {Token::STRING, sb.take()};
    }

    Token _next(Io::SScan& s) const {
        s.begin();
        if (s.ended()) {
            return {Token::END_OF_FILE, s.end()};
        } else if (s.skip(RE_WHITESPACE_TOKEN)) {
            return {Token::WHITESPACE, s.end()};
        } else if (s.skip(RE_BRACKET_OPEN)) {
            return {Token::LEFT_CURLY_BRACKET, s.end()};
        } else if (s.skip(RE_BRACKET_CLOSE)) {
            return {Token::RIGHT_CURLY_BRACKET, s.end()};
        } else if (s.skip(RE_SQUARE_BRACKET_OPEN)) {
            return {Token::LEFT_SQUARE_BRACKET, s.end()};
        } else if (s.skip(RE_SQUARE_BRACKET_CLOSE)) {
            return {Token::RIGHT_SQUARE_BRACKET, s.end()};
        } else if (s.skip(RE_PARENTHESIS_OPEN)) {
            return {Token::LEFT_PARENTHESIS, s.end()};
        } else if (s.skip(RE_PARENTHESIS_CLOSE)) {
            return {Token::RIGHT_PARENTHESIS, s.end()};
        } else if (s.skip(RE_SEMICOLON)) {
            return {Token::SEMICOLON, s.end()};
        } else if (s.skip(RE_COLON)) {
            return {Token::COLON, s.end()};
        } else if (s.skip(RE_COMMA)) {
            return {Token::COMMA, s.end()};
        } else if (s.skip(RE_HASH)) {
            return {Token::HASH, s.end()};
        } else if (s.skip("<!--")) {
            return {Token::CDO, s.end()};
        } else if (s.skip("-->")) {
            return {Token::CDC, s.end()};
        } else if (s.skip("/*")) {
            // https://www.w3.org/TR/css-syntax-3/#consume-comment
            s.skip(Re::untilAndConsume(Re::word("*/")));
            return {Token::COMMENT, s.end()};
        } else if (s.skip(RE_NUMBER)) {
            // https://www.w3.org/TR/css-syntax-3/#consume-numeric-token
            if (s.skip(RE_IDENTIFIER)) {
                return {Token::DIMENSION, s.end()};
            } else if (s.skip(Re::single('%'))) {
                return {Token::PERCENTAGE, s.end()};
            } else {
                return {Token::NUMBER, s.end()};
            }
        } else if (s.skip(RE_IDENTIFIER)) {
            return _nextIdent(s);
        } else if (s.skip(RE_AT_KEYWORD)) {
            return {Token::AT_KEYWORD, s.end()};
        } else if (s.peek() == '"' or s.peek() == '\'') {
            return _consumeStringToken(s);
        } else {
            s.next();
            return {Token::DELIM, s.end()};
        }
    }

    Token next() {
        return _next(_scan);
    }

    bool ended() const {
        return _scan.ended();
    }
};

// https://www.w3.org/TR/css-syntax-3/#consume-declaration
export void eatWhitespace(Lexer& lex) {
    while (lex.peek() == Token::WHITESPACE and not lex.ended())
        lex.next();
}

} // namespace Vaev::Css
