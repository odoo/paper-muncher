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
            return makeRc<CounterResetProperty>(self(), Vec<Style::Counters::Reset>{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<CounterResetProperty>(self(), c.counters->reset);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterResetProperty>(self(), Vec<Style::Counters::Reset>{}));
            auto value = try$(parseValue<Vec<Style::Counters::Reset>>(c));
            return Ok(makeRc<CounterResetProperty>(self(), value));
        }
    };

    Vec<Style::Counters::Reset> _value;

    CounterResetProperty(Rc<Property::Registration> registration, Vec<Style::Counters::Reset> value)
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
            return makeRc<CounterIncrementProperty>(self(), Vec<Style::Counters::Increment>{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<CounterIncrementProperty>(self(), c.counters->increment);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterIncrementProperty>(self(), Vec<Style::Counters::Increment>{}));
            auto value = try$(parseValue<Vec<Style::Counters::Increment>>(c));
            return Ok(makeRc<CounterIncrementProperty>(self(), value));
        }
    };

    Vec<Style::Counters::Increment> _value;

    CounterIncrementProperty(Rc<Property::Registration> registration, Vec<Style::Counters::Increment> value)
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
            return makeRc<CounterSetProperty>(self(), Vec<Style::Counters::Set>{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<CounterSetProperty>(self(), c.counters->set);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterSetProperty>(self(), Vec<Style::Counters::Set>{}));
            auto value = try$(parseValue<Vec<Style::Counters::Set>>(c));
            return Ok(makeRc<CounterSetProperty>(self(), value));
        }
    };

    Vec<Style::Counters::Set> _value;

    CounterSetProperty(Rc<Property::Registration> registration, Vec<Style::Counters::Set> value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues const&, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.counters.cow().set = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
