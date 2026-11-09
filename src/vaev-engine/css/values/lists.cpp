module;

#include <karm/macros>

export module Vaev.Engine:css.values.lists;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.counterStyles;
import :css.values.images;
import :css.values.primitives;

using namespace Karm;

namespace Vaev::Css {

// https://drafts.csswg.org/css-lists/#funcdef-counter
export struct CounterFunc {
    CustomIdent name;
    Opt<CounterStyle> style = NONE;

    void repr(Io::Emit& e) const {
        e("(counter {})", name);
    }
};

export template <>
struct ValueParser<CounterFunc> {
    static Res<CounterFunc> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix != Token::function("counter("))
            return Error::invalidData("expected counter(");

        Cursor<Sst> content = c->content;
        auto name = try$(parseValue<CustomIdent>(content));
        content.skip(Token::COMMA);
        auto maybeSymbols = parseValue<CounterStyle>(content).ok();

        c.next();
        return Ok(CounterFunc{name, maybeSymbols});
    }
};

// https://drafts.csswg.org/css-lists/#funcdef-counters
export struct CountersFunc {
    CustomIdent name;
    String join;
    Opt<CounterStyle> style = NONE;

    void repr(Io::Emit& e) const {
        e("(counters {} {} {})", name, join, style);
    }
};


export template <>
struct ValueParser<CountersFunc> {
    static Res<CountersFunc> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() != Token::function("counters("))
            return Error::invalidData("expected counter(");

        Cursor<Sst> content = c->content;
        auto name = try$(parseValue<CustomIdent>(content));
        c.skip(Token::COMMA);
        auto join = try$(parseValue<String>(content));
        c.skip(Token::COMMA);
        auto maybeSymbols = parseValue<CounterStyle>(content).ok();

        c.next();
        return Ok(CountersFunc{name, join, maybeSymbols});
    }
};

// https://www.w3.org/TR/css-lists-3/#image-markers
export using ListImage = Union<Keywords::None, Image>;

// https://www.w3.org/TR/css-lists-3/#list-style-position-property
export using ListPosition = Union<Keywords::Inside, Keywords::Outside>;

// https://www.w3.org/TR/css-lists-3/#marker-side
export using MarkerSide = Union<Keywords::MatchSelf, Keywords::MatchParent>;

// https://www.w3.org/TR/css-lists-3/#text-markers
export using ListType = Union<Keywords::None, CustomIdent, CounterSymbolsFunc, String>;

// https://www.w3.org/TR/css-lists-3/#counter-reset
export struct CounterReset {
    CustomIdent name;
    bool reversed;
    Opt<Integer> value = Some(0);

    void repr(Io::Emit& e) const {
        e("(counter-reset {}{} {})", reversed ? "reversed " : "", name, value);
    }
};

// https://www.w3.org/TR/css-lists-3/#increment-set
export struct CounterIncrement {
    CustomIdent name;
    Opt<Integer> value;

    void repr(Io::Emit& e) const {
        e("(counter-increment {} {})", name, value);
    }
};

// https://drafts.csswg.org/css-lists-3/#typedef-counter-name
// Counters are referred to in CSS syntax using the <counter-name> type, which represents their name as a <custom-ident>.
// A <counter-name> name cannot match the keyword none; such an identifier is invalid as a <counter-name>.
static Res<CustomIdent> _parseCounterName(Cursor<Sst>& c) {
    if (c.peek() == Token::ident("none"))
        return Error::invalidData("none is not a valid counter name");
    return parseValue<CustomIdent>(c);
}

// https://drafts.csswg.org/css-lists-3/#counter-reset
export template <>
struct ValueParser<CounterReset> {
    static Res<CounterReset> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        bool reversed = false;
        CustomIdent name;

        if (c.peek() == Sst::FUNC and c.peek().prefix == Token::function("reversed(")) {
            Cursor<Sst> content = c->content;
            eatWhitespace(content);
            name = try$(_parseCounterName(content));
            eatWhitespace(content);
            if (not content.ended())
                return Error::invalidData("expected end of reversed()");
            reversed = true;
            c.next();
        } else {
            name = try$(_parseCounterName(c));
        }

        auto value = parseValue<Integer>(c).ok();
        return Ok(CounterReset{name, reversed, value});
    }
};

// https://drafts.csswg.org/css-lists-3/#propdef-counter-increment
export template <>
struct ValueParser<CounterIncrement> {
    static Res<CounterIncrement> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");
        CustomIdent name = try$(_parseCounterName(c));
        auto value = parseValue<Integer>(c).ok();
        return Ok(CounterIncrement{name, value});
    }
};

} // namespace Vaev::Css
