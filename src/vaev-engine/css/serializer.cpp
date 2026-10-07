module;

#include <karm/macros>

export module Vaev.Engine:css.serializer;

import Karm.Core;

import :css.lexer;
import :css.parser;

using namespace Karm;

namespace Vaev::Css {

// MARK: Serializer ------------------------------------------------------------
// https://www.w3.org/TR/css-syntax-3/#serialization

struct Serializer {
    Io::TextWriter& _w;
    Token _last{};

    // MARK: Tokens ------------------------------------------------------------

    // https://www.w3.org/TR/cssom-1/#serialize-a-string
    Res<> _serializeString(Str string, bool bad = false) {
        // To serialize a string means to create a string represented by '"' (U+0022),
        try$(_w.writeRune('"'));

        // followed by the result of applying the rules below to each character of the given string,
        for (auto character : iterRunes(string)) {
            // If the character is NULL (U+0000), then the REPLACEMENT CHARACTER (U+FFFD).
            if (character == 0)
                try$(_w.writeRune(0xfffd));
            // If the character is in the range [\1-\1f] (U+0001 to U+001F)
            // or is U+007F, the character escaped as code point.
            else if ((character >= 0x1 and character <= 0x1f) or character == 0x7f)
                try$(Io::format(_w, "\\{:x} ", character));
            // If the character is '"' (U+0022) or "\" (U+005C), the escaped character.
            else if (character == '"' or character == '\\') {
                try$(_w.writeRune('\\'));
                try$(_w.writeRune(character));
            }
            // Otherwise, the character itself.
            else
                try$(_w.writeRune(character));
        }

        // NOSPEC: There is no spec for serializing a bad string, so it ends with the newline that cut it.
        if (bad)
            return _w.writeRune('\n');

        // followed by '"' (U+0022):
        return _w.writeRune('"');
    }

    // https://www.w3.org/TR/css-syntax-3/#serialization-tables
    Res<> _token(Token const& token) {
        // For any consecutive pair of tokens, if the first token shows up in the row headings of the following table,
        // and the second token shows up in the column headings, and there’s a ✗ in the cell denoted by the
        // intersection of the chosen row and column, the pair of tokens must be serialized with a comment between them.
        //
        // If the tokenizer preserves comments, and there were comments originally between the token pair,
        // the preserved comment(s) should be used; otherwise, an empty comment (/**/) must be inserted.
        if (_last.mergesWith(token))
            try$(_w.writeStr("/**/"s));

        if (token == Token::STRING or token == Token::BAD_STRING)
            try$(_serializeString(token.data, token == Token::BAD_STRING));
        // A <delim-token> containing U+005C REVERSE SOLIDUS (\) must be serialized as U+005C REVERSE SOLIDUS
        // followed by a newline.
        else if (token == Token::delim("\\"))
            try$(_w.writeStr("\\\n"s));
        else
            try$(_w.writeStr(token.data.str()));

        _last = token;
        return Ok();
    }

    Res<> _text(Str text) {
        try$(_w.writeStr(text));
        _last = Token{};
        return Ok();
    }

    Res<> _content(Slice<Sst> content) {
        for (usize i = 0; i < content.len(); i++) {
            if (i > 0 and content[i - 1].statement() and content[i].statement())
                try$(_text(content[i - 1] == Sst::DECL and content[i] == Sst::DECL ? " " : "\n"));
            try$(serialize(content[i]));
        }
        return Ok();
    }

    // MARK: Rules -------------------------------------------------------------

    // https://www.w3.org/TR/cssom-1/#serialize-a-css-declaration
    Res<> _serializeDeclaration(Token const& property, Slice<Sst> value, bool important) {
        // 1. Let s be the empty string.
        // NOTE: Instead of building s, everything is appended to the writer directly.

        // 2. Append property to s.
        try$(_token(property));

        // 3. Append ": " (U+003A U+0020) to s.
        try$(_text(": "));

        // 4. Append value to s.
        try$(_content(value));

        // 5. If the important flag is set, append " !important"
        //    (U+0020 U+0021 U+0069 U+006D U+0070 U+006F U+0072 U+0074 U+0061 U+006E U+0074) to s.
        if (important)
            try$(_text(" !important"));

        // 6. Append ";" (U+003B) to s.
        try$(_text(";"));

        // 7. Return s.
        // NOTE: Everything was already appended to the writer, return Ok().
        return Ok();
    }

    // https://www.w3.org/TR/cssom-1/#serialize-a-css-declaration-block
    Res<> _serializeDeclarationBlock(Slice<Sst> declarations) {
        // NOSPEC: CSSOM stores shorthands as longhands, so the spec merges them back and skips the ones already
        //         written. The Sst keeps declarations as written, so there is nothing to merge and they're only joined.
        for (usize i = 0; i < declarations.len(); i++) {
            // 4. Return list joined with " " (U+0020).
            if (i > 0)
                try$(_text(" "));
            try$(serialize(declarations[i]));
        }
        return Ok();
    }

    static Slice<Sst> _declarations(Slice<Sst> content) {
        usize len = 0;
        while (len < content.len() and content[len] == Sst::DECL)
            len++;
        return sub(content, 0, len);
    }

    // https://www.w3.org/TR/cssom-1/#serialize-a-css-rule
    Res<> _serializeRule(Sst const& sst) {
        // NOSPEC: The Sst doesn't know the type of a rule, so the prelude stands in for the selectors
        //         and every rule with a block follows the CSSStyleRule steps.
        if (sst.token)
            try$(_token(sst.token));

        if (sst.prefix) {
            Slice<Sst> prelude = (*sst.prefix)->content;
            while (not isEmpty(prelude) and last(prelude) == Token::WHITESPACE)
                prelude = sub(prelude, 0, prelude.len() - 1);
            try$(_content(prelude));
        }

        // https://www.w3.org/TR/css-syntax-3/#block-at-rule
        if (not sst.flags.has(Sst::WITH_BLOCK))
            return _text(";");

        // 1. Let s initially be the result of performing serialize a group of selectors on the rule’s associated selectors,
        //    followed by the string " {", i.e., a single SPACE (U+0020), followed by LEFT CURLY BRACKET (U+007B).
        try$(_text(" {"));

        // 2. Let decls be the result of performing serialize a CSS declaration block on the rule’s associated declarations,
        //    or null if there are no such declarations.
        Slice<Sst> content = sst.content;
        Slice<Sst> decls = _declarations(content);

        // 3. Let rules be the result of performing serialize a CSS rule on each rule in the rule’s cssRules list,
        //    or null if there are no such rules.
        Slice<Sst> rules = next(content, decls.len());

        // 4. If decls and rules are both null, append " }" to s (i.e. a single SPACE (U+0020)
        //    followed by RIGHT CURLY BRACKET (U+007D)) and return s.
        if (isEmpty(decls) and isEmpty(rules))
            return _text(" }");

        // 5. If rules is null:
        if (isEmpty(rules)) {
            // 5.1. Append a single SPACE (U+0020) to s
            try$(_text(" "));

            // 5.2. Append decls to s
            try$(_serializeDeclarationBlock(decls));

            // 5.3. Append " }" to s (i.e. a single SPACE (U+0020) followed by RIGHT CURLY BRACKET (U+007D)).
            try$(_text(" }"));

            // 5.4. Return s.
            return Ok();
        }

        // 6. Otherwise:
        // 6.1. If decls is not null, prepend it to rules.
        rules = content;

        // 6.2. For each rule in rules:
        while (not isEmpty(rules)) {
            Slice<Sst> rule = _declarations(rules);
            if (isEmpty(rule))
                rule = sub(rules, 0, 1);
            rules = next(rules, rule.len());

            // 6.2.1. Append a newline followed by two spaces to s.
            try$(_text("\n  "));

            // 6.2.2. Append rule to s.
            // https://drafts.csswg.org/css-nesting-1/#the-cssnestrule
            // "The CSSNestedDeclarations rule serializes as if its declaration block had been serialized directly."
            if (rule[0] == Sst::DECL)
                try$(_serializeDeclarationBlock(rule));
            else
                try$(serialize(rule[0]));
        }

        // 6.3. Append a newline followed by RIGHT CURLY BRACKET (U+007D) to s.
        try$(_text("\n}"));

        // 6.4. Return s.
        return Ok();
    }

    Res<> serialize(Sst const& sst) {
        switch (sst.type) {
        case Sst::TOKEN:
            return _token(sst.token);

        case Sst::LIST:
            return _content(sst.content);

        case Sst::FUNC:
            try$(_token((*sst.prefix)->token));
            try$(_content(sst.content));
            return _token(Token::rightParenthesis(")"));

        case Sst::BLOCK:
            try$(_token(sst.token));
            try$(_content(sst.content));
            return _token(sst.token.closing());

        case Sst::DECL:
            return _serializeDeclaration(sst.token, sst.content, sst.flags.has(Sst::IMPORTANT));

        case Sst::RULE:
            return _serializeRule(sst);

        case Sst::_LEN:
            return Ok();
        }
    }
};

export Res<> serialize(Io::TextWriter& w, Sst const& sst) {
    return Serializer{w}.serialize(sst);
}

export String serialize(Sst const& sst) {
    Io::StringWriter sw;
    serialize(sw, sst).expect("serializing to a string");
    return sw.take();
}

} // namespace Vaev::Css
