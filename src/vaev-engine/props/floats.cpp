module;

#include <karm/macros>

export module Vaev.Engine:props.floats;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;

namespace Vaev::Style {

// MARK: Float & Clear ---------------------------------------------------------

export struct FloatProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::FLOAT) {}

        Rc<Property> initial() const override {
            return makeRc<FloatProperty>(self(), Keywords::NONE);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FloatProperty>(self(), c.float_);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FloatProperty>(self(), try$(parseValue<Css::Float>(c))));
        }
    };

    Css::Float _value;

    FloatProperty(Rc<Property::Registration> registration, Css::Float value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.float_ = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct ClearProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::CLEAR) {}

        Rc<Property> initial() const override {
            return makeRc<ClearProperty>(self(), Keywords::NONE);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ClearProperty>(self(), c.clear);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ClearProperty>(self(), try$(parseValue<Css::Clear>(c))));
        }
    };

    Css::Clear _value;

    ClearProperty(Rc<Property::Registration> registration, Css::Clear value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.clear = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
