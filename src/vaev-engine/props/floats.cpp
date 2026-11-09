module;

#include <karm/macros>

export module Vaev.Engine:props.floats;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// MARK: Float & Clear ---------------------------------------------------------

// https://drafts.csswg.org/css-page-floats-3/#propdef-float
export struct FloatProperty : Property {
    using Value = Css::Float;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::FLOAT) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FloatProperty>(self(), c.float_);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FloatProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    FloatProperty(Rc<Property::Registration> registration, Value value)
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

// https://drafts.csswg.org/css-page-floats-3/#propdef-clear
export struct ClearProperty : Property {
    using Value = Css::Clear;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::CLEAR) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<ClearProperty>(self(), c.clear);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ClearProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    ClearProperty(Rc<Property::Registration> registration, Value value)
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
