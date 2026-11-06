module;

#include <karm/macros>

export module Vaev.Engine:css.values.counter;

import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.calc2;
import :css.values.image;
import :css.values.keywords;
import :css.values.primitives;
import :values.counter;
import :values.keywords;
import :values.primitives;

namespace Vaev::Css {

// https://drafts.csswg.org/css-counter-styles-3/#typedef-symbol
// TODO: Add Image support to CounterSymbol
export using CounterSymbol = Union<String, /* Image ,*/ CustomIdent>;

// https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-additive-symbols
export struct AdditiveCounterSymbol {
    Integer value;
    CounterSymbol symbol;

    void repr(Io::Emit& e) const {
        e("(additive-symbol {} {})", value, symbol);
    }
};

// https://drafts.csswg.org/css-counter-styles-3/#symbols-function
export struct CounterSymbolsFunc {
    Opt<CounterSymbolsType> type;
    Vec<CounterSymbol> symbols;

    void repr(Io::Emit& e) const {
        e("(counter-symbols {} {})", type, symbols);
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

// https://www.w3.org/TR/css-counter-styles-3/#typedef-counter-style
export using CounterStyle = Union<CustomIdent, CounterSymbolsFunc>;

// https://drafts.csswg.org/css-lists/#auto-numbering
// https://drafts.csswg.org/css-lists/#counter-functions
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

export using CounterSet = CounterIncrement;

export template <>
struct ValueParser<Css::CounterReset> {
    static Res<Css::CounterReset> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        bool reversed = false;
        CustomIdent name;

        if (c.peek() == Token::function("reversed(")) {
            Cursor<Sst> content = c->content;
            name = try$(parseValue<CustomIdent>(content));
            reversed = true;
            c.next();
        } else {
            name = try$(parseValue<CustomIdent>(c));
        }

        auto value = try$(parseValue<Integer>(c));
        return Ok(Css::CounterReset{name, reversed, Some(value)});
    }
};

export template <>
struct ValueParser<Css::CounterIncrement> {
    static Res<Css::CounterIncrement> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");
        CustomIdent name = try$(parseValue<CustomIdent>(c));
        auto value = try$(parseValue<Integer>(c));
        return Ok(Css::CounterIncrement{name, Some(value)});
    }
};

} // namespace Vaev::Css
