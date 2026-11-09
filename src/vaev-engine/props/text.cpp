module;

#include <karm/macros>

export module Vaev.Engine:props.text;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;

namespace Vaev::Style {

// MARK: Text
// https://drafts.csswg.org/css-text-4

// https://drafts.csswg.org/css-text/#text-align-property

export struct TextAlignProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::TEXT_ALIGN, {INHERITED, BULK_INHERITED}) {}

        Rc<Property> initial() const override {
            return makeRc<TextAlignProperty>(self(), Keywords::LEFT);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.text.cow().align = parent.text->align;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<TextAlignProperty>(self(), c.text->align);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            // NOSPEC: Legacy aliases of -vaev-block-center.
            if (c.skip(Css::Token::ident("-webkit-center")) or c.skip(Css::Token::ident("-moz-center")))
                return Ok(makeRc<TextAlignProperty>(self(), Keywords::VAEV_BLOCK_CENTER));

            return Ok(makeRc<TextAlignProperty>(self(), try$(parseValue<Css::TextAlign>(c))));
        }
    };

    Css::TextAlign _value;

    TextAlignProperty(Rc<Property::Registration> registration, Css::TextAlign value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.text.cow().align = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        // NOSPEC: Non-standard value, written with the name res/html.css uses.
        if (_value == Keywords::VAEV_BLOCK_CENTER)
            return w.writeStr("-vaev-block-center"s);
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-text-4/#text-transform-property

export struct TextTransformProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::TEXT_TRANSFORM, {INHERITED, BULK_INHERITED}) {}

        Rc<Property> initial() const override {
            return makeRc<TextTransformProperty>(self(), Keywords::NONE);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.text.cow().transform = parent.text->transform;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<TextTransformProperty>(self(), c.text->transform);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            return Ok(makeRc<TextTransformProperty>(self(), try$(parseValue<Css::TextTransform>(c))));
        }
    };

    Css::TextTransform _value;

    TextTransformProperty(Rc<Property::Registration> registration, Css::TextTransform value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.text.cow().transform = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-text/#white-space-property

export struct WhiteSpaceProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::WHITE_SPACE, {INHERITED, BULK_INHERITED}) {}

        Rc<Property> initial() const override {
            return makeRc<WhiteSpaceProperty>(self(), Keywords::NORMAL);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.text.cow().whiteSpace = parent.text->whiteSpace;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<WhiteSpaceProperty>(self(), c.text->whiteSpace);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<WhiteSpaceProperty>(self(), try$(parseValue<Css::WhiteSpace>(c))));
        }
    };

    Css::WhiteSpace _value;

    WhiteSpaceProperty(Rc<Property::Registration> registration, Css::WhiteSpace value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.text.cow().whiteSpace = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
