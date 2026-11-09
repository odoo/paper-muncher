module;

#include <karm/macros>

export module Vaev.Engine:props.fonts;

import Karm.Core;
import :props.base;
import :props.baseline;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-fonts-4/#font-width-prop
// Computed value: a percentage
export Percent toComputed(Css::FontWidth const& value) {
    return Percent{value.val().value()};
}

static Css::FontWidth _toSpecifiedFontWidth(Percent width) {
    return Css::FontWidth{Css::PurePercentage{width.value()}};
}

// https://drafts.csswg.org/css-fonts-4/#font-style-prop
export FontStyle toComputed(Css::FontStyle const& value) {
    return {value.val, toComputed(value.obliqueAngle)};
}

export Css::FontStyle toSpecified(FontStyle const& value) {
    if (value.val == Gfx::FontStyle::OBLIQUE)
        return Css::FontStyle{Css::PureAngle::fromDegree(value.obliqueAngle.value())};
    return Css::FontStyle{value.val};
}

// MARK: Fonts -----------------------------------------------------------------

// https://www.w3.org/TR/css-fonts-4/#font-family-prop
export struct FontFamilyProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::FONT_FAMILY, {INHERITED, BULK_INHERITED}) {}

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
        }

        Rc<Property> initial() const override {
            return makeRc<FontFamilyProperty>(self(), Vec<FontFamily>{"sans-serif"_sym});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontFamilyProperty>(self(), c.font->families);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.font.cow().families = parent.font->families;
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Vec<FontFamily> value{};
            eatWhitespace(c);
            while (not c.ended()) {
                value.pushBack(try$(parseValue<Css::FontFamily>(c)));

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
    enum struct Relative : u8 {
        LIGHTER,
        BOLDER,

        _LEN
    };

    struct Value : Union<Gfx::FontWeight, Relative> {
        using _Base = Union<Gfx::FontWeight, Relative>;
        using _Base::_Base;

        Value()
            : Value(Gfx::FontWeight::REGULAR) {
        }

        bool isRelative() const {
            return is<Relative>().has();
        }

        Gfx::FontWeight resolve() const {
            if (is<Relative>())
                return Gfx::FontWeight::REGULAR;

            return expect<Gfx::FontWeight>();
        }

        Gfx::FontWeight resolve(Gfx::FontWeight const& parent) const {
            if (is<Relative>()) {
                if (expect<Relative>() == Relative::LIGHTER)
                    return parent.lighter();
                return parent.bolder();
            }

            return expect<Gfx::FontWeight>();
        }

        // <font-weight-absolute> | bolder | lighter
        static Res<Value> parse(Cursor<Css::Sst>& c) {
            if (c.ended())
                return Error::invalidData("unexpected end of input");

            if (c.skip(Css::Token::ident("bolder")))
                return Ok(Relative::BOLDER);
            if (c.skip(Css::Token::ident("lighter")))
                return Ok(Relative::LIGHTER);
            return Ok(try$(parseValue<Gfx::FontWeight>(c)));
        }

        static Res<> serialize(Io::TextWriter& w, Value const& value) {
            return value.visit([&](auto const& v) {
                return Css::serializeValue(w, v);
            });
        }

        void repr(Io::Emit& e) const {
            e("{}", static_cast<_Base const&>(*this));
        }
    };

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::FONT_WEIGHT, {INHERITED, BULK_INHERITED}) {}

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
        }

        Rc<Property> initial() const override {
            return makeRc<FontWeightProperty>(self(), Value{Gfx::FontWeight::REGULAR});
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.font.cow().weight = parent.font->weight;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontWeightProperty>(self(), c.font->weight);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontWeightProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    FontWeightProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().weight = _value.resolve(parent.font->weight);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-fonts-4/#font-width-prop
export struct FontWidthProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::FONT_WIDTH, {INHERITED, BULK_INHERITED}) {}

        // https://drafts.csswg.org/css-fonts/#font-stretch-prop
        SmallVec<Symbol, 1> legacyNameAlias() const override {
            return {"font-stretch"_sym};
        }

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
        }

        Rc<Property> initial() const override {
            return makeRc<FontWidthProperty>(self(), Css::FontWidth::NORMAL);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.font.cow().width = parent.font->width;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontWidthProperty>(self(), _toSpecifiedFontWidth(c.font->width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontWidthProperty>(self(), try$(parseValue<Css::FontWidth>(c))));
        }
    };

    Css::FontWidth _value;

    FontWidthProperty(Rc<Property::Registration> registration, Css::FontWidth value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().width = toComputed(_value);
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
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::FONT_STYLE, {INHERITED, BULK_INHERITED}) {}

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
        }

        Rc<Property> initial() const override {
            return makeRc<FontStyleProperty>(self(), Css::FontStyle::NORMAL);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.font.cow().style = parent.font->style;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontStyleProperty>(self(), toSpecified(c.font->style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontStyleProperty>(self(), try$(parseValue<Css::FontStyle>(c))));
        }
    };

    Css::FontStyle _value;

    FontStyleProperty(Rc<Property::Registration> registration, Css::FontStyle value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().style = toComputed(_value);
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
    using Value = Union<
        Css::LengthPercentage,
        Keywords::XxSmall,
        Keywords::XSmall,
        Keywords::Small,
        Keywords::Medium,
        Keywords::Large,
        Keywords::XLarge,
        Keywords::XxLarge,
        Keywords::Smaller,
        Keywords::Larger>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::FONT_SIZE, {INHERITED, BULK_INHERITED}) {}

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
        }

        Rc<Property> initial() const override {
            return makeRc<FontSizeProperty>(self(), Keywords::MEDIUM);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.font.cow().size = parent.font->size;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontSizeProperty>(self(), Css::LengthPercentage::fromCanonical(c.font->size.value()));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontSizeProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    FontSizeProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().size = _value.visit(
            [&](Css::LengthPercentage const& v) {
                // NOTE: At this point, cx's font metrics are the parent's, so `em` resolves against the parent font size, and
                //       percentages are relative to it too. Negative calc() results are clamped to the allowed [0,∞] range.
                //       https://www.w3.org/TR/css-fonts-4/#font-size-prop
                return Pixel{max(toComputed(v, cx, cx.parentFontSize).value(), 0.0)};
            },
            [&](Keywords::XxSmall) {
                return Pixel{cx.userFontSize * 0.5};
            },
            [&](Keywords::XSmall) {
                return Pixel{cx.userFontSize * 0.75};
            },
            [&](Keywords::Small) {
                return Pixel{cx.userFontSize * 0.875};
            },
            [&](Keywords::Medium) {
                return Pixel{cx.userFontSize};
            },
            [&](Keywords::Large) {
                return Pixel{cx.userFontSize * 1.125};
            },
            [&](Keywords::XLarge) {
                return Pixel{cx.userFontSize * 1.25};
            },
            [&](Keywords::XxLarge) {
                return Pixel{cx.userFontSize * 1.5};
            },
            [&](Keywords::Larger) {
                return Pixel{cx.parentFontSize * 1.25};
            },
            [&](Keywords::Smaller) {
                return Pixel{cx.parentFontSize * 0.875};
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

// https://www.w3.org/TR/css-fonts-4/#font-prop
export struct FontProperty : Property {
    struct Value {
        Vec<FontFamily> families = {"sans-serif"_sym};
        Opt<FontWeightProperty::Value> weight;
        Css::FontWidth width = Css::FontWidth::NORMAL;
        Css::FontStyle style = Css::FontStyle::NORMAL;
        FontSizeProperty::Value size = Keywords::MEDIUM;
        LineHeightProperty::Value lineHeight = Keywords::NORMAL;

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
        Registration()
            : Property::Registration(Properties::FONT, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<FontProperty>(self(), Value{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            auto const& font = *c.font;
            return makeRc<FontProperty>(
                self(),
                Value{
                    font.families,
                    Some(font.weight),
                    _toSpecifiedFontWidth(font.width),
                    toSpecified(font.style),
                    Css::LengthPercentage::fromCanonical(font.size.value()),
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            // TODO: system family name
            FontProperty::Value value;

            while (true) {
                auto fontStyle = parseValue<Css::FontStyle>(c);
                if (fontStyle) {
                    value.style = fontStyle.expect();
                    continue;
                }

                auto fontWeight = parseValue<FontWeightProperty::Value>(c);
                if (fontWeight) {
                    value.weight = Some(fontWeight.expect());
                    continue;
                }

                // TODO: font variant https://www.w3.org/TR/css-fonts-4/#font-variant-css21-values

                auto fontWidth = parseValue<Css::FontWidth>(c);
                if (fontWidth) {
                    value.width = fontWidth.expect();
                    continue;
                }

                auto fontSize = parseValue<FontSizeProperty::Value>(c);
                if (fontSize) {
                    value.size = fontSize.expect();
                    break;
                }

                return Error::invalidData("expected font-style, font-weight, font-width or font-size");
            }

            if (c.skip(Css::Token::delim("/")))
                value.lineHeight = try$(parseValue<LineHeightProperty::Value>(c));

            value.families = {try$(parseValue<Css::FontFamily>(c))};

            return Ok(makeRc<FontProperty>(self(), std::move(value)));
        }
    };

    Value _value;

    FontProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        Longhands result;
        result.pushBack(makeRc<FontStyleProperty>(registry.resolveRegistration(Properties::FONT_STYLE, {}).expect(), _value.style));
        result.pushBack(makeRc<FontWidthProperty>(registry.resolveRegistration(Properties::FONT_WIDTH, {}).expect(), _value.width));
        result.pushBack(makeRc<FontSizeProperty>(registry.resolveRegistration(Properties::FONT_SIZE, {}).expect(), _value.size));
        result.pushBack(makeRc<FontFamilyProperty>(registry.resolveRegistration(Properties::FONT_FAMILY, {}).expect(), _value.families));
        result.pushBack(makeRc<LineHeightProperty>(registry.resolveRegistration(Properties::LINE_HEIGHT, {}).expect(), _value.lineHeight));
        if (_value.weight)
            result.pushBack(makeRc<FontWeightProperty>(registry.resolveRegistration(Properties::FONT_WEIGHT, {}).expect(), *_value.weight));
        return result;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
