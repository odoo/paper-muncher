module;

#include <karm/macros>

export module Vaev.Engine:css.values.counterStyles;

import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.calc;
import :css.values.keywords;
import :css.values.primitives;
import :values.counterStyles;
import :values.keywords;
import :values.primitives;

namespace Vaev::Css {

// https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-additive-symbols
export struct AdditiveCounterSymbol {
    Integer value;
    CounterSymbol symbol;

    void repr(Io::Emit& e) const {
        e("(additive-symbol {} {})", value, symbol);
    }
};

export template <>
struct ValueParser<CounterSymbolsFunc> {
    static Res<CounterSymbolsFunc> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() != Token::function("symbols("))
            return Error::invalidData("expected counter(");

        Cursor<Sst> content = c->content;
        auto type = parseValue<CounterSymbolsType>(content).ok();

        auto symbols = try$(parseValue<Vec<CounterSymbol>>(content));
        if (symbols.len() == 0)
            return Error::invalidData("expected at least one symbol in symbols()");

        return Ok(CounterSymbolsFunc{type, symbols});
    }
};

// https://drafts.csswg.org/css-counter-styles-3/#fixed-system
export struct FixedCounterSystem {
    Opt<Integer> number;

    void repr(Io::Emit& e) const {
        e("(fixed number:{})", number);
    }
};

export struct ExtendsCounterSystem {
    CustomIdent name;

    void repr(Io::Emit& e) const {
        e("(extends name:{:#})", name);
    }
};

// https://drafts.csswg.org/css-counter-styles-3/#counter-style-system
export using CounterSystem = Union<
    Keywords::Cyclic,
    Keywords::Numeric,
    Keywords::Alphabetic,
    Keywords::Symbolic,
    Keywords::Additive,
    FixedCounterSystem,
    ExtendsCounterSystem>;

// https://drafts.csswg.org/css-counter-styles-3/#counter-style-negative
export struct CounterNegative {
    CounterSymbol prepended;
    Opt<CounterSymbol> appended;

    void repr(Io::Emit& e) const {
        e("{} {}", prepended, appended);
    }
};

// https://drafts.csswg.org/css-counter-styles-3/#counter-style-range
export using CounterRange = Union<Vec<Pair<Union<Integer, Keywords::Infinite>>>, Keywords::Auto>;

// https://drafts.csswg.org/css-counter-styles-3/#counter-style-speak-as
export using CounterSpeakAs = Union<
    Keywords::Auto,
    Keywords::Bullets,
    Keywords::Number,
    Keywords::Words,
    Keywords::SpellOut,
    CustomIdent>;

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

} // namespace Vaev::Css
