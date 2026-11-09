module;

#include <karm/macros>

export module Vaev.Engine:css.values.lists;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.counterStyles;
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

    static Res<CounterFunc> parse(Cursor<Sst>& c);
};

// https://www.w3.org/TR/css-lists-3/#funcdef-counters
export struct CountersFunc {
    CustomIdent name;
    String join;
    Opt<CounterStyle> style = NONE;

    void repr(Io::Emit& e) const {
        e("(counters {} {} {})", name, join, style);
    }

    static Res<CountersFunc> parse(Cursor<Sst>& c);
};

Res<CounterFunc> CounterFunc::parse(Cursor<Sst>& c) {
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

Res<CountersFunc> CountersFunc::parse(Cursor<Sst>& c) {
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

} // namespace Vaev::Css
