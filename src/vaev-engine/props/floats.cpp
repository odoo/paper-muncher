module;

#include <karm/macros>

export module Vaev.Engine:props.floats;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Float & Clear ---------------------------------------------------------

export struct FloatProperty : Property {
    static constexpr auto ID = LonghandId::FLOAT;

    static Rc<Property> initial() {
        return makeRc<FloatProperty>(Float::NONE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FloatProperty>(c.float_);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<FloatProperty>(try$(parseValue<Float>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::FLOAT) {}
    };

    Float _value;

    FloatProperty(Float value)
        : Property(registrationOf<FloatProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.float_ = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct ClearProperty : Property {
    static constexpr auto ID = LonghandId::CLEAR;

    static Rc<Property> initial() {
        return makeRc<ClearProperty>(Clear::NONE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<ClearProperty>(c.clear);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<ClearProperty>(try$(parseValue<Clear>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::CLEAR) {}
    };

    Clear _value;

    ClearProperty(Clear value)
        : Property(registrationOf<ClearProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.clear = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
