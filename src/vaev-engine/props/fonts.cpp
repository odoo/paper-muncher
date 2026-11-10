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
using namespace Karm::Literals;

namespace Vaev::Style {

// MARK: Fonts -----------------------------------------------------------------

// https://www.w3.org/TR/css-fonts-4/#font-family-prop
export struct FontFamilyProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::FONT_FAMILY, {INHERITED, BULK_INHERITED}) {}

        // NOSPEC: The initial value depends on the user agent.
        InitialValue initial() const override {
            return "sans-serif"s;
        }

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontFamilyProperty>(self(), c.font->families);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.font.cow().families = parent.font->families;
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Vec<Css::FontFamily> value{};
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

    Vec<Css::FontFamily> _value;

    FontFamilyProperty(Rc<Property::Registration> registration, Vec<Css::FontFamily> value)
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
    // <font-weight-absolute> | bolder | lighter
    using Value = Union<Css::FontWeightAbsolute, Css::Keywords::Lighter, Css::Keywords::Bolder>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::FONT_WEIGHT, {INHERITED, BULK_INHERITED}) {}

        InitialValue initial() const override {
            return "normal"s;
        }

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.font.cow().weight = parent.font->weight;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontWeightProperty>(self(), Value{Css::FontWeightAbsolute{toSpecified(c.font->weight)}});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontWeightProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    FontWeightProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().weight = _value.visit(
            [](Css::FontWeightAbsolute const& weight) -> f64 {
                return toComputed(weight);
            },
            // https://www.w3.org/TR/css-fonts-4/#relative-weights
            [&](Css::Keywords::Lighter) -> f64 {
                auto w = parent.font->weight;
                if (w < 100)
                    return w;
                if (w < 550)
                    return 100;
                if (w < 750)
                    return 400;
                return 700;
            },
            [&](Css::Keywords::Bolder) -> f64 {
                auto w = parent.font->weight;
                if (w < 350)
                    return 400;
                if (w < 550)
                    return 700;
                if (w < 900)
                    return 900;
                return w;
            }
        );
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

        InitialValue initial() const override {
            return "normal"s;
        }

        // https://drafts.csswg.org/css-fonts/#font-stretch-prop
        SmallVec<Symbol, 1> legacyNameAlias() const override {
            return {"font-stretch"_sym};
        }

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.font.cow().width = parent.font->width;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontWidthProperty>(self(), Css::FontWidth{toSpecified(c.font->width)});
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

        InitialValue initial() const override {
            return "normal"s;
        }

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
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

        InitialValue initial() const override {
            return "medium"s;
        }

        ComputationPhase computationPhase() const override {
            return ComputationPhase::FONT;
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.font.cow().size = parent.font->size;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<FontSizeProperty>(self(), toSpecified(PxPercent{c.font->size}));
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
                return Px{max(toComputed(v, cx, cx.parentFontSize).value(), 0.0)};
            },
            [&](Keywords::XxSmall) {
                return Px{cx.userFontSize * 0.5};
            },
            [&](Keywords::XSmall) {
                return Px{cx.userFontSize * 0.75};
            },
            [&](Keywords::Small) {
                return Px{cx.userFontSize * 0.875};
            },
            [&](Keywords::Medium) {
                return Px{cx.userFontSize};
            },
            [&](Keywords::Large) {
                return Px{cx.userFontSize * 1.125};
            },
            [&](Keywords::XLarge) {
                return Px{cx.userFontSize * 1.25};
            },
            [&](Keywords::XxLarge) {
                return Px{cx.userFontSize * 1.5};
            },
            [&](Keywords::Larger) {
                return Px{cx.parentFontSize * 1.25};
            },
            [&](Keywords::Smaller) {
                return Px{cx.parentFontSize * 0.875};
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
        Vec<Css::FontFamily> families = {"sans-serif"_sym};
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

        InitialValue initial() const override {
            return "medium sans-serif"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            auto const& font = *c.font;
            return makeRc<FontProperty>(
                self(),
                Value{
                    font.families,
                    Some(FontWeightProperty::Value{Css::FontWeightAbsolute{toSpecified(font.weight)}}),
                    Css::FontWidth{toSpecified(font.width)},
                    toSpecified(font.style),
                    toSpecified(PxPercent{font.size}),
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
