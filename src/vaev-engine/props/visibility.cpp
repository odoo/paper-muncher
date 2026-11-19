module;

#include <karm/macros>

export module Vaev.Engine:props.visibility;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// https://drafts.csswg.org/css-display/#visibility
export struct VisibilityProperty : Property {
    using Value = Css::Visibility;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::VISIBILITY, INHERITED) {}

        InitialValue initialValue() const override {
            return "visible"s;
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.visibility = parent.visibility;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<VisibilityProperty>(self(), c.visibility);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<VisibilityProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    VisibilityProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.visibility = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-color-4/#propdef-opacity
export struct OpacityProperty : Property {
    using Value = Css::OpacityValue;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::OPACITY) {}

        InitialValue initialValue() const override {
            return "1"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OpacityProperty>(self(), specifyValue<Value>(c.opacity));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OpacityProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    OpacityProperty(Rc<Property::Registration> registration, Value const& value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.opacity = computeValue(_value);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-masking/#the-clip-path
export struct ClipPathProperty : Property {
    using Value = Css::ClipPath;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::CLIP_PATH) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            if (auto const& [clip] = *c.clip)
                return makeRc<ClipPathProperty>(self(), specifyValue<Css::BasicShape>(clip));
            return makeRc<ClipPathProperty>(self(), Keywords::NONE);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<ClipPathProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    ClipPathProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        if (auto clipShape = _value.is<Css::BasicShape>())
            c.clip.cow() = Some(computeValue(*clipShape, cx));
        else
            c.clip.cow() = NONE;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
