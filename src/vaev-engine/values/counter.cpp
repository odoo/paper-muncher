module;

#include <karm/macros>

export module Vaev.Engine:values.counter;

import :values.primitives;
import :values.image;

namespace Vaev {

export using CounterSymbolsType = Union<
    Keywords::Cyclic,
    Keywords::Numeric,
    Keywords::Alphabetic,
    Keywords::Fixed>;

// https://drafts.csswg.org/css-counter-styles-3/#typedef-symbol
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
    static Res<CounterSymbolsFunc> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() != Css::Token::function("symbols("))
            return Error::invalidData("expected counter(");

        Cursor<Css::Sst> content = c->content;
        auto type = parseValue<CounterSymbolsType>(content).ok();

        auto symbols = try$(parseValue<Vec<CounterSymbol>>(content));
        if (symbols.len() == 0)
            return Error::invalidData("expected at least one symbol in symbols()");

        return Ok(CounterSymbolsFunc{type, symbols});
    }
};

// https://drafts.csswg.org/css-counter-styles-3/#counter-style-system
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

export using CounterSystem = Union<
    Keywords::Cyclic,
    Keywords::Numeric,
    Keywords::Alphabetic,
    Keywords ::Symbolic,
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
    static Res<CounterFunc> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix != Css::Token::function("counter("))
            return Error::invalidData("expected counter(");

        Cursor<Css::Sst> content = c->content;
        auto name = try$(parseValue<CustomIdent>(content));
        content.skip(Css::Token::COMMA);
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
    static Res<CountersFunc> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() != Css::Token::function("counters("))
            return Error::invalidData("expected counter(");

        Cursor<Css::Sst> content = c->content;
        auto name = try$(parseValue<CustomIdent>(content));
        c.skip(Css::Token::COMMA);
        auto join = try$(parseValue<String>(content));
        c.skip(Css::Token::COMMA);
        auto maybeSymbols = parseValue<CounterStyle>(content).ok();

        c.next();
        return Ok(CountersFunc{name, join, maybeSymbols});
    }
};

export struct CounterProps {
    struct Reset {
        CustomIdent name;
        bool reversed;
        Opt<Integer> value = 0;

        void repr(Io::Emit& e) const {
            e("(counter-reset {}{} {})", reversed ? "reversed " : "", name, value);
        }
    };

    struct Increment {
        CustomIdent name;
        Opt<Integer> value;

        void repr(Io::Emit& e) const {
            e("(counter-increment {} {})", name, value);
        }
    };

    using Set = Increment;

    Vec<Reset> reset;
    Vec<Increment> increment;
    Vec<Set> set;
};

export template <>
struct ValueParser<CounterProps::Reset> {
    static Res<CounterProps::Reset> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        bool reversed = false;
        CustomIdent name;

        if (c.peek() == Css::Token::function("reversed(")) {
            Cursor<Css::Sst> content = c->content;
            name = try$(parseValue<CustomIdent>(content));
            reversed = true;
            c.next();
        } else {
            name = try$(parseValue<CustomIdent>(c));
        }

        auto value = try$(parseValue<Integer>(c));
        return Ok(CounterProps::Reset{name, reversed, value});
    }
};

export template <>
struct ValueParser<CounterProps::Increment> {
    static Res<CounterProps::Increment> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");
        CustomIdent name = try$(parseValue<CustomIdent>(c));
        auto value = try$(parseValue<Integer>(c));
        return Ok(CounterProps::Increment{name, value});
    }
};

} // namespace Vaev
