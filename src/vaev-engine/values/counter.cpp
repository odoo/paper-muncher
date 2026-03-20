export module Vaev.Engine:values.counter;

import :values.primitives;
import :values.image;

namespace Vaev {

// https://drafts.csswg.org/css-counter-styles-3/#typedef-symbol
using CounterSymbol = Union<String, Image, CustomIdent>;

struct AdditiveCounterSymbol {
    Integer value;
    CounterSymbol symbol;
};

// https://drafts.csswg.org/css-counter-styles-3/#counter-style-system
struct FixedCounterSystem {
    Opt<int>;
};

struct ExtendsCounterSystem {
    CustomIdent;
};

using CounterSystem = Union<
    Keywords::Cyclic,
    Keywords::Numeric,
    Keywords::Alphabetic,
    Keywords ::Symbolic,
    Keywords::Additive,
    FixedCounterSystem,
    ExtendsCounterSystem>;

// https://drafts.csswg.org/css-counter-styles-3/#counter-style-negative
struct CounterNegative {
    Opt<CounterSymbol> prepended;
    Opt<CounterSymbol> appended;
};

// https://drafts.csswg.org/css-counter-styles-3/#counter-style-range
using CounterRange = Union<Pair<Union<Integer, Keywords::Infinite>>, Keywords::Auto>;

// https://drafts.csswg.org/css-counter-styles-3/#counter-style-speak-as
using CounterSpeakAs = Union<
    Keywords::Auto,
    Keywords::Bullets,
    Keywords::Number,
    Keywords::Words,
    Keywords::SpellOut,
    CustomIdent>;

// https://drafts.csswg.org/css-counter-styles-3/#the-counter-style-rule
export struct CounterStyle {
    CustomIdent name;

    // https://drafts.csswg.org/css-counter-styles-3/#extends-system
    CustomIdent extends;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-prefix
    CounterSymbol pefix = ""s;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-suffix
    CounterSymbol suffix = ""s;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-range
    CounterRange range = Keywords::AUTO;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-pad
    Tuple<Integer, CounterSymbol> pad = {0, ""s};

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-fallback
    CustomIdent fallback;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-symbols
    Vec<CounterSymbol> symbols;

    // https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-additive-symbols
    Vec<AdditiveCounterSymbol> additiveSymbols;
};

// https://drafts.csswg.org/css-lists/#auto-numbering
// https://drafts.csswg.org/css-lists/#counter-functions
export struct CounterFunc {
    CustomIdent name;

    void repr(Io::Emit& e) const {
        e("counter (type:'{}')", type);
    }
};

export struct CountersFunc {
    CustomIdent name;
    String join;
};

struct CounterReset {
    CustomIdent name;
    bool reversed;
    Opt<Integer> value;
};

export struct CounterIncrement {
    CustomIdent name;
    Opt<Integer> value;
};

export using CounterSet = CounterIncrement;

export struct Counters {
    Vec<CounterReset> reset;
    Vec<CounterIncrement> increment;
    Vec<CounterSet> set;
};

} // namespace Vaev