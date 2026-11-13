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
    static constexpr auto ID = LonghandId::WRITING_MODE;
    static inline Flags<Options> const FLAGS = {INHERITED};

    static Rc<Property> initial() {
        return makeRc<WritingModeProperty>(Keywords::HORIZONTAL_TB);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<WritingModeProperty>(c.writingMode);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<WritingModeProperty>(try$(parseValue<WritingMode>(c))));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.writingMode = parent.writingMode;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::WRITING_MODE, FLAGS) {}
    };

    WritingMode _value;

    WritingModeProperty(WritingMode value)
        : Property(registrationOf<WritingModeProperty>()), _value(value) {}

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
    static constexpr auto ID = LonghandId::DIRECTION;
    static inline Flags<Options> const FLAGS = {INHERITED, ALL_EXCLUDED};

    static Rc<Property> initial() {
        return makeRc<DirectionProperty>(Keywords::LTR);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<DirectionProperty>(c.direction);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<DirectionProperty>(try$(parseValue<Direction>(c))));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.direction = parent.direction;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::DIRECTION, FLAGS) {}
    };

    Direction _value;

    DirectionProperty(Direction value)
        : Property(registrationOf<DirectionProperty>()), _value(value) {}

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
