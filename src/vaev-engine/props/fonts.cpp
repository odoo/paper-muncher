module;

#include <karm/macros>

export module Vaev.Engine:props.fonts;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

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
            return {INHERITED};
        }

        Rc<Property> initial() const override {
            return makeRc<FontFamilyProperty>(self(), Vec<FontFamily>{"sans-serif"_sym});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FontFamilyProperty>(self(), c.font->families);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Vec<FontFamily> value{};
            eatWhitespace(c);
            while (not c.ended()) {
                value.pushBack(try$(parseValue<FontFamily>(c)));

                eatWhitespace(c);
                c.skip(Css::Token::comma());
                eatWhitespace(c);
            }
            return Ok(makeRc<FontFamilyProperty>(self(), std::move(value)));
        }
    };

    Vec<FontFamily> _value;

    FontFamilyProperty(Rc<Property::Registration> registration, Vec<FontFamily> value)
        : Property(registration), _value(std::move(value)) {}

    void apply(SpecifiedValues& c) const override {
        c.font.cow().families = _value;
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

        Rc<Property> initial() const override {
            return makeRc<FontWeightProperty>(self(), FontWeight{Gfx::FontWeight::REGULAR});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FontWeightProperty>(self(), c.font->weight);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontWeightProperty>(self(), try$(parseValue<FontWeight>(c))));
        }
    };

    FontWeight _value;

    FontWeightProperty(Rc<Property::Registration> registration, FontWeight value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.font.cow().weight = _value.resolve();
    }

    void apply(SpecifiedValues const& parent, SpecifiedValues& c) const override {
        c.font.cow().weight = _value.resolve(parent.font->weight);
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
            return {INHERITED};
        }

        Rc<Property> initial() const override {
            return makeRc<FontWidthProperty>(self(), FontWidth::NORMAL);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FontWidthProperty>(self(), c.font->width);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontWidthProperty>(self(), try$(parseValue<FontWidth>(c))));
        }
    };

    FontWidth _value;

    FontWidthProperty(Rc<Property::Registration> registration, FontWidth value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.font.cow().width = _value;
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
            return {INHERITED};
        }

        Rc<Property> initial() const override {
            return makeRc<FontStyleProperty>(self(), FontStyle::NORMAL);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FontStyleProperty>(self(), c.font->style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontStyleProperty>(self(), try$(parseValue<FontStyle>(c))));
        }
    };

    FontStyle _value;

    FontStyleProperty(Rc<Property::Registration> registration, FontStyle value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.font.cow().style = _value;
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

        Rc<Property> initial() const override {
            return makeRc<FontSizeProperty>(self(), FontSize::MEDIUM);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FontSizeProperty>(self(), c.font->size);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontSizeProperty>(self(), try$(parseValue<FontSize>(c))));
        }
    };

    FontSize _value;

    FontSizeProperty(Rc<Property::Registration> registration, FontSize value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.font.cow().size = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-prop
export struct FontProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::FONT;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<FontProperty>(self(), FontProps{}, None{});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<FontProperty>(self(), *c.font, None{});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            // TODO: system family name
            FontProps value;
            Opt<FontWeight> unresolvedWeight;

            while (true) {
                auto fontStyle = parseValue<FontStyle>(c);
                if (fontStyle) {
                    value.style = fontStyle.unwrap();
                    continue;
                }

                auto fontWeight = parseValue<FontWeight>(c);
                if (fontWeight) {
                    unresolvedWeight = fontWeight.unwrap();
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

            return Ok(makeRc<FontProperty>(self(), std::move(value), unresolvedWeight));
        }
    };

    FontProps _value;
    Opt<FontWeight> _unresolvedWeight;

    FontProperty(Rc<Property::Registration> registration, FontProps value, Opt<FontWeight> unresolvedWeight)
        : Property(registration), _value(std::move(value)), _unresolvedWeight(unresolvedWeight) {}

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
        Vec<Rc<Property>> result;
        result.pushBack(makeRc<FontStyleProperty>(registry.resolveRegistration(Properties::FONT_STYLE, {}).unwrap(), _value.style));
        result.pushBack(makeRc<FontWidthProperty>(registry.resolveRegistration(Properties::FONT_WIDTH, {}).unwrap(), _value.width));
        result.pushBack(makeRc<FontSizeProperty>(registry.resolveRegistration(Properties::FONT_SIZE, {}).unwrap(), _value.size));
        result.pushBack(makeRc<FontFamilyProperty>(registry.resolveRegistration(Properties::FONT_FAMILY, {}).unwrap(), _value.families));
        if (_unresolvedWeight)
            result.pushBack(makeRc<FontWeightProperty>(registry.resolveRegistration(Properties::FONT_WEIGHT, {}).unwrap(), *_unresolvedWeight));
        return result;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
