module;

#include <karm/macros>

export module Vaev.Engine:props.fonts;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :values.specified;

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
            return makeRc<FontFamilyProperty>(self(), Experimental::FontFamilies{"sans-serif"_sym});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontFamilyProperty>(self(), Experimental::valueFromComputed<Experimental::FontFamilies>(c.font->families));
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.font.cow().families = parent.font->families;
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontFamilyProperty>(self(), try$(Experimental::parseValue<Experimental::FontFamilies>(c))));
        }
    };

    Experimental::FontFamilies _value;

    FontFamilyProperty(Rc<Property::Registration> registration, Experimental::FontFamilies value)
        : Property(registration), _value(std::move(value)) {}

    void apply(Experimental::ComputationContext const& ctx, ComputedValues const& parent, ComputedValues& c) const override {
        (void)parent;
        c.font.cow().families = Experimental::computeValue(_value, ctx, c);
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
            return makeRc<FontWeightProperty>(self(), Experimental::FontWeight{Experimental::Keywords::NORMAL});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontWeightProperty>(self(), Experimental::valueFromComputed<Experimental::FontWeight>(c.font->weight));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontWeightProperty>(self(), try$(Experimental::parseValue<Experimental::FontWeight>(c))));
        }
    };

    Experimental::FontWeight _value;

    FontWeightProperty(Rc<Property::Registration> registration, Experimental::FontWeight value)
        : Property(registration), _value(value) {}

    void apply(Experimental::ComputationContext const& ctx, ComputedValues const& parent, ComputedValues& c) const override {
        (void)parent;
        c.font.cow().weight = Experimental::computeValue(_value, ctx, c);
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
            return makeRc<FontWidthProperty>(self(), Experimental::FontWidth{Experimental::Keywords::NORMAL});
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.font.cow().width = parent.font->width;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontWidthProperty>(self(), c.font->width);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontWidthProperty>(self(), try$(Experimental::parseValue<Experimental::FontWidth>(c))));
        }
    };

    Experimental::FontWidth _value;

    FontWidthProperty(Rc<Property::Registration> registration, Experimental::FontWidth value)
        : Property(registration), _value(value) {}

    void apply(Experimental::ComputationContext const& ctx, ComputedValues const& parent, ComputedValues& c) const override {
        (void)parent;
        c.font.cow().width = Experimental::computeValue(_value, ctx, c);
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
            return makeRc<FontStyleProperty>(self(), Experimental::FontStyle{Experimental::Keywords::NORMAL});
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.font.cow().style = parent.font->style;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontStyleProperty>(self(), Experimental::valueFromComputed<Experimental::FontStyle>(c.font->style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontStyleProperty>(self(), try$(Experimental::parseValue<Experimental::FontStyle>(c))));
        }
    };

    Experimental::FontStyle _value;

    FontStyleProperty(Rc<Property::Registration> registration, Experimental::FontStyle value)
        : Property(registration), _value(value) {}

    void apply(Experimental::ComputationContext const& ctx, ComputedValues const& parent, ComputedValues& c) const override {
        (void)parent;
        c.font.cow().style = Experimental::computeValue(_value, ctx, c);
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
            return makeRc<FontSizeProperty>(self(), Experimental::FontSize{Experimental::Keywords::MEDIUM});
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.font.cow().size = parent.font->size;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontSizeProperty>(self(), Experimental::valueFromComputed<Experimental::FontSize>(c.font->size));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontSizeProperty>(self(), try$(Experimental::parseValue<Experimental::FontSize>(c))));
        }
    };

    Experimental::FontSize _value;

    FontSizeProperty(Rc<Property::Registration> registration, Experimental::FontSize value)
        : Property(registration), _value(value) {}

    void apply(Experimental::ComputationContext const& ctx, ComputedValues const& parent, ComputedValues& c) const override {
        (void)parent;
        c.font.cow().size = Experimental::computeValue(_value, ctx, c);
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
            return makeRc<FontProperty>(
                self(),
                Opt{Experimental::FontStyle{Experimental::Keywords::NORMAL}},
                Opt{Experimental::FontWeight{Experimental::Keywords::NORMAL}},
                Opt{Experimental::FontWidth{Experimental::Keywords::NORMAL}},
                Experimental::FontSize{Experimental::Keywords::MEDIUM},
                Experimental::FontFamilies{"sans-serif"_sym}
            );
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontProperty>(
                self(),
                Opt{Experimental::valueFromComputed<Experimental::FontStyle>(c.font->style)},
                Opt{Experimental::valueFromComputed<Experimental::FontWeight>(c.font->weight)},
                Opt{Experimental::valueFromComputed<Experimental::FontWidth>(c.font->width)},
                Experimental::valueFromComputed<Experimental::FontSize>(c.font->size),
                Experimental::valueFromComputed<Experimental::FontFamilies>(c.font->families)
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            // TODO: system family name
            Opt<Experimental::FontStyle> style = NONE;
            Opt<Experimental::FontWeight> weight = NONE;
            Opt<Experimental::FontWidth> width = NONE;
            Opt<Experimental::FontSize> size = NONE;

            while (true) {
                auto fontStyle = Experimental::parseValue<Experimental::FontStyle>(c);
                if (fontStyle) {
                    style = fontStyle.unwrap();
                    continue;
                }

                auto fontWeight = Experimental::parseValue<Experimental::FontWeight>(c);
                if (fontWeight) {
                    weight = fontWeight.unwrap();
                    continue;
                }

                // TODO: font variant https://www.w3.org/TR/css-fonts-4/#font-variant-css21-values

                auto fontWidth = Experimental::parseValue<Experimental::FontWidth>(c);
                if (fontWidth) {
                    width = fontWidth.unwrap();
                    continue;
                }

                auto fontSize = Experimental::parseValue<Experimental::FontSize>(c);
                if (fontSize) {
                    size = fontSize.unwrap();
                    break;
                }

                return Error::invalidData("expected font-style, font-weight, font-width or font-size");
            }

            if (c.skip(Css::Token::delim("/"))) {
                auto lh = Ok(parseValue<LineHeight>(c));
                // TODO: use lineheight parsed value
            }

            Experimental::FontFamilies families = try$(Experimental::parseValue<Experimental::FontFamilies>(c));

            return Ok(makeRc<FontProperty>(self(), style, weight, width, *size, std::move(families)));
        }
    };

    Opt<Experimental::FontStyle> _style;
    Opt<Experimental::FontWeight> _weight;
    Opt<Experimental::FontWidth> _width;
    Experimental::FontSize _size;
    Experimental::FontFamilies _families;

    FontProperty(Rc<Property::Registration> registration, Opt<Experimental::FontStyle> style, Opt<Experimental::FontWeight> weight, Opt<Experimental::FontWidth> width, Experimental::FontSize size, Experimental::FontFamilies families)
        : Property(registration), _style(style), _weight(weight), _width(width), _size(size), _families(std::move(families)) {}

    Vec<Rc<Property>> expandShorthand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        Vec<Rc<Property>> result;

        auto fontStyleRegistration = registry.resolveRegistration(Properties::FONT_STYLE, {}).unwrap();
        if (_style)
            result.pushBack(makeRc<FontStyleProperty>(fontStyleRegistration, *_style));
        else
            result.pushBack(fontStyleRegistration->initial());

        auto fontWeightRegistration = registry.resolveRegistration(Properties::FONT_WEIGHT, {}).unwrap();
        if (_weight)
            result.pushBack(makeRc<FontWeightProperty>(fontWeightRegistration, *_weight));
        else
            result.pushBack(fontWeightRegistration->initial());

        auto fontWidthRegistration = registry.resolveRegistration(Properties::FONT_WIDTH, {}).unwrap();
        if (_width)
            result.pushBack(makeRc<FontWidthProperty>(fontWidthRegistration, *_width));
        else
            result.pushBack(fontWidthRegistration->initial());

        result.pushBack(makeRc<FontSizeProperty>(registry.resolveRegistration(Properties::FONT_SIZE, {}).unwrap(), _size));
        result.pushBack(makeRc<FontFamilyProperty>(registry.resolveRegistration(Properties::FONT_FAMILY, {}).unwrap(), _families));

        return result;
    }

    void repr(Io::Emit& e) const override {
        e("{} {} {} {} {}", _style, _weight, _width, _size, _families);
    }
};

} // namespace Vaev::Style
