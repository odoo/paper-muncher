module;

#include <karm/macros>

export module Vaev.Engine:props.counter;

import :props.base;
import :style.counter;

using namespace Karm;

namespace Vaev::Css {

export template <>
struct ValueParser<Style::CounterReset> {
    static Res<Style::CounterReset> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        bool reversed = false;
        CustomIdent name;

        if (c.peek() == Token::function("reversed(")) {
            Cursor<Sst> content = c->content;
            name = try$(parseValue<CustomIdent>(content));
            reversed = true;
            c.next();
        } else {
            name = try$(parseValue<CustomIdent>(c));
        }

        auto value = try$(parseValue<Integer>(c));
        return Ok(Style::CounterReset{name, reversed, Some(value)});
    }
};

export template <>
struct ValueParser<Style::CounterIncrement> {
    static Res<Style::CounterIncrement> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");
        CustomIdent name = try$(parseValue<CustomIdent>(c));
        auto value = try$(parseValue<Integer>(c));
        return Ok(Style::CounterIncrement{name, Some(value)});
    }
};

} // namespace Vaev::Css

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
            return makeRc<CounterSetProperty>(self(), Vec<Style::CounterIncrement>{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<CounterSetProperty>(self(), c.counters->set);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterSetProperty>(self(), Vec<Style::CounterIncrement>{}));
            auto value = try$(parseValue<Vec<Style::CounterIncrement>>(c));
            return Ok(makeRc<CounterSetProperty>(self(), value));
        }
    };

    Vec<Style::CounterIncrement> _value;

    CounterSetProperty(Rc<Property::Registration> registration, Vec<Style::CounterIncrement> value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues const&, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.counters.cow().set = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
