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

    void repr(Io::Emit& e) const {
        e("(counter {} {} {})", name, reversed, value);
    }
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

    Integer innerMostValue(CustomIdent name) {
        auto [counter, _] = innerMost(name);
        if (not counter)
            return 0;
        return counter.unwrap().value;
    }

    void append(Counter counter) {
        _counters.pushBack(counter);
    }

    void repr(Io::Emit& e) const {
        e("(counter-set {})", _counters);
    }
};

// MARK: Counter ---------------------------------------------------------------

// https://drafts.csswg.org/css-counter-styles-3/#counter-style-pad
export struct CounterPad {
    Integer value;
    CounterSymbol symbol;

    explicit operator bool() const {
        return value and symbol;
    }

    void repr(Io::Emit& e) const {
        e("(counter-pad {})", value, symbol);
    }
};

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
    CounterPad pad = {0, String{""s}};

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-fallback
    CustomIdent fallback;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-symbols
    Vec<CounterSymbol> symbols;

    // https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-additive-symbols
    Vec<AdditiveCounterSymbol> additiveSymbols;

    // https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-speak-as
    CounterSpeakAs speakAs;

    static CounterStyle initial() {
        return {
            .system = Keywords::SYMBOLIC,
            .negative = {.prepended = String{"-"s}, .appended = NONE},
            .prefix = String{""s},
            .suffix = String{". "s},
            .range = Keywords::AUTO,
            .pad = {0, String{""s}},
            .fallback = CustomIdent{"decimal"_sym},
            .symbols = {},
            .additiveSymbols = {},
            .speakAs = Keywords::AUTO,
        };
    }

    template <typename Descriptor>
    auto load() {
        return Descriptor::load(*this);
    }
};

struct CounterStyleSet {
    Map<CustomIdent, CounterStyle> _counters;

    // https://drafts.csswg.org/css-counter-styles-3/#cyclic-system
    Opt<Vec<CounterSymbol>> _constructCyclicCounter(CounterStyle const& style, Integer value) {
        if (not style.symbols)
            return NONE;
        isize i = (value - 1) % style.symbols.len();
        if (i < 0)
            i += style.symbols.len();
        return Vec<CounterSymbol>{style.symbols[i]};
    }

    // https://drafts.csswg.org/css-counter-styles-3/#fixed-system
    Opt<Vec<CounterSymbol>> _constructFixedCounter(CounterStyle const& style, FixedCounterSystem const& system, Integer value) {
        if (not style.symbols)
            return NONE;
        value -= system.number.unwrapOr(0);
        value -= 1;
        if (value < 0)
            return NONE;
        if (value >= static_cast<isize>(style.symbols.len()))
            return NONE;
        return Vec<CounterSymbol>{style.symbols[value]};
    }

    // https://drafts.csswg.org/css-counter-styles-3/#symbolic-system
    Opt<Vec<CounterSymbol>> _constructSymbolicCounter(CounterStyle const& style, Integer value) {
        if (not style.symbols)
            return NONE;
        if (value < 0)
            return NONE;

        auto n = style.symbols.len();
        Vec<CounterSymbol> s;

        //  1. Let the chosen symbol be symbol( (value - 1) mod N).
        auto chosen = style.symbols[(value - 1) % n];
        //  2. Let the representation length be ceil( value / N ).
        auto representationLength = Math::ceil(value / static_cast<f64>(n));
        //  3. Append the chosen symbol to S a number of times equal to the representation length.
        s.resize(representationLength, chosen);

        // Finally, return S.
        return s;
    }

    // https://drafts.csswg.org/css-counter-styles-3/#alphabetic-system
    Opt<Vec<CounterSymbol>> _constructAlphabeticCounter(CounterStyle const& style, Integer value) {
        if (style.symbols.len() < 2)
            return NONE;
        if (value <= 0)
            return NONE;

        // Let N be the length of the list of counter symbols
        auto n = style.symbols.len();
        Vec<CounterSymbol> s;

        // While value is not equal to 0:
        while (value != 0) {
            //  1. Set value to value - 1.
            value = value - 1;
            //  2. Prepend symbol( value mod N ) to S.
            s.pushFront(style.symbols[value % n]);
            //  3. Set value to floor( value / N ).
            value = value / n;
        }

        // Finally, return S.
        return s;
    }

    // https://drafts.csswg.org/css-counter-styles-3/#numeric-system
    Opt<Vec<CounterSymbol>> _constructNumericCounter(CounterStyle const& style, Integer value) {
        if (style.symbols.len() < 2)
            return NONE;

        // Let N be the length of the list of counter symbols, value initially be the counter value, S initially be the empty string, and symbol(n) be the nth counter symbol in the list of counter symbols (0-indexed).
        auto n = style.symbols.len();
        Vec<CounterSymbol> s;

        // If value is 0, append symbol(0) to S and return S.
        if (value == 0) {
            s.pushBack(style.symbols[value]);
            return s;
        }

        // While value is not equal to 0:
        while (value != 0) {
            //  1. Prepend symbol( value mod N ) to S.
            s.pushFront(style.symbols[value % n]);
            //  2. Set value to floor( value / N ).
            value = value / n;
        }

        // Return S.
        return s;
    }

    // https://drafts.csswg.org/css-counter-styles-3/#additive-system
    Opt<Vec<CounterSymbol>> _constructAdditiveCounter(CounterStyle const& style, Integer value) {
        if (not style.additiveSymbols.len())
            return NONE;

        // 1. Let value initially be the counter value, S initially be the empty
        //    string, and symbol list initially be the list of additive tuples.
        Vec<CounterSymbol> s;

        // 2. If value is zero:
        if (value == 0) {
            // 1. If symbol list contains a tuple with a weight of zero,
            //    append that tuple’s counter symbol to S and return S.
            auto zeroSym =
                iter(style.additiveSymbols) |
                FindFirst([](AdditiveCounterSymbol const& it) {
                    return it.value == 0;
                });

            if (zeroSym) {
                s.pushBack(zeroSym->symbol);
                return s;
            }

            // 2. Otherwise, the given counter value cannot be represented by
            //    this counter style, and must instead be represented by the
            //    fallback counter style.
            return NONE;
        }

        // 3. For each tuple in symbol list:
        for (auto const& [weight, symbol] : style.additiveSymbols) {
            // 1. Let symbol and weight be tuple’s counter symbol and weight, respectively.
            // 2. If weight is zero, or weight is greater than value, continue.
            if (weight == 0 or weight > value)
                continue;

            // 3. Let reps be floor( value / weight ).
            auto reps = value / weight;

            // 4. Append symbol to S reps times.
            for (auto _ : urange::zeroTo(reps))
                s.pushBack(symbol);

            // 5. Decrement value by weight * reps.
            value -= weight * reps;

            // 6. If value is zero, return S.
            if (value == 0)
                return s;
        }

        // 4. Assertion: value is still non-zero.
        if (value != 0)
            return NONE;

        return s;
    }

    Opt<Vec<CounterSymbol>> _dispatchCounterSystem(CounterStyle const& style, Integer value) {
        return style.system.visit(
            [&](Keywords::Cyclic) {
                return _constructCyclicCounter(style, value);
            },
            [&](FixedCounterSystem const& it) {
                return _constructFixedCounter(style, it, value);
            },
            [&](Keywords::Numeric) {
                return _constructNumericCounter(style, value);
            },
            [&](Keywords::Alphabetic) {
                return _constructAlphabeticCounter(style, value);
            },
            [&](Keywords::Symbolic) {
                return _constructSymbolicCounter(style, value);
            },
            [&](Keywords::Additive) {
                return _constructAdditiveCounter(style, value);
            },

            [&](ExtendsCounterSystem) -> Opt<Vec<CounterSymbol>> {
                // NOTE: This should be unreachable because all `extends` should
                //       have been resolved away by `CounterDescriptorsSet`.
                unreachable();
            }
        );
    }

    Opt<bool> _counterDefinedFor(CounterStyle const& style, Integer value) {
        auto range = style.range;
        if (auto it = range.is<Vec<Pair<Union<Integer, Keywords::Infinite>>>>()) {
            for (auto& r : *it) {
                auto lower = r.v0.visit(
                    [](Integer i) {
                        return i;
                    },
                    [](Keywords::Infinite) {
                        return Limits<Integer>::MIN;
                    }
                );

                auto upper = r.v1.visit(
                    [](Integer i) {
                        return i;
                    },
                    [](Keywords::Infinite) {
                        return Limits<Integer>::MAX;
                    }
                );

                if (value >= lower and value <= upper)
                    return true;
            }
            return false;
        }

        return NONE;
    }

    CounterStyle _fallbackToDecimal() {
        return _counters.lookup(CustomIdent{"decimal"_sym}).unwrap();
    }

    CounterStyle _lookupCounterOrFallbackToDecimal(CustomIdent counterStyleName) {
        return _counters
            .lookup(counterStyleName)
            .unwrapOrElse([&] {
                return _fallbackToDecimal();
            });
    }

    Vec<CounterSymbol> _dispatchToFallbackCounter(CounterStyle const& style, Integer value, Set<CustomIdent>& seen) {
        if (seen.contains(style.fallback))
            return _dispatchCounterFallback(_fallbackToDecimal(), value, seen);
        auto const& fallbackStyle = _lookupCounterOrFallbackToDecimal(style.fallback);
        seen.add(style.fallback);
        return _dispatchCounterFallback(fallbackStyle, value, seen);
    }

    Integer _measureRepresentation(Vec<CounterSymbol>& repr) {
        Integer len = 0;
        for (auto& i : repr) {
            len += i.visit(
                [](auto const& s) {
                    return Icu::countGrapheneClusters(s.str());
                }
            );
        }
        return len;
    }

    // https://drafts.csswg.org/css-counter-styles-3/#generate-a-counter
    Vec<CounterSymbol> _dispatchCounterFallback(CounterStyle const& style, Integer value, Set<CustomIdent>& seen) {
        // 2. If the counter value is outside the range of the counter style,
        //    exit this algorithm and instead generate a counter representation
        //    using the counter style’s fallback style and the same counter value.
        auto defined = _counterDefinedFor(style, value);
        if (defined == false)
            return _dispatchToFallbackCounter(style, value, seen);
        auto maybeRepr = _dispatchCounterSystem(style, value);
        if (maybeRepr == NONE)
            return _dispatchToFallbackCounter(style, Math::abs(value), seen);
        auto repr = maybeRepr.take();

        // 4. Prepend symbols to the representation as specified in the pad descriptor.
        if (style.pad) {
            auto& [padLen, padSym] = style.pad;
            auto reprLen = _measureRepresentation(repr);
            if (reprLen < padLen) {
                for (auto _ : urange::fromStartEnd(reprLen, padLen))
                    repr.pushFront(padSym);
            }
        }

        repr.pushFront(style.prefix);
        if (value < 0) {
            repr.pushFront(style.negative.prepended);

            // 5. If the counter value is negative and the counter style
            //    uses a negative sign, wrap the representation in the
            //    counter style’s negative sign as specified in the
            //    negative descriptor.
            if (style.negative.appended)
                repr.pushBack(*style.negative.appended);
        }
        repr.pushBack(style.suffix);

        // 6. Return the representation.
        return repr;
    }

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-system
    Vec<CounterSymbol> constructCounterRepresentation(CustomIdent counterStyleName, Integer value) {
        auto const& style = _lookupCounterOrFallbackToDecimal(counterStyleName);
        Set seen = {counterStyleName};
        return _dispatchCounterFallback(style, value, seen);
    }

    Vec<CounterSymbol> constructCounterRepresentation(CounterStyle const& anonymousStyle, Integer value) {
        Set<CustomIdent> seen = {};
        return _dispatchCounterFallback(anonymousStyle, value, seen);
    }
};

export struct CounterDescriptors {
    // https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-system
    Opt<CounterSystem> system;

    // https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-negative
    Opt<CounterNegative> negative;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-prefix
    Opt<CounterSymbol> prefix;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-suffix
    Opt<CounterSymbol> suffix;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-range
    Opt<CounterRange> range;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-pad
    Opt<CounterPad> pad;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-fallback
    Opt<CustomIdent> fallback;

    // https://drafts.csswg.org/css-counter-styles-3/#counter-style-symbols
    Opt<Vec<CounterSymbol>> symbols;

    // https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-additive-symbols
    Opt<Vec<AdditiveCounterSymbol>> additiveSymbols;

    // https://drafts.csswg.org/css-counter-styles-3/#descdef-counter-style-speak-as
    Opt<CounterSpeakAs> speakAs;

    CounterStyle extends(CounterStyle const& other) const {
        return {
            .system = system.unwrapOr(other.system),
            .negative = negative.unwrapOr(other.negative),
            .prefix = prefix.unwrapOr(other.prefix),
            .suffix = suffix.unwrapOr(other.suffix),
            .range = range.unwrapOr(other.range),
            .pad = pad.unwrapOr(other.pad),
            .fallback = fallback.unwrapOr(other.fallback),
            .symbols = symbols.unwrapOr(other.symbols),
            .additiveSymbols = additiveSymbols.unwrapOr(other.additiveSymbols),
            .speakAs = speakAs.unwrapOr(other.speakAs),
        };
    }
};

export using CounterDescriptorSet = Map<CustomIdent, CounterDescriptors>;

export CounterStyleSet resolveExtends(Map<CustomIdent, CounterDescriptors> const& counters) {
    Map<CustomIdent, CounterStyle> resolved = {};
    auto resolveOne = [&](this auto const& recurse, CustomIdent name, Set<CustomIdent>& seen) -> CounterStyle {
        if (auto it = resolved.lookup(name))
            return it.unwrap();

        if (seen.contains(name))
            return CounterStyle::initial();

        auto descOpt = counters.lookup(name);
        if (not descOpt)
            return CounterStyle::initial();

        seen.add(name);
        auto& desc = descOpt.unwrap();

        CounterStyle base = CounterStyle::initial();

        if (desc.system and desc.system->is<ExtendsCounterSystem>()) {
            auto const& ext = desc.system->unwrap<ExtendsCounterSystem>();
            base = recurse(ext.name, seen);
        }

        CounterStyle resolvedStyle = desc.extends(base);
        if (desc.system and desc.system->is<ExtendsCounterSystem>())
            resolvedStyle.system = base.system;

        resolved.put(name, resolvedStyle);
        return resolvedStyle;
    };

    for (auto const& [k, v] : counters.iterItems()) {
        Set<CustomIdent> seen;
        resolveOne(k, seen);
    }

    return {std::move(resolved)};
}

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
    CounterPad value;

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
