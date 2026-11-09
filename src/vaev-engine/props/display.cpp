module;

#include <karm/macros>

export module Vaev.Engine:props.display;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// https://drafts.csswg.org/css-display/#the-display-properties
export struct DisplayProperty : Property {
    using Value = Css::Display;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::DISPLAY) {}

        InitialValue initialValue() const override {
            return "inline"s;
        }

        Rc<Property> load(ComputedValues const& s) const override {
            return makeRc<DisplayProperty>(self(), s.display);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<DisplayProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    DisplayProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.display = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-gcpm-3/
// https://drafts.csswg.org/css-content/#content-property
export struct ContentProperty : Property {
    using Value = Css::ContentValue;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::CONTENT) {}

        InitialValue initialValue() const override {
            return "normal"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ContentProperty>(self(), c.content);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ContentProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    ContentProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.content = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-display-3/#order-property
export struct OrderProperty : Property {
    using Value = Integer;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::ORDER) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OrderProperty>(self(), c.order);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OrderProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    OrderProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.order = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
