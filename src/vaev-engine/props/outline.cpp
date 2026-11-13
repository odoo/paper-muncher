module;

#include <karm/macros>

export module Vaev.Engine:props.outline;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Outline --------------------------------------------------------------

// https://drafts.csswg.org/css-ui/#outline-width
export struct OutlineWidthProperty : Property {
    static constexpr auto ID = LonghandId::OUTLINE_WIDTH;

    static Rc<Property> initial() {
        return makeRc<OutlineWidthProperty>(LineWidth{Keywords::MEDIUM});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OutlineWidthProperty>(c.outline->width);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<OutlineWidthProperty>(try$(parseValue<LineWidth>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::OUTLINE_WIDTH) {}
    };

    LineWidth _value;

    OutlineWidthProperty(LineWidth value)
        : Property(registrationOf<OutlineWidthProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.outline.cow().width = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-style
export struct OutlineStyleProperty : Property {
    static constexpr auto ID = LonghandId::OUTLINE_STYLE;

    using Value = Union<Keywords::Auto, Gfx::BorderStyle>;

    static Rc<Property> initial() {
        return makeRc<OutlineStyleProperty>(Value{Gfx::BorderStyle::NONE});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OutlineStyleProperty>(c.outline->style);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<OutlineStyleProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::OUTLINE_STYLE) {}
    };

    Value _value;

    OutlineStyleProperty(Value value)
        : Property(registrationOf<OutlineStyleProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.outline.cow().style = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-color
export struct OutlineColorProperty : Property {
    static constexpr auto ID = LonghandId::OUTLINE_COLOR;
    // Must be applied after 'outline-style'.
    static constexpr auto PHASE = ComputationPhase::LATE;

    using Value = Union<Keywords::Auto, Color>;

    static Rc<Property> initial() {
        return makeRc<OutlineColorProperty>(Value{Keywords::AUTO});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OutlineColorProperty>(Color{c.outline->color});
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<OutlineColorProperty>(try$(parseValue<Value>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::OUTLINE_COLOR) {}
    };

    Value _value;

    OutlineColorProperty(Value value)
        : Property(registrationOf<OutlineColorProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.outline.cow().color = _value.visit(
            // https://drafts.csswg.org/css-ui/#valdef-outline-color-auto
            [&](Keywords::Auto) {
                // When outline-style is auto, outline-color: auto computes to auto and represents the accent color.
                // Otherwise, outline-color: auto computes to currentColor.
                return c.outline->style == Keywords::AUTO
                           ? resolve(Color{SystemColor::ACCENT_COLOR}, c.color)
                           : c.color;
            },
            [&](Color color) {
                return resolve(color, c.color);
            }
        );
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-offset
export struct OutlineOffsetProperty : Property {
    static constexpr auto ID = LonghandId::OUTLINE_OFFSET;

    static Rc<Property> initial() {
        return makeRc<OutlineOffsetProperty>(Calc<Length>{0_au});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<OutlineOffsetProperty>(c.outline->offset);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<OutlineOffsetProperty>(try$(parseValue<Calc<Length>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::OUTLINE_OFFSET) {}
    };

    Calc<Length> _value;

    OutlineOffsetProperty(Calc<Length> value)
        : Property(registrationOf<OutlineOffsetProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.outline.cow().offset = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// // https://drafts.csswg.org/css-ui/#outline
// export struct OutlineProperty : Property {
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::OUTLINE, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<OutlineProperty>(self(), SpecifiedOutline{});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<OutlineProperty>(self(), *c.outline);
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             SpecifiedOutline value;
//             bool styleSet = false;
//             while (not c.ended()) {
//                 auto width = parseValue<Calc<Length>>(c);
//                 if (width) {
//                     value.width = width.expect();
//                     continue;
//                 }
//
//                 auto color = parseValue<Color>(c);
//                 if (color) {
//                     value.color = color.expect();
//                     continue;
//                 }
//
//                 auto style = parseValue<Gfx::BorderStyle>(c);
//                 if (style) {
//                     value.style = style.expect();
//                     styleSet = true;
//                     continue;
//                 }
//
//                 if (c.skip(Css::Token::ident("auto"))) {
//                     if (not styleSet)
//                         value.style = Keywords::AUTO;
//                     value.color = Keywords::AUTO;
//                     continue;
//                 }
//
//                 break;
//             }
//
//             return Ok(makeRc<OutlineProperty>(self(), value));
//         }
//     };
//
//     SpecifiedOutline _value;
//
//     OutlineProperty(Rc<Property::Registration> registration, SpecifiedOutline value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<OutlineWidthProperty>(registry.resolveRegistration(Properties::OUTLINE_WIDTH, {}).expect(), _value.width),
//             makeRc<OutlineStyleProperty>(registry.resolveRegistration(Properties::OUTLINE_STYLE, {}).expect(), _value.style),
//             makeRc<OutlineColorProperty>(registry.resolveRegistration(Properties::OUTLINE_COLOR, {}).expect(), _value.color),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

} // namespace Vaev::Style
