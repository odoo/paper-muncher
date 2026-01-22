module;

#include <karm-core/macros.h>

export module Vaev.Engine:props.floats;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// MARK: Float & Clear ---------------------------------------------------------

export struct FloatProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FLOAT;
        }

        Rc<Property> initial() const override {
            return makeRc<FloatProperty>(self(), Float::NONE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FloatProperty>(self(), c.float_);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FloatProperty>(self(), try$(parseValue<Float>(c))));
        }
    };

    Float _value;

    FloatProperty(Rc<Property::Registration> registration, Float value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.float_ = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct ClearProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::CLEAR;
        }

        Rc<Property> initial() const override {
            return makeRc<ClearProperty>(self(), Clear::NONE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<ClearProperty>(self(), c.clear);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ClearProperty>(self(), try$(parseValue<Clear>(c))));
        }
    };

    Clear _value;

    ClearProperty(Rc<Property::Registration> registration, Clear value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.clear = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
