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

// https://drafts.csswg.org/css-lists/#auto-numbering
// https://drafts.csswg.org/css-lists/#counter-functions
export struct CounterFunc {
    CustomIdent name;
    Opt<CounterStyle> style = NONE;

    void repr(Io::Emit& e) const {
        e("(counter {})", name);
    }
};

// https://www.w3.org/TR/css-lists-3/#funcdef-counters
export struct CountersFunc {
    CustomIdent name;
    String join;
    Opt<CounterStyle> style = NONE;

    void repr(Io::Emit& e) const {
        e("(counters {} {} {})", name, join, style);
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

} // namespace Vaev::Css
