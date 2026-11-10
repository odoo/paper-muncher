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
    // Value: <font-weight-absolute> | bolder | lighter
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

// https://drafts.csswg.org/css-fonts-4/#font-width-prop
export struct FontWidthProperty : Property {
    using Value = Css::FontWidth;

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
            return makeRc<FontWidthProperty>(self(), toSpecified(c.font->width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontWidthProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    FontWidthProperty(Rc<Property::Registration> registration, Value value)
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

// https://drafts.csswg.org/css-fonts-4/#font-style-prop
export struct FontStyleProperty : Property {
    // Value: normal | italic | left | right | oblique <angle [-90deg,90deg]>?
    using Oblique = Css::All<Css::Keywords::Oblique, Opt<Css::Angle>>;
    using Value = Union<
        Css::Keywords::Normal,
        Css::Keywords::Italic,
        Css::Keywords::Left,
        Css::Keywords::Right,
        Oblique>;

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
            return Ok(makeRc<FontStyleProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    FontStyleProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    // Computed value: the keyword specified, plus angle in degrees if specified
    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.font.cow().style = _value.visit(
            [](Oblique const& oblique) {
                auto style = FontStyle{.val = Gfx::FontStyle::OBLIQUE};
                // <angle [-90deg,90deg]>: math functions are clamped to the range.
                if (oblique.v1)
                    style.obliqueAngle = Deg{clamp(toComputed(*oblique.v1).value(), -90.0, 90.0)};
                return style;
            },
            [](Css::Keywords::Italic) {
                return FontStyle{.val = Gfx::FontStyle::ITALIC};
            },
            // NOSPEC: left and right are treated as normal.
            [](auto) {
                return FontStyle{.val = Gfx::FontStyle::NORMAL};
            }
        );
    }

    Res<> serialize(Io::TextWriter& w) const override {
        // https://drafts.csswg.org/cssom/#serialize-a-css-value
        // If component values can be omitted or replaced with a shorter representation
        // without changing the meaning of the value, omit/replace them.
        // oblique 0deg is normal.
        if (auto oblique = _value.is<Oblique>(); oblique and oblique->v1) {
            auto plain = oblique->v1->calcTree().is<Css::UnitValue>();
            if (plain and plain->to(Css::UnitValue::Unit::DEG) == 0.0)
                return w.writeStr("normal"s);
        }
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-fonts-4/#font-size-prop
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

// https://drafts.csswg.org/css-fonts-4/#font-prop
export struct FontProperty : Property {
    // Value: [ [ <'font-style'> || <font-variant-css2> || <'font-weight'> || <font-width-css3> ]? <'font-
    //        size'> [ / <'line-height'> ]? <'font-family'># ] | <system-font-family-name>
    // FIXME: Implement <font-variant-css2>, <font-width-css3> and <system-font-family-name>.
    using Value = Css::All<
        Opt<Css::Any<FontStyleProperty::Value, FontWeightProperty::Value>>,
        FontSizeProperty::Value,
        Opt<Css::All<Css::Slash, LineHeightProperty::Value>>,
        Css::CommaSeparated<Css::FontFamily>>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::FONT, SHORTHAND) {}

        InitialValue initial() const override {
            return "medium sans-serif"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            auto const& font = *c.font;
            Css::Any<FontStyleProperty::Value, FontWeightProperty::Value> styleAndWeight;
            styleAndWeight.v0 = Some(toSpecified(font.style));
            styleAndWeight.v1 = Some(FontWeightProperty::Value{Css::FontWeightAbsolute{toSpecified(font.weight)}});

            Css::CommaSeparated<Css::FontFamily> families;
            for (auto const& family : font.families)
                families.pushBack(family);

            return makeRc<FontProperty>(
                self(),
                Value{
                    Some(std::move(styleAndWeight)),
                    FontSizeProperty::Value{toSpecified(PxPercent{font.size})},
                    Some(Css::All{Css::SLASH, toSpecified(c.inline_->lineHeight)}),
                    std::move(families),
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<FontProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    FontProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(std::move(value)) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        Longhands result;

        if (auto const& [styleWeightComb] = _value.v0) {
            if (auto const& [style] = styleWeightComb.v0)
                result.pushBack(makeRc<FontStyleProperty>(registry.resolveRegistration(Properties::FONT_STYLE, {}).expect(), style));

            if (auto const& [weight] = styleWeightComb.v1)
                result.pushBack(makeRc<FontWeightProperty>(registry.resolveRegistration(Properties::FONT_WEIGHT, {}).expect(), weight));
        }

        result.pushBack(makeRc<FontSizeProperty>(registry.resolveRegistration(Properties::FONT_SIZE, {}).expect(), _value.v1));

        if (auto const& [slashLineHeight] = _value.v2)
            result.pushBack(makeRc<LineHeightProperty>(registry.resolveRegistration(Properties::LINE_HEIGHT, {}).expect(), slashLineHeight.v1));

        Vec<Css::FontFamily> families;
        for (auto const& family : _value.v3)
            families.pushBack(family);
        result.pushBack(makeRc<FontFamilyProperty>(registry.resolveRegistration(Properties::FONT_FAMILY, {}).expect(), std::move(families)));

        return result;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
