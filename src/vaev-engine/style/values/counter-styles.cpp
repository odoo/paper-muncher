export module Vaev.Engine:style.values.counterStyles;

import Karm.Core;

import :css.values.counterStyles;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-counter-styles-3/
export using CounterSymbolsType = Css::CounterSymbolsType;
export using CounterSymbol = Css::CounterSymbol;
export using CounterSymbolsFunc = Css::CounterSymbolsFunc;
export using CounterStyle = Css::CounterStyle;
export using AdditiveCounterSymbol = Css::AdditiveCounterSymbol;
export using FixedCounterSystem = Css::FixedCounterSystem;
export using ExtendsCounterSystem = Css::ExtendsCounterSystem;
export using CounterSystem = Css::CounterSystem;
export using CounterNegative = Css::CounterNegative;
export using CounterRange = Css::CounterRange;
export using CounterSpeakAs = Css::CounterSpeakAs;

} // namespace Vaev::Style
