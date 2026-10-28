module;

#include <karm/macros>

export module Vaev.Engine:props.writing;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-writing-modes-4/#propdef-writing-mode
export struct WritingModeProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::WRITING_MODE, INHERITED) {}

        Rc<Property> initial() const override {
            return makeRc<WritingModeProperty>(self(), Keywords::HORIZONTAL_TB);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.writingMode = parent.writingMode;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<WritingModeProperty>(self(), c.writingMode);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<WritingModeProperty>(self(), try$(parseValue<WritingMode>(c))));
        }
    };

    WritingMode _value;

    WritingModeProperty(Rc<Property::Registration> registration, WritingMode value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.writingMode = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-writing-modes-4/#propdef-direction
export struct DirectionProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::DIRECTION, {INHERITED, ALL_EXCLUDED}) {}

        Rc<Property> initial() const override {
            return makeRc<DirectionProperty>(self(), Keywords::LTR);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.direction = parent.direction;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<DirectionProperty>(self(), c.direction);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<DirectionProperty>(self(), try$(parseValue<Direction>(c))));
        }
    };

    Direction _value;

    DirectionProperty(Rc<Property::Registration> registration, Direction value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.direction = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
