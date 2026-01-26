module;

#include <karm/macros>

export module Vaev.Engine:props.text;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

using namespace Karm;

namespace Vaev::Style {

// MARK: Text
// https://drafts.csswg.org/css-text-4

// https://drafts.csswg.org/css-text/#text-align-property

export struct TextAlignProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::TEXT_ALIGN;
        }

        Flags<Options> flags() const override {
            return {INHERITED};
        }

        Rc<Property> initial() const override {
            return makeRc<TextAlignProperty>(self(), TextAlign::LEFT);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<TextAlignProperty>(self(), c.text->align);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
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
            } else {
                return Error::invalidData("expected text-align");
            }
            return Ok(makeRc<TextAlignProperty>(self(), value));
        }
    };

    TextAlign _value;

    TextAlignProperty(Rc<Property::Registration> registration, TextAlign value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.text.cow().align = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-text-4/#text-transform-property

export struct TextTransformProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::TEXT_TRANSFORM;
        }

        Flags<Options> flags() const override {
            return {INHERITED};
        }

        Rc<Property> initial() const override {
            return makeRc<TextTransformProperty>(self(), TextTransform::NONE);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<TextTransformProperty>(self(), c.text->transform);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            TextTransform value;
            if (c.skip(Css::Token::ident("none"))) {
                value = TextTransform::NONE;
            } else if (c.skip(Css::Token::ident("uppercase"))) {
                value = TextTransform::UPPERCASE;
            } else if (c.skip(Css::Token::ident("lowsercase"))) {
                value = TextTransform::LOWERCASE;
            } else {
                return Error::invalidData("expected text-transform");
            }

            return Ok(makeRc<TextTransformProperty>(self(), value));
        }
    };

    TextTransform _value;

    TextTransformProperty(Rc<Property::Registration> registration, TextTransform value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.text.cow().transform = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-text/#white-space-property

export struct WhiteSpaceProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::WHITE_SPACE;
        }

        Flags<Options> flags() const override {
            return {INHERITED};
        }

        Rc<Property> initial() const override {
            return makeRc<WhiteSpaceProperty>(self(), WhiteSpace::NORMAL);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<WhiteSpaceProperty>(self(), c.text->whiteSpace);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
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

            return Ok(makeRc<WhiteSpaceProperty>(self(), value));
        }
    };

    WhiteSpace _value;

    WhiteSpaceProperty(Rc<Property::Registration> registration, WhiteSpace value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.text.cow().whiteSpace = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
