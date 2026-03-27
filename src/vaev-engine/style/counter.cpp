export module Vaev.Engine:style.counter;

import Karm.Core;
import :values.primitives;
import :values.counter;

using namespace Karm;

namespace Vaev::Style {

// MARK: Counter Set ------------------------------------------------------

using ElementHandle = void*;

struct Counter {
    CustomIdent name;
    ElementHandle el;
    bool reversed;
    Integer value;
};

struct CounterSet {
    Vec<Counter> _counters;

    static CounterSet inherits(CounterSet& parent, CounterSet& sibling) {
        // 1. Let element counters be an initially empty CSS counters set representing element’s own CSS counters set.
        CounterSet elementCounters;

        // 2. If element is the root of its document tree, return. (The element has an initially-empty counter map and inherits nothing.)

        // 3. Let counter source be the CSS counters set of element’s preceding sibling, if it has one, or else of element’s parent if it does not.
        auto& counterSource = sibling.any() ? sibling : parent;

        // 4. Let value source be the CSS counters set of the element immediately preceding element in tree order.
        auto& valueSource = sibling;

        // 5. For each (|=CSS counter/name=|, originating element, |=value=|) of value source:
        for (auto& sourceCounter : valueSource._counters) {
            // If counter source also contains a counter with the same |=CSS counter/name=| and originating element,
            if (counterSource.contains(sourceCounter.name, sourceCounter.el)) {
                // then append a copy of value source’s counter (|=CSS counter/name=|, originating element, |=value=|) to element counters.
                elementCounters.append(sourceCounter);
            }
        }

        return elementCounters;
    }

    // https://drafts.csswg.org/css-lists/#instantiate-counter
    Counter& instantiateCounter(ElementHandle el, CounterProps::Reset const& reset, Integer initial) {
        // Let counters be element’s CSS counters set.

        // Let innermost counter be the last counter in counters with the name name.
        auto [innerMostCounter, innerMostIndex] = innerMost(reset.name);
        // If innermost counter’s originating element is element or a previous sibling of element, remove innermost counter from counters.
        if (innerMostCounter and innerMostCounter->el == el) {
            _counters.removeAt(innerMostIndex);
        }
        // Append a new counter to counters with name name, originating element element, reversed being reversed, and initial value value (if given)
        _counters.pushBack(Counter{reset.name, el, reset.reversed, initial});
        return last(_counters);
    }

    // https://drafts.csswg.org/css-lists/#propdef-counter-increment
    void increment(ElementHandle el, CounterProps::Increment const& increment) {
        auto [counter, _] = innerMost(increment.name);
        if (not counter) {
            // If there is not currently a counter of the given name on the element, the element instantiates a new counter of the given name with a starting value of 0 before setting or incrementing its value.
            CounterProps::Reset counterReset{increment.name, false};
            counter = instantiateCounter(el, counterReset, 0);
        }

        counter->value += (counter->reversed ? -1 : 1) * increment.value.unwrapOr(1);
    }

    // https://drafts.csswg.org/css-lists/#propdef-counter-set
    void set(ElementHandle el, CounterProps::Set& set) {
        auto [counter, _] = innerMost(set.name);
        if (not counter) {
            // If there is not currently a counter of the given name on the element, the element instantiates a new counter of the given name with a starting value of 0 before setting or incrementing its value.
            CounterProps::Reset counterReset{set.name, false};
            counter = instantiateCounter(el, counterReset, 0);
        }

        counter->value = set.value.unwrapOr(0);
    }

    bool any() const {
        return Karm::any(_counters);
    }

    bool contains(CustomIdent name, ElementHandle el) const {
        return iter(_counters) |
               Any([&](Counter const& c) {
                   return c.name == name and c.el == el;
               });
    }

    Tuple<Opt<Counter&>, usize> innerMost(CustomIdent name) {
        Opt<Counter&> counter = NONE;
        usize index = 0;
        for (auto i : urange::zeroTo(_counters.len())) {
            auto& c = _counters[i];
            if (c.name == name) {
                counter = c;
                index = i;
            }
        }
        return {counter, index};
    }

    void append(Counter counter) {
        _counters.pushBack(counter);
    }
};

// MARK: Counter ---------------------------------------------------------------

// https://drafts.csswg.org/css-counter-styles-3/#the-counter-style-rule
export struct CounterStyle {
    // https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-system
    CounterSystem system;

    // https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-negative
    CounterNegative negative;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-prefix
    CounterSymbol prefix = String{""s};

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-suffix
    CounterSymbol suffix = String{""s};

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-range
    CounterRange range = Keywords::AUTO;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-pad
    Tuple<Integer, CounterSymbol> pad = {0, String{""s}};

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-fallback
    CustomIdent fallback;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-symbols
    Vec<CounterSymbol> symbols;

    // https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-additive-symbols
    Vec<AdditiveCounterSymbol> additiveSymbols;

    // https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-speak-as
    CounterSpeakAs speakAs;

    template <typename Descriptor>
    auto load() {
        return Descriptor::load(*this);
    }
};

struct CountersStyle {
    Map<CustomIdent, Counter> _counters;
};

// MARK: Descriptor ------------------------------------------------------------

// https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-system
struct SystemCounterDescriptor {
    CounterSystem value;

    static Str name() {
        return "system"s;
    }

    static auto load(CounterStyle& s) {
        return s.system;
    }

    void apply(CounterStyle& s) const {
        s.system = value;
    }
};

// https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-negative
struct NegativeCounterDescriptor {
    CounterNegative value;

    static Str name() {
        return "negative"s;
    }

    static auto load(CounterStyle& s) {
        return s.system;
    }

    void apply(CounterStyle& s) const {
        s.negative = value;
    }
};

// https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-prefix
struct PrefixCounterDescriptor {
    CounterSymbol value;

    static Str name() {
        return "prefix"s;
    }

    static auto load(CounterStyle& s) {
        return s.prefix;
    }

    void apply(CounterStyle& s) const {
        s.prefix = value;
    }
};

// https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-suffix
struct SuffixCounterDescriptor {
    CounterSymbol value;

    static Str name() {
        return "suffix"s;
    }

    static auto load(CounterStyle& s) {
        return s.suffix;
    }

    void apply(CounterStyle& s) const {
        s.suffix = value;
    }
};

// https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-range
struct RangeCounterDescriptor {
    CounterRange value;

    static Str name() {
        return "range"s;
    }

    static auto load(CounterStyle& s) {
        return s.range;
    }

    void apply(CounterStyle& s) const {
        s.range = value;
    }
};

// https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-pad
struct PadCounterDescriptor {
    Tuple<Integer, CounterSymbol> value;

    static Str name() {
        return "pad"s;
    }

    static auto load(CounterStyle& s) {
        return s.pad;
    }

    void apply(CounterStyle& s) const {
        s.pad = value;
    }
};

// https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-fallback
struct FallbackCounterDescriptor {
    CustomIdent value;

    static Str name() {
        return "fallback"s;
    }

    static auto load(CounterStyle& s) {
        return s.fallback;
    }

    void apply(CounterStyle& s) const {
        s.fallback = value;
    }
};

// https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-symbols
struct SymbolsCounterDescriptor {
    Vec<CounterSymbol> value;

    static Str name() {
        return "symbols"s;
    }

    static auto load(CounterStyle& s) {
        return s.symbols;
    }

    void apply(CounterStyle& s) const {
        s.symbols = value;
    }
};

// https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-additive-symbols
struct AdditiveSymbolsCounterDescriptor {
    Vec<AdditiveCounterSymbol> value;

    static Str name() {
        return "additive-symbols"s;
    }

    static auto load(CounterStyle& s) {
        return s.additiveSymbols;
    }

    void apply(CounterStyle& s) const {
        s.additiveSymbols = value;
    }
};

// https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-speak-as
struct SpeakAsCounterDescriptor {
    CounterSpeakAs value;

    static Str name() {
        return "speak-as"s;
    }

    static auto load(CounterStyle& s) {
        return s.speakAs;
    }

    void apply(CounterStyle& s) const {
        s.speakAs = value;
    }
};

using _CounterDescriptor = Union<
    SystemCounterDescriptor,
    NegativeCounterDescriptor,
    PrefixCounterDescriptor,
    SuffixCounterDescriptor,
    RangeCounterDescriptor,
    PadCounterDescriptor,
    FallbackCounterDescriptor,
    SymbolsCounterDescriptor,
    AdditiveSymbolsCounterDescriptor,
    SpeakAsCounterDescriptor>;

export struct CounterDescriptor : _CounterDescriptor {
    using _CounterDescriptor::_CounterDescriptor;

    Str name() const {
        return visit([](auto const& p) {
            return p.name();
        });
    }

    void apply(CounterStyle& c) const {
        visit([&](auto const& p) {
            p.apply(c);
        });
    }

    void repr(Io::Emit& e) const {
        e("({}", name());
        visit([&](auto const& p) {
            e(" {}", p.value);
        });
        e(")");
    }
};

} // namespace Vaev::Style