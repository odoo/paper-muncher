module;

#include <karm/macros>

export module Vaev.Engine:props.counter;

import :props.base;
import :style.counter;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-lists/#counter-reset
export struct CounterResetProperty : Property {
    static constexpr auto ID = LonghandId::COUNTER_RESET;

    static Rc<Property> initial() {
        return makeRc<CounterResetProperty>(Vec<CounterProps::Reset>{});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<CounterResetProperty>(c.counters->reset);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        if (parseValue<Keywords::None>(c))
            return Ok(makeRc<CounterResetProperty>(Vec<CounterProps::Reset>{}));
        auto value = try$(parseValue<Vec<CounterProps::Reset>>(c));
        return Ok(makeRc<CounterResetProperty>(value));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::COUNTER_RESET) {}
    };

    Vec<CounterProps::Reset> _value;

    CounterResetProperty(Vec<CounterProps::Reset> value)
        : Property(ID), _value(value) {}

    void apply(ComputedValues const&, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.counters.cow().reset = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-lists/#propdef-counter-increment
export struct CounterIncrementProperty : Property {
    static constexpr auto ID = LonghandId::COUNTER_INCREMENT;

    static Rc<Property> initial() {
        return makeRc<CounterIncrementProperty>(Vec<CounterProps::Increment>{});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<CounterIncrementProperty>(c.counters->increment);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        if (parseValue<Keywords::None>(c))
            return Ok(makeRc<CounterIncrementProperty>(Vec<CounterProps::Increment>{}));
        auto value = try$(parseValue<Vec<CounterProps::Increment>>(c));
        return Ok(makeRc<CounterIncrementProperty>(value));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::COUNTER_INCREMENT) {}
    };

    Vec<CounterProps::Increment> _value;

    CounterIncrementProperty(Vec<CounterProps::Increment> value)
        : Property(ID), _value(value) {}

    void apply(ComputedValues const&, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.counters.cow().increment = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-lists/#propdef-counter-set
export struct CounterSetProperty : Property {
    static constexpr auto ID = LonghandId::COUNTER_SET;

    static Rc<Property> initial() {
        return makeRc<CounterSetProperty>(Vec<CounterProps::Set>{});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<CounterSetProperty>(c.counters->set);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        if (parseValue<Keywords::None>(c))
            return Ok(makeRc<CounterSetProperty>(Vec<CounterProps::Set>{}));
        auto value = try$(parseValue<Vec<CounterProps::Set>>(c));
        return Ok(makeRc<CounterSetProperty>(value));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::COUNTER_SET) {}
    };

    Vec<CounterProps::Set> _value;

    CounterSetProperty(Vec<CounterProps::Set> value)
        : Property(ID), _value(value) {}

    void apply(ComputedValues const&, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.counters.cow().set = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
