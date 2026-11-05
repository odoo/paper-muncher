module;

#include <karm/macros>

export module Vaev.Engine:props.display;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// https://www.w3.org/TR/CSS22/visuren.html#propdef-display
export struct DisplayProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::DISPLAY) {}

        Rc<Property> initial() const override {
            return makeRc<DisplayProperty>(self(), Css::Display{Css::Display::FLOW, Css::Display::INLINE});
        }

        Rc<Property> load(ComputedValues const& s) const override {
            return makeRc<DisplayProperty>(self(), s.display);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<DisplayProperty>(self(), try$(parseValue<Css::Display>(c))));
        }
    };

    Css::Display _value;

    DisplayProperty(Rc<Property::Registration> registration, Css::Display value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.display = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-gcpm-3/
// https://drafts.csswg.org/css-content/#content-property
export struct ContentProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::CONTENT) {}

        Rc<Property> initial() const override {
            return makeRc<ContentProperty>(self(), Keywords::NORMAL);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ContentProperty>(self(), c.content);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ContentProperty>(self(), try$(parseValue<Css::ContentValue>(c))));
        }
    };

    Css::ContentValue _value;

    ContentProperty(Rc<Property::Registration> registration, Css::ContentValue value)
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
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::ORDER) {}

        Rc<Property> initial() const override {
            return makeRc<OrderProperty>(self(), Css::Integer{0});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OrderProperty>(self(), c.order);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OrderProperty>(self(), try$(parseValue<Css::Integer>(c))));
        }
    };

    Css::Integer _value;

    OrderProperty(Rc<Property::Registration> registration, Css::Integer value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.order = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
