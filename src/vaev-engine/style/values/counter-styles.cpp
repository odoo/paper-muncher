export module Vaev.Engine:style.values.counterStyles;

import Karm.Core;

import :css.values.counterStyles;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-counter-styles-3/
export using CounterSymbolsType = Css::CounterSymbolsType;

// https://drafts.csswg.org/css-counter-styles-3/#typedef-symbol
export using CounterSymbol = Css::CounterSymbol;

// https://drafts.csswg.org/css-counter-styles-3/#symbols-function
export using CounterSymbolsFunc = Css::CounterSymbolsFunc;

// https://www.w3.org/TR/css-counter-styles-3/#typedef-counter-style
export using CounterStyle = Css::CounterStyle;

// https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-additive-symbols
export using AdditiveCounterSymbol = Css::AdditiveCounterSymbol;

// https://drafts.csswg.org/css-counter-styles-3/#fixed-system
export using FixedCounterSystem = Css::FixedCounterSystem;

// https://drafts.csswg.org/css-counter-styles-3/#extends-system
export using ExtendsCounterSystem = Css::ExtendsCounterSystem;

// https://drafts.csswg.org/css-counter-styles-3/#counter-style-system
export using CounterSystem = Css::CounterSystem;

// https://drafts.csswg.org/css-counter-styles-3/#counter-style-negative
export using CounterNegative = Css::CounterNegative;

// https://drafts.csswg.org/css-counter-styles-3/#counter-style-range
export using CounterRange = Css::CounterRange;

// https://drafts.csswg.org/css-counter-styles-3/#counter-style-speak-as
export using CounterSpeakAs = Css::CounterSpeakAs;

} // namespace Vaev::Style
