module;

#include <karm/macros>

export module Vaev.Engine:props.fonts;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Fonts -----------------------------------------------------------------

// https://www.w3.org/TR/css-fonts-4/#font-family-prop
export struct FontFamilyProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FONT_FAMILY;
        }

        Flags<Options> flags() const override {
            return {INHERITED, BULK_INHERITED};
        }

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
        }

        Rc<Property> initial() const override {
            return makeRc<FontFamilyProperty>(self(), Vec<FontFamily>{"sans-serif"_sym});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontFamilyProperty>(self(), c.inherited->fontFamilies);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Vec<FontFamily> value{};
            eatWhitespace(c);
            while (not c.ended()) {
                value.pushBack(try$(parseValue<FontFamily>(c)));

                eatWhitespace(c);
                c.skip(Css::Token::COMMA);
                eatWhitespace(c);
            }
            return Ok(makeRc<FontFamilyProperty>(self(), std::move(value)));
        }
    };

    Vec<FontFamily> _value;

    FontFamilyProperty(Rc<Property::Registration> registration, Vec<FontFamily> value)
        : Property(registration), _value(std::move(value)) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.inherited.cow().fontFamilies = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-weight-prop
export struct FontWeightProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FONT_WEIGHT;
        }

        Flags<Options> flags() const override {
            return {INHERITED, BULK_INHERITED};
        }

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
        }

        Rc<Property> initial() const override {
            return makeRc<FontWeightProperty>(self(), FontWeight{Gfx::FontWeight::REGULAR});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontWeightProperty>(self(), c.inherited->fontWeight);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontWeightProperty>(self(), try$(parseValue<FontWeight>(c))));
        }
    };

    FontWeight _value;

    FontWeightProperty(Rc<Property::Registration> registration, FontWeight value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.inherited.cow().fontWeight = _value.resolve(parent.inherited->fontWeight);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-width-prop
export struct FontWidthProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FONT_WIDTH;
        }

        // https://drafts.csswg.org/css-fonts/#font-stretch-prop
        Vec<Symbol> legacyAlias() const override {
            return {"font-stretch"_sym};
        }

        Flags<Options> flags() const override {
            return {INHERITED, BULK_INHERITED};
        }

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
        }

        Rc<Property> initial() const override {
            return makeRc<FontWidthProperty>(self(), FontWidth::NORMAL);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontWidthProperty>(self(), c.inherited->fontWidth);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontWidthProperty>(self(), try$(parseValue<FontWidth>(c))));
        }
    };

    FontWidth _value;

    FontWidthProperty(Rc<Property::Registration> registration, FontWidth value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.inherited.cow().fontWidth = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-style-prop
export struct FontStyleProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FONT_STYLE;
        }

        Flags<Options> flags() const override {
            return {INHERITED, BULK_INHERITED};
        }

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
        }

        Rc<Property> initial() const override {
            return makeRc<FontStyleProperty>(self(), FontStyle::NORMAL);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontStyleProperty>(self(), c.inherited->fontStyle);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontStyleProperty>(self(), try$(parseValue<FontStyle>(c))));
        }
    };

    FontStyle _value;

    FontStyleProperty(Rc<Property::Registration> registration, FontStyle value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.inherited.cow().fontStyle = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-size-prop
export struct FontSizeProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FONT_SIZE;
        }

        Flags<Options> flags() const override {
            return {INHERITED};
        }

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
        }

        Rc<Property> initial() const override {
            return makeRc<FontSizeProperty>(self(), Keywords::MEDIUM);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.fontSize = parent.fontSize;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontSizeProperty>(self(), Length{c.fontSize});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontSizeProperty>(self(), try$(parseValue<FontSize>(c))));
        }
    };

    FontSize _value;

    FontSizeProperty(Rc<Property::Registration> registration, FontSize value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.fontSize = resolve(_value, cx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-prop
export struct FontProperty : Property {
    struct Value {
        Vec<FontFamily> families = {"sans-serif"_sym};
        Opt<FontWeight> weight;
        FontWidth width = FontWidth::NORMAL;
        FontStyle style = FontStyle::NORMAL;
        FontSize size = Keywords::MEDIUM;

        void repr(Io::Emit& e) const {
            e("(font");
            e(" families={}", families);
            e(" weight={}", weight);
            e(" width={}", width);
            e(" style={}", style);
            e(" size={}", size);
            e(")");
        }
    };

    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FONT;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<FontProperty>(self(), Value{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            auto const& font = *c.inherited;
            return makeRc<FontProperty>(
                self(),
                Value{
                    font.fontFamilies,
                    Some(font.fontWeight),
                    font.fontWidth,
                    font.fontStyle,
                    Length{c.fontSize},
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            // TODO: system family name
            FontProperty::Value value;

            while (true) {
                auto fontStyle = parseValue<FontStyle>(c);
                if (fontStyle) {
                    value.style = fontStyle.unwrap();
                    continue;
                }

                auto fontWeight = parseValue<FontWeight>(c);
                if (fontWeight) {
                    value.weight = Some(fontWeight.unwrap());
                    continue;
                }

                // TODO: font variant https://www.w3.org/TR/css-fonts-4/#font-variant-css21-values

                auto fontWidth = parseValue<FontWidth>(c);
                if (fontWidth) {
                    value.width = fontWidth.unwrap();
                    continue;
                }

                auto fontSize = parseValue<FontSize>(c);
                if (fontSize) {
                    value.size = fontSize.unwrap();
                    break;
                }

                return Error::invalidData("expected font-style, font-weight, font-width or font-size");
            }

            if (c.skip(Css::Token::delim("/"))) {
                auto lh = Ok(parseValue<LineHeight>(c));
                // TODO: use lineheight parsed value
            }

            value.families = {try$(parseValue<FontFamily>(c))};

            return Ok(makeRc<FontProperty>(self(), std::move(value)));
        }
    };

    Value _value;

    FontProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    Vec<Rc<Property>> expandShorthand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        Vec<Rc<Property>> result;
        result.pushBack(makeRc<FontStyleProperty>(registry.resolveRegistration(Properties::FONT_STYLE, {}).unwrap(), _value.style));
        result.pushBack(makeRc<FontWidthProperty>(registry.resolveRegistration(Properties::FONT_WIDTH, {}).unwrap(), _value.width));
        result.pushBack(makeRc<FontSizeProperty>(registry.resolveRegistration(Properties::FONT_SIZE, {}).unwrap(), _value.size));
        result.pushBack(makeRc<FontFamilyProperty>(registry.resolveRegistration(Properties::FONT_FAMILY, {}).unwrap(), _value.families));
        if (_value.weight)
            result.pushBack(makeRc<FontWeightProperty>(registry.resolveRegistration(Properties::FONT_WEIGHT, {}).unwrap(), *_value.weight));
        return result;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
