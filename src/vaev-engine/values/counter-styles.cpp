module;

#include <karm/macros>

export module Vaev.Engine:values.counterStyles;

import Karm.Core;

import :values.keywords;
import :values.primitives;

using namespace Karm;

namespace Vaev {

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

} // namespace Vaev
