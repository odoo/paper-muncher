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

// https://drafts.csswg.org/css-backgrounds-3/#background
export struct BackgroundShorthand {
    static constexpr auto ID = ShorthandId::BACKGROUND;

    // FIXME: background-size, background-origin and background-clip are not implemented yet.
    static constexpr auto LONGHANDS = Array{
        LonghandId::BACKGROUND_IMAGE,
        LonghandId::BACKGROUND_POSITION,
        // LonghandId::BACKGROUND_SIZE,
        LonghandId::BACKGROUND_REPEAT,
        // LonghandId::BACKGROUND_ORIGIN,
        // LonghandId::BACKGROUND_CLIP,
        LonghandId::BACKGROUND_ATTACHMENT,
        LonghandId::BACKGROUND_COLOR,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        auto color = try$(parseValue<Color>(c));
        stylesheet.pushBack(makeRc<BackgroundColorProperty>(color));

        // TODO
        stylesheet.pushBack(initialProperty(LonghandId::BACKGROUND_IMAGE));
        stylesheet.pushBack(initialProperty(LonghandId::BACKGROUND_POSITION));
        // stylesheet.pushBack(initialProperty(LonghandId::BACKGROUND_SIZE));
        stylesheet.pushBack(initialProperty(LonghandId::BACKGROUND_REPEAT));
        // stylesheet.pushBack(initialProperty(LonghandId::BACKGROUND_ORIGIN));
        // stylesheet.pushBack(initialProperty(LonghandId::BACKGROUND_CLIP));
        stylesheet.pushBack(initialProperty(LonghandId::BACKGROUND_ATTACHMENT));

        return Ok();
    }
};

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
