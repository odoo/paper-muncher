module;

#include <karm/macros>

export module Vaev.Engine:props.overflow;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Overflow --------------------------------------------------------------

// https://www.w3.org/TR/css-overflow/#overflow-control
export struct OverflowXProperty : Property {
    static constexpr auto ID = LonghandId::OVERFLOW_X;

    static Rc<Property> initial() {
        return makeRc<OverflowXProperty>(Overflow::VISIBLE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OverflowXProperty>(c.overflows.x);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<OverflowXProperty>(try$(parseValue<Overflow>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::OVERFLOW_X) {}
    };

    Overflow _value;

    OverflowXProperty(Overflow value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.overflows.x = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-control
export struct OverflowYProperty : Property {
    static constexpr auto ID = LonghandId::OVERFLOW_Y;

    static Rc<Property> initial() {
        return makeRc<OverflowYProperty>(Overflow::VISIBLE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OverflowYProperty>(c.overflows.y);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<OverflowYProperty>(try$(parseValue<Overflow>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::OVERFLOW_Y) {}
    };

    Overflow _value;

    OverflowYProperty(Overflow value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.overflows.y = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-block
export struct OverflowBlockProperty : Property {
    static constexpr auto ID = LonghandId::OVERFLOW_BLOCK;

    static Rc<Property> initial() {
        return makeRc<OverflowBlockProperty>(Overflow::VISIBLE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OverflowBlockProperty>(c.overflows.block);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<OverflowBlockProperty>(try$(parseValue<Overflow>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::OVERFLOW_BLOCK) {}
    };

    Overflow _value;

    OverflowBlockProperty(Overflow value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.overflows.block = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-overflow/#overflow-inline
export struct OverflowInlineProperty : Property {
    static constexpr auto ID = LonghandId::OVERFLOW_INLINE;

    static Rc<Property> initial() {
        return makeRc<OverflowInlineProperty>(Overflow::VISIBLE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OverflowInlineProperty>(c.overflows.inline_);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<OverflowInlineProperty>(try$(parseValue<Overflow>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::OVERFLOW_INLINE) {}
    };

    Overflow _value;

    OverflowInlineProperty(Overflow value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.overflows.inline_ = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// // https://www.w3.org/TR/css-overflow-3/#propdef-overflow
// export struct OverflowProperty : Property {
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::OVERFLOW, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<OverflowProperty>(self(), Pair{Overflow::VISIBLE, Overflow::VISIBLE});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<OverflowProperty>(self(), Pair{c.overflows.x, c.overflows.y});
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             eatWhitespace(c);
//             if (c.ended())
//                 return Error::invalidData("unexpected end of input");
//
//             Pair<Overflow> value;
//             value.v0 = try$(parseValue<Overflow>(c));
//
//             eatWhitespace(c);
//             if (c.ended()) {
//                 value.v1 = value.v0;
//             } else {
//                 value.v1 = try$(parseValue<Overflow>(c));
//             }
//
//             return Ok(makeRc<OverflowProperty>(self(), value));
//         }
//     };
//
//     Pair<Overflow> _value;
//
//     OverflowProperty(Rc<Property::Registration> registration, Pair<Overflow> value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<OverflowXProperty>(registry.resolveRegistration(Properties::OVERFLOW_X, {}).expect(), _value.v0),
//             makeRc<OverflowYProperty>(registry.resolveRegistration(Properties::OVERFLOW_Y, {}).expect(), _value.v1),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{} {}", _value.v0, _value.v1);
//     }
// };

} // namespace Vaev::Style
