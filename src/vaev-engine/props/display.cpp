module;

#include <karm/macros>

export module Vaev.Engine:props.display;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// https://www.w3.org/TR/CSS22/visuren.html#propdef-display
export struct DisplayProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::DISPLAY;
        }

        Rc<Property> initial() const override {
            return makeRc<DisplayProperty>(self(), Display{Display::FLOW, Display::INLINE});
        }

        Rc<Property> load(SpecifiedValues const& s) const override {
            return makeRc<DisplayProperty>(self(), s.display);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<DisplayProperty>(self(), try$(parseValue<Display>(c))));
        }
    };

    Display _value;

    DisplayProperty(Rc<Property::Registration> registration, Display value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& s) const override {
        s.display = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-gcpm-3/
// https://drafts.csswg.org/css-content/#content-property
export struct ContentProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::CONTENT;
        }

        Rc<Property> initial() const override {
            return makeRc<ContentProperty>(self(), Keywords::NORMAL);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<ContentProperty>(self(), c.content);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ContentProperty>(self(), try$(parseValue<Content>(c))));
        }
    };

    Content _value;

    ContentProperty(Rc<Property::Registration> registration, Content value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.content = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-display-3/#order-property
export struct OrderProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::ORDER;
        }

        Rc<Property> initial() const override {
            return makeRc<OrderProperty>(self(), Integer{0});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<OrderProperty>(self(), c.order);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OrderProperty>(self(), try$(parseValue<Integer>(c))));
        }
    };

    Integer _value;

    OrderProperty(Rc<Property::Registration> registration, Integer value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.order = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
