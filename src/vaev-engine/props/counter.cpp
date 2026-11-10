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

// https://drafts.csswg.org/css-lists-3/#counter-reset
// <counter-name> <integer>? | <reversed-counter-name> <integer>?
static Res<Css::CounterReset> _parseCounterReset(Cursor<Css::Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    bool reversed = false;
    Css::CustomIdent name;

    if (c.peek() == Css::Token::function("reversed(")) {
        Cursor<Css::Sst> content = c->content;
        name = try$(parseValue<Css::CustomIdent>(content));
        reversed = true;
        c.next();
    } else {
        name = try$(parseValue<Css::CustomIdent>(c));
    }

    auto value = try$(parseValue<Css::Integer>(c));
    return Ok(Css::CounterReset{name, reversed, Some(value)});
}

// https://drafts.csswg.org/css-lists-3/#propdef-counter-increment
// <counter-name> <integer>?
static Res<Css::CounterIncrement> _parseCounterIncrement(Cursor<Css::Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");
    Css::CustomIdent name = try$(parseValue<Css::CustomIdent>(c));
    auto value = try$(parseValue<Css::Integer>(c));
    return Ok(Css::CounterIncrement{name, Some(value)});
}

// One or more items, up to the first that doesn't parse.
template <typename T>
static Vec<T> _parseItems(Cursor<Css::Sst>& c, Res<T> (*parseItem)(Cursor<Css::Sst>&)) {
    Vec<T> items;
    while (not c.ended()) {
        auto item = parseItem(c);
        if (not item)
            break;
        items.pushBack(item.take());
    }
    return items;
}

// https://drafts.csswg.org/css-lists/#counter-reset
export struct CounterResetProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::COUNTER_RESET) {}

        InitialValue initial() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<CounterResetProperty>(self(), c.counters->reset);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterResetProperty>(self(), Vec<Css::CounterReset>{}));
            auto value = _parseItems(c, _parseCounterReset);
            return Ok(makeRc<CounterResetProperty>(self(), value));
        }
    };

    Vec<Css::CounterReset> _value;

    CounterResetProperty(Rc<Property::Registration> registration, Vec<Css::CounterReset> value)
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

        InitialValue initial() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<CounterIncrementProperty>(self(), c.counters->increment);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterIncrementProperty>(self(), Vec<Css::CounterIncrement>{}));
            auto value = _parseItems(c, _parseCounterIncrement);
            return Ok(makeRc<CounterIncrementProperty>(self(), value));
        }
    };

    Vec<Css::CounterIncrement> _value;

    CounterIncrementProperty(Rc<Property::Registration> registration, Vec<Css::CounterIncrement> value)
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

        InitialValue initial() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<CounterSetProperty>(self(), c.counters->set);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (parseValue<Keywords::None>(c))
                return Ok(makeRc<CounterSetProperty>(self(), Vec<Css::CounterIncrement>{}));
            auto value = _parseItems(c, _parseCounterIncrement);
            return Ok(makeRc<CounterSetProperty>(self(), value));
        }
    };

    Vec<Css::CounterIncrement> _value;

    CounterSetProperty(Rc<Property::Registration> registration, Vec<Css::CounterIncrement> value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues const&, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.counters.cow().set = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
