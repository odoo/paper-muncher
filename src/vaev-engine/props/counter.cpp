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
        Symbol name() const override {
            return Properties::COUNTER_RESET;
        }

        Rc<Property> initial() const override {
            return makeRc<CounterResetProperty>(self(), Vec<CounterProps::Reset>{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<CounterResetProperty>(self(), c.counters->reset);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterResetProperty>(self(), Vec<CounterProps::Reset>{}));
            auto value = try$(parseValue<Vec<CounterProps::Reset>>(c));
            return Ok(makeRc<CounterResetProperty>(self(), value));
        }
    };

    Vec<CounterProps::Reset> _value;

    CounterResetProperty(Rc<Property::Registration> registration, Vec<CounterProps::Reset> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.counters.cow().reset = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-lists/#propdef-counter-increment
export struct CounterIncrementProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::COUNTER_INCREMENT;
        }

        Rc<Property> initial() const override {
            return makeRc<CounterIncrementProperty>(self(), Vec<CounterProps::Increment>{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<CounterIncrementProperty>(self(), c.counters->increment);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterIncrementProperty>(self(), Vec<CounterProps::Increment>{}));
            auto value = try$(parseValue<Vec<CounterProps::Increment>>(c));
            return Ok(makeRc<CounterIncrementProperty>(self(), value));
        }
    };

    Vec<CounterProps::Increment> _value;

    CounterIncrementProperty(Rc<Property::Registration> registration, Vec<CounterProps::Increment> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.counters.cow().increment = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-lists/#propdef-counter-set
export struct CounterSetProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::COUNTER_SET;
        }

        Rc<Property> initial() const override {
            return makeRc<CounterSetProperty>(self(), Vec<CounterProps::Set>{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<CounterSetProperty>(self(), c.counters->set);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterSetProperty>(self(), Vec<CounterProps::Set>{}));
            auto value = try$(parseValue<Vec<CounterProps::Set>>(c));
            return Ok(makeRc<CounterSetProperty>(self(), value));
        }
    };

    Vec<CounterProps::Set> _value;

    CounterSetProperty(Rc<Property::Registration> registration, Vec<CounterProps::Set> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.counters.cow().set = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style