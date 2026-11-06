module;

#include <karm/macros>

export module Vaev.Engine:props.counter;

import :props.base;
import :style.counter;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-lists/#counter-reset
export struct CounterResetProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::COUNTER_RESET) {}

        Rc<Property> initial() const override {
            return makeRc<CounterResetProperty>(self(), Vec<CounterReset>{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<CounterResetProperty>(self(), c.counters->reset);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterResetProperty>(self(), Vec<CounterReset>{}));
            auto value = try$(parseValue<Vec<CounterReset>>(c));
            return Ok(makeRc<CounterResetProperty>(self(), value));
        }
    };

    Vec<CounterReset> _value;

    CounterResetProperty(Rc<Property::Registration> registration, Vec<CounterReset> value)
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
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::COUNTER_INCREMENT) {}

        Rc<Property> initial() const override {
            return makeRc<CounterIncrementProperty>(self(), Vec<CounterIncrement>{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<CounterIncrementProperty>(self(), c.counters->increment);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterIncrementProperty>(self(), Vec<CounterIncrement>{}));
            auto value = try$(parseValue<Vec<CounterIncrement>>(c));
            return Ok(makeRc<CounterIncrementProperty>(self(), value));
        }
    };

    Vec<CounterIncrement> _value;

    CounterIncrementProperty(Rc<Property::Registration> registration, Vec<CounterIncrement> value)
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
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::COUNTER_SET) {}

        Rc<Property> initial() const override {
            return makeRc<CounterSetProperty>(self(), Vec<Vaev::CounterSet>{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<CounterSetProperty>(self(), c.counters->set);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterSetProperty>(self(), Vec<Vaev::CounterSet>{}));
            auto value = try$(parseValue<Vec<Vaev::CounterSet>>(c));
            return Ok(makeRc<CounterSetProperty>(self(), value));
        }
    };

    Vec<Vaev::CounterSet> _value;

    CounterSetProperty(Rc<Property::Registration> registration, Vec<Vaev::CounterSet> value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues const&, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.counters.cow().set = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
