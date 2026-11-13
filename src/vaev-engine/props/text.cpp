module;

#include <karm/macros>

export module Vaev.Engine:props.text;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Text
// https://drafts.csswg.org/css-text-4

// https://drafts.csswg.org/css-text/#text-align-property

export struct TextAlignProperty : Property {
    static constexpr auto ID = LonghandId::TEXT_ALIGN;
    static inline Flags<Options> const FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<TextAlignProperty>(TextAlign::LEFT);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<TextAlignProperty>(c.text->align);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        TextAlign value;
        if (c.skip(Css::Token::ident("left"))) {
            value = TextAlign::LEFT;
        } else if (c.skip(Css::Token::ident("right"))) {
            value = TextAlign::RIGHT;
        } else if (c.skip(Css::Token::ident("center"))) {
            value = TextAlign::CENTER;
        } else if (c.skip(Css::Token::ident("justify"))) {
            value = TextAlign::JUSTIFY;
        } else if (c.skip(Css::Token::ident("start"))) {
            value = TextAlign::START;
        } else if (c.skip(Css::Token::ident("end"))) {
            value = TextAlign::END;
        } else if (
            c.skip(Css::Token::ident("-vaev-block-center")) or
            c.skip(Css::Token::ident("-webkit-center")) or
            c.skip(Css::Token::ident("-moz-center"))
        ) {
            value = TextAlign::BLOCK_CENTER;
        } else {
            return Error::invalidData("expected text-align");
        }
        return Ok(makeRc<TextAlignProperty>(value));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.text.cow().align = parent.text->align;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::TEXT_ALIGN, FLAGS) {}
    };

    TextAlign _value;

    TextAlignProperty(TextAlign value)
        : Property(registrationOf<TextAlignProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.text.cow().align = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        // NOSPEC: Non-standard value, written with the name res/html.css uses.
        if (_value == TextAlign::BLOCK_CENTER)
            return w.writeStr("-vaev-block-center"s);
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-text-4/#text-transform-property

export struct TextTransformProperty : Property {
    static constexpr auto ID = LonghandId::TEXT_TRANSFORM;
    static inline Flags<Options> const FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<TextTransformProperty>(TextTransform::NONE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<TextTransformProperty>(c.text->transform);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        TextTransform value;
        if (c.skip(Css::Token::ident("none"))) {
            value = TextTransform::NONE;
        } else if (c.skip(Css::Token::ident("uppercase"))) {
            value = TextTransform::UPPERCASE;
        } else if (c.skip(Css::Token::ident("lowercase"))) {
            value = TextTransform::LOWERCASE;
        } else {
            return Error::invalidData("expected text-transform");
        }

        return Ok(makeRc<TextTransformProperty>(value));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.text.cow().transform = parent.text->transform;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::TEXT_TRANSFORM, FLAGS) {}
    };

    TextTransform _value;

    TextTransformProperty(TextTransform value)
        : Property(registrationOf<TextTransformProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.text.cow().transform = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-text/#white-space-property

export struct WhiteSpaceProperty : Property {
    static constexpr auto ID = LonghandId::WHITE_SPACE;
    static inline Flags<Options> const FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<WhiteSpaceProperty>(WhiteSpace::NORMAL);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<WhiteSpaceProperty>(c.text->whiteSpace);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c) {
        WhiteSpace value;
        if (c.skip(Css::Token::ident("normal"))) {
            value = WhiteSpace::NORMAL;
        } else if (c.skip(Css::Token::ident("nowrap"))) {
            value = WhiteSpace::NOWRAP;
        } else if (c.skip(Css::Token::ident("pre"))) {
            value = WhiteSpace::PRE;
        } else if (c.skip(Css::Token::ident("pre-wrap"))) {
            value = WhiteSpace::PRE_WRAP;
        } else if (c.skip(Css::Token::ident("pre-line"))) {
            value = WhiteSpace::PRE_LINE;
        } else if (c.skip(Css::Token::ident("break-spaces"))) {
            value = WhiteSpace::BREAK_SPACES;
        } else {
            return Error::invalidData("expected white-space");
        }

        return Ok(makeRc<WhiteSpaceProperty>(value));
    }

    static void inherit(ComputedValues const& parent, ComputedValues& child) {
        child.text.cow().whiteSpace = parent.text->whiteSpace;
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::WHITE_SPACE, FLAGS) {}
    };

    WhiteSpace _value;

    WhiteSpaceProperty(WhiteSpace value)
        : Property(registrationOf<WhiteSpaceProperty>()), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.text.cow().whiteSpace = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
