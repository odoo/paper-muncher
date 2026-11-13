module;

#include <karm/macros>

export module Vaev.Engine:props.background;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :values.base;
import :values.primitives;
import :values.background;

using namespace Karm;

namespace Vaev::Style {

// MARK: Background Color ------------------------------------------------------

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-color
export struct BackgroundColorProperty : Property {
    static constexpr auto ID = LonghandId::BACKGROUND_COLOR;

    static Rc<Property> initial() {
        return makeRc<BackgroundColorProperty>(TRANSPARENT);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BackgroundColorProperty>(c.backgrounds->color);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BackgroundColorProperty>(try$(parseValue<Color>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BACKGROUND_COLOR) {}
    };

    Color _value;

    BackgroundColorProperty(Color value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.backgrounds.cow().color = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Background Image ------------------------------------------------------

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-attachment
export struct BackgroundAttachmentProperty : Property {
    static constexpr auto ID = LonghandId::BACKGROUND_ATTACHMENT;

    static Rc<Property> initial() {
        return makeRc<BackgroundAttachmentProperty>(Vec<BackgroundAttachment>{Keywords::SCROLL});
    }

    static Rc<Property> load(ComputedValues const& c) {
        Vec<BackgroundAttachment> layers;
        for (auto const& l : c.backgrounds->layers)
            layers.pushBack(l.attachment);
        return makeRc<BackgroundAttachmentProperty>(std::move(layers));
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {

        Vec<BackgroundAttachment> value;
        while (not c.ended()) {
            value.pushBack(try$(parseValue<BackgroundAttachment>(c)));
            eatWhitespace(c);
        }

        return Ok(makeRc<BackgroundAttachmentProperty>(std::move(value)));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BACKGROUND_ATTACHMENT) {}
    };

    Vec<BackgroundAttachment> _value;

    BackgroundAttachmentProperty(Vec<BackgroundAttachment> value)
        : Property(ID), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        auto& layers = c.backgrounds.cow().layers;
        layers.resize(max(layers.len(), _value.len()));
        for (usize i = 0; i < _value.len(); ++i)
            layers[i].attachment = _value[i];
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-image
export struct BackgroundImageProperty : Property {
    static constexpr auto ID = LonghandId::BACKGROUND_IMAGE;

    static Rc<Property> initial() {
        return makeRc<BackgroundImageProperty>(Vec<Image>{});
    }

    static Rc<Property> load(ComputedValues const&) {
        return makeRc<BackgroundImageProperty>(Vec<Image>{});
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>&, ParsingMode) {
        // TODO
        return Ok(makeRc<BackgroundImageProperty>(Vec<Image>{}));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BACKGROUND_IMAGE) {}
    };

    Vec<Image> _value;

    BackgroundImageProperty(Vec<Image> value)
        : Property(ID), _value(std::move(value)) {}

    void apply(ComputedValues const&, ComputedValues&, [[maybe_unused]] ComputationContext const& cx) const override {
        // TODO
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-position
export struct BackgroundPositionProperty : Property {
    static constexpr auto ID = LonghandId::BACKGROUND_POSITION;

    static Rc<Property> initial() {
        return makeRc<BackgroundPositionProperty>(Vec<BackgroundPosition>{});
    }

    static Rc<Property> load(ComputedValues const&) {
        return makeRc<BackgroundPositionProperty>(Vec<BackgroundPosition>{});
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>&, ParsingMode) {
        // TODO
        return Ok(makeRc<BackgroundPositionProperty>(Vec<BackgroundPosition>{}));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BACKGROUND_POSITION) {}
    };

    Vec<BackgroundPosition> _value;

    BackgroundPositionProperty(Vec<BackgroundPosition> value)
        : Property(ID), _value(std::move(value)) {}

    void apply(ComputedValues const&, ComputedValues&, [[maybe_unused]] ComputationContext const& cx) const override {
        // TODO
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/colors.html#propdef-background-repeat
export struct BackgroundRepeatProperty : Property {
    static constexpr auto ID = LonghandId::BACKGROUND_REPEAT;

    static Rc<Property> initial() {
        return makeRc<BackgroundRepeatProperty>(Vec<BackgroundRepeat>{BackgroundRepeat::REPEAT});
    }

    static Rc<Property> load(ComputedValues const&) {
        // TODO
        return makeRc<BackgroundRepeatProperty>(Vec<BackgroundRepeat>{BackgroundRepeat::REPEAT});
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>&, ParsingMode) {
        // TODO
        return Ok(makeRc<BackgroundRepeatProperty>(Vec<BackgroundRepeat>{}));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BACKGROUND_REPEAT) {}
    };

    Vec<BackgroundRepeat> _value;

    BackgroundRepeatProperty(Vec<BackgroundRepeat> value)
        : Property(ID),
          _value(std::move(value)) {}

    void apply(ComputedValues const&, ComputedValues&, [[maybe_unused]] ComputationContext const& cx) const override {
        // TODO
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// // https://www.w3.org/TR/CSS22/colors.html#x10
// export struct BackgroundProperty : Property {
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::BACKGROUND, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<BackgroundProperty>(self(), SpecifiedBackground{TRANSPARENT});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<BackgroundProperty>(self(), *c.backgrounds);
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             SpecifiedBackground value;
//             value.color = try$(parseValue<Color>(c));
//             return Ok(makeRc<BackgroundProperty>(self(), std::move(value)));
//         }
//     };
//
//     SpecifiedBackground _value;
//
//     BackgroundProperty(Rc<Property::Registration> registration, SpecifiedBackground value)
//         : Property(registration), _value(std::move(value)) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<BackgroundColorProperty>(
//                 registry.resolveRegistration(Properties::BACKGROUND_COLOR, {}).take(),
//                 _value.color
//             ),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

// https://www.w3.org/TR/CSS22/colors.html#propdef-color
export struct ColorProperty : Property {
    static constexpr auto ID = LonghandId::COLOR;
    static constexpr Flags<Options> FLAGS = {INHERITED};

    static Rc<Property> initial() {
        return makeRc<ColorProperty>(BLACK);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<ColorProperty>(c.color);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<ColorProperty>(try$(parseValue<Color>(c))));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.color = parent.color;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::COLOR, FLAGS) {}
    };

    Color _value;

    ColorProperty(Color value)
        : Property(ID), _value(value) {}

    void apply(ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        // In the color property, the used value of currentcolor is the resolved inherited value.
        c.color = resolve(_value, parent.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
