module;

#include <karm/macros>

export module Vaev.Engine:css.values.counterStyles;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.keywords;
import :css.values.primitives;

using namespace Karm;

namespace Vaev::Css {

export using CounterSymbolsType = Union<
    Keywords::Cyclic,
    Keywords::Numeric,
    Keywords::Alphabetic,
    Keywords::Fixed>;

// https://drafts.csswg.org/css-counter-styles-3/#typedef-symbol
// TODO: Add Image support to CounterSymbol
export using CounterSymbol = Union<String, /* Image ,*/ CustomIdent>;

// https://drafts.csswg.org/css-counter-styles-3/#symbols-function
export struct CounterSymbolsFunc {
    Opt<CounterSymbolsType> type;
    Vec<CounterSymbol> symbols;

    void repr(Io::Emit& e) const {
        e("(counter-symbols {} {})", type, symbols);
    }
};

// https://www.w3.org/TR/css-counter-styles-3/#typedef-counter-style
export using CounterStyle = Union<CustomIdent, CounterSymbolsFunc>;

// https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-additive-symbols
export struct AdditiveCounterSymbol {
    Integer value;
    CounterSymbol symbol;

    void repr(Io::Emit& e) const {
        e("(additive-symbol {} {})", value, symbol);
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

} // namespace Vaev::Css
