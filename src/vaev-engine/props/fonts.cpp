module;

#include <karm/macros>

export module Vaev.Engine:props.fonts;

import Karm.Core;
import :props.base;
import :props.baseline;
import :css.parser;
import :style.computed;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// MARK: Fonts -----------------------------------------------------------------

// https://www.w3.org/TR/css-fonts-4/#font-family-prop
export struct FontFamilyProperty : Property {
    static constexpr auto ID = LonghandId::FONT_FAMILY;
    static inline Flags<Options> const FLAGS = {INHERITED, BULK_INHERITED};
    static constexpr auto PHASE = ComputationPhase::FONT;

    static Rc<Property> initial() {
        return makeRc<FontFamilyProperty>(Vec<FontFamily>{"sans-serif"_sym});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FontFamilyProperty>(c.font->families);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        Vec<FontFamily> value{};
        eatWhitespace(c);
        while (not c.ended()) {
            value.pushBack(try$(parseValue<FontFamily>(c)));

            eatWhitespace(c);
            c.skip(Css::Token::COMMA);
            eatWhitespace(c);
        }
        return Ok(makeRc<FontFamilyProperty>(std::move(value)));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.font.cow().families = parent.font->families;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::FONT_FAMILY, FLAGS) {}
    };

    Vec<FontFamily> _value;

    FontFamilyProperty(Vec<FontFamily> value)
        : Property(registrationOf<FontFamilyProperty>()), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().families = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        bool first = true;

        for (auto const& family : _value) {
            if (first) {
                first = false;
            } else {
                try$(w.writeStr(", "s));
            }

            try$(w.writeStr(family.name.str()));
        }

        return Ok();
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-weight-prop
export struct FontWeightProperty : Property {
    static constexpr auto ID = LonghandId::FONT_WEIGHT;
    static inline Flags<Options> const FLAGS = {INHERITED, BULK_INHERITED};
    static constexpr auto PHASE = ComputationPhase::FONT;

    static Rc<Property> initial() {
        return makeRc<FontWeightProperty>(FontWeight{Gfx::FontWeight::REGULAR});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FontWeightProperty>(c.font->weight);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<FontWeightProperty>(try$(parseValue<FontWeight>(c))));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.font.cow().weight = parent.font->weight;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::FONT_WEIGHT, FLAGS) {}
    };

    FontWeight _value;

    FontWeightProperty(FontWeight value)
        : Property(registrationOf<FontWeightProperty>()), _value(value) {}

    void apply(ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().weight = _value.resolve(parent.font->weight);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-width-prop
export struct FontWidthProperty : Property {
    static constexpr auto ID = LonghandId::FONT_WIDTH;
    static inline Flags<Options> const FLAGS = {INHERITED, BULK_INHERITED};
    // https://drafts.csswg.org/css-fonts/#font-stretch-prop
    static constexpr Array LEGACY_NAME_ALIASES = {"font-stretch"s};
    static constexpr auto PHASE = ComputationPhase::FONT;

    static Rc<Property> initial() {
        return makeRc<FontWidthProperty>(FontWidth::NORMAL);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FontWidthProperty>(c.font->width);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<FontWidthProperty>(try$(parseValue<FontWidth>(c))));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.font.cow().width = parent.font->width;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::FONT_WIDTH, FLAGS) {}
    };

    FontWidth _value;

    FontWidthProperty(FontWidth value)
        : Property(registrationOf<FontWidthProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().width = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-style-prop
export struct FontStyleProperty : Property {
    static constexpr auto ID = LonghandId::FONT_STYLE;
    static inline Flags<Options> const FLAGS = {INHERITED, BULK_INHERITED};
    static constexpr auto PHASE = ComputationPhase::FONT;

    static Rc<Property> initial() {
        return makeRc<FontStyleProperty>(FontStyle::NORMAL);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FontStyleProperty>(c.font->style);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<FontStyleProperty>(try$(parseValue<FontStyle>(c))));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.font.cow().style = parent.font->style;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::FONT_STYLE, FLAGS) {}
    };

    FontStyle _value;

    FontStyleProperty(FontStyle value)
        : Property(registrationOf<FontStyleProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().style = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-size-prop
export struct FontSizeProperty : Property {
    static constexpr auto ID = LonghandId::FONT_SIZE;
    static inline Flags<Options> const FLAGS = {INHERITED, BULK_INHERITED};
    static constexpr auto PHASE = ComputationPhase::FONT;

    static Rc<Property> initial() {
        return makeRc<FontSizeProperty>(Keywords::MEDIUM);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<FontSizeProperty>(Length{c.font->size});
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        return Ok(makeRc<FontSizeProperty>(try$(parseValue<FontSize>(c))));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.font.cow().size = parent.font->size;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::FONT_SIZE, FLAGS) {}
    };

    FontSize _value;

    FontSizeProperty(FontSize value)
        : Property(registrationOf<FontSizeProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().size = resolve(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// // https://www.w3.org/TR/css-fonts-4/#font-prop
// export struct FontProperty : Property {
//     struct Value {
//         Vec<FontFamily> families = {"sans-serif"_sym};
//         Opt<FontWeight> weight;
//         FontWidth width = FontWidth::NORMAL;
//         FontStyle style = FontStyle::NORMAL;
//         FontSize size = Keywords::MEDIUM;
//         LineHeight lineHeight = Keywords::NORMAL;
//
//         void repr(Io::Emit& e) const {
//             e("(font");
//             e(" families={}", families);
//             e(" weight={}", weight);
//             e(" width={}", width);
//             e(" style={}", style);
//             e(" size={}", size);
//             e(")");
//         }
//     };
//
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::FONT, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<FontProperty>(self(), Value{});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             auto const& font = *c.font;
//             return makeRc<FontProperty>(
//                 self(),
//                 Value{
//                     font.families,
//                     Some(font.weight),
//                     font.width,
//                     font.style,
//                     Length{font.size},
//                 }
//             );
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             // TODO: system family name
//             FontProperty::Value value;
//
//             while (true) {
//                 auto fontStyle = parseValue<FontStyle>(c);
//                 if (fontStyle) {
//                     value.style = fontStyle.expect();
//                     continue;
//                 }
//
//                 auto fontWeight = parseValue<FontWeight>(c);
//                 if (fontWeight) {
//                     value.weight = Some(fontWeight.expect());
//                     continue;
//                 }
//
//                 // TODO: font variant https://www.w3.org/TR/css-fonts-4/#font-variant-css21-values
//
//                 auto fontWidth = parseValue<FontWidth>(c);
//                 if (fontWidth) {
//                     value.width = fontWidth.expect();
//                     continue;
//                 }
//
//                 auto fontSize = parseValue<FontSize>(c);
//                 if (fontSize) {
//                     value.size = fontSize.expect();
//                     break;
//                 }
//
//                 return Error::invalidData("expected font-style, font-weight, font-width or font-size");
//             }
//
//             if (c.skip(Css::Token::delim("/")))
//                 value.lineHeight = try$(parseValue<LineHeight>(c));
//
//             value.families = {try$(parseValue<FontFamily>(c))};
//
//             return Ok(makeRc<FontProperty>(self(), std::move(value)));
//         }
//     };
//
//     Value _value;
//
//     FontProperty(Rc<Property::Registration> registration, Value value)
//         : Property(registration), _value(std::move(value)) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         Longhands result;
//         result.pushBack(makeRc<FontStyleProperty>(registry.resolveRegistration(Properties::FONT_STYLE, {}).expect(), _value.style));
//         result.pushBack(makeRc<FontWidthProperty>(registry.resolveRegistration(Properties::FONT_WIDTH, {}).expect(), _value.width));
//         result.pushBack(makeRc<FontSizeProperty>(registry.resolveRegistration(Properties::FONT_SIZE, {}).expect(), _value.size));
//         result.pushBack(makeRc<FontFamilyProperty>(registry.resolveRegistration(Properties::FONT_FAMILY, {}).expect(), _value.families));
//         result.pushBack(makeRc<LineHeightProperty>(registry.resolveRegistration(Properties::LINE_HEIGHT, {}).expect(), _value.lineHeight));
//         if (_value.weight)
//             result.pushBack(makeRc<FontWeightProperty>(registry.resolveRegistration(Properties::FONT_WEIGHT, {}).expect(), *_value.weight));
//         return result;
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

} // namespace Vaev::Style
