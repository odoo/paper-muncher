module;

#include <karm/macros>

export module Vaev.Engine:props.counter;

import :props.base;
import :style.counter;
import :style.values.keywords;
import :style.values.primitives;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// https://drafts.csswg.org/css-lists/#counter-reset
export struct CounterResetProperty : Property {
    using Value = Vec<Css::CounterReset>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::COUNTER_RESET) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<CounterResetProperty>(self(), c.counters->reset);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterResetProperty>(self(), Vec<Css::CounterReset>{}));
            auto value = try$(parseValue<Value>(c));
            return Ok(makeRc<CounterResetProperty>(self(), value));
        }
    };

    Value _value;

    CounterResetProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues const&, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.counters.cow().reset = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-lists/#propdef-counter-increment
export struct CounterIncrementProperty : Property {
    using Value = Vec<Css::CounterIncrement>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::COUNTER_INCREMENT) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<CounterIncrementProperty>(self(), c.counters->increment);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterIncrementProperty>(self(), Vec<Css::CounterIncrement>{}));
            auto value = try$(parseValue<Value>(c));
            return Ok(makeRc<CounterIncrementProperty>(self(), value));
        }
    };

    Value _value;

    CounterIncrementProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues const&, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.counters.cow().increment = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-lists/#propdef-counter-set
export struct CounterSetProperty : Property {
    using Value = Vec<Css::CounterIncrement>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::COUNTER_SET) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<CounterSetProperty>(self(), c.counters->set);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterSetProperty>(self(), Vec<Css::CounterIncrement>{}));
            auto value = try$(parseValue<Value>(c));
            return Ok(makeRc<CounterSetProperty>(self(), value));
        }
    };

    Value _value;

    CounterSetProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues const&, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.counters.cow().set = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
