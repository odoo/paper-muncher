module;

#include <karm/macros>

export module Vaev.Engine:props.outline;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// MARK: Outline --------------------------------------------------------------

// https://drafts.csswg.org/css-ui/#outline-width
export struct OutlineWidthProperty : Property {
    using Value = Css::LineWidth;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::OUTLINE_WIDTH) {}

        InitialValue initialValue() const override {
            return "medium"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OutlineWidthProperty>(self(), toSpecified(c.outline->width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OutlineWidthProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    OutlineWidthProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.outline.cow().width = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-style
export struct OutlineStyleProperty : Property {
    using Value = Union<Keywords::Auto, Gfx::BorderStyle>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::OUTLINE_STYLE) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OutlineStyleProperty>(self(), c.outline->style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OutlineStyleProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    OutlineStyleProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.outline.cow().style = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline-color
export struct OutlineColorProperty : Property {
    using Value = Union<Keywords::Auto, Css::Color>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::OUTLINE_COLOR) {}

        InitialValue initialValue() const override {
            return "auto"s;
        }

        ComputationPhase computationPhase() const override {
            // Must be applied after 'outline-style'.
            return ComputationPhase::LATE;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OutlineColorProperty>(self(), toSpecified(c.outline->color));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OutlineColorProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    OutlineColorProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.outline.cow().color = _value.visit(
            // https://drafts.csswg.org/css-ui/#valdef-outline-color-auto
            [&](Keywords::Auto) -> Color {
                // When outline-style is auto, outline-color: auto computes to auto and represents the accent color.
                // Otherwise, outline-color: auto computes to currentColor.
                // NOSPEC: auto is stored as the accent color it represents.
                if (c.outline->style == Keywords::AUTO)
                    return toComputed(Css::Color{Css::SystemColor::ACCENT_COLOR}, c.color);
                return Keywords::CURRENT_COLOR;
            },
            [&](Css::Color const& color) {
                return toComputed(color, c.color);
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

// https://drafts.csswg.org/css-ui/#outline-offset
export struct OutlineOffsetProperty : Property {
    using Value = Css::Length;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::OUTLINE_OFFSET) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OutlineOffsetProperty>(self(), toSpecified(c.outline->offset));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<OutlineOffsetProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    OutlineOffsetProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.outline.cow().offset = toComputed(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-ui/#outline
export struct OutlineProperty : Property {
    struct Value {
        Css::LineWidth width = Keywords::MEDIUM;
        Css::Length offset = Css::px(0);
        Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
        Union<Keywords::Auto, Css::Color> color = Keywords::AUTO;

        void repr(Io::Emit& e) const {
            e("(outline {} {} {} {})", width, offset, style, color);
        }
    };

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::OUTLINE, SHORTHAND) {}

        InitialValue initialValue() const override {
            return "medium none auto"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<OutlineProperty>(
                self(),
                Value{
                    toSpecified(c.outline->width),
                    toSpecified(c.outline->offset),
                    c.outline->style,
                    toSpecified(c.outline->color),
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Value value;
            bool widthSet = false;
            bool styleSet = false;
            bool colorSet = false;
            usize autoCount = 0;

            while (not c.ended()) {
                if (c.skip(Css::Token::ident("auto"))) {
                    autoCount++;
                    continue;
                }

                auto width = parseValue<Css::LineWidth>(c);
                if (width) {
                    if (widthSet)
                        return Error::invalidData("duplicated outline width");
                    value.width = width.expect();
                    widthSet = true;
                    continue;
                }

                auto color = parseValue<Css::Color>(c);
                if (color) {
                    if (colorSet)
                        return Error::invalidData("duplicated outline color");
                    value.color = color.expect();
                    colorSet = true;
                    continue;
                }

                auto style = parseValue<Gfx::BorderStyle>(c);
                if (style) {
                    if (styleSet)
                        return Error::invalidData("duplicated outline style");
                    value.style = style.expect();
                    styleSet = true;
                    continue;
                }

                break;
            }

            for (usize i = 0; i < autoCount; i++) {
                if (not styleSet) {
                    value.style = Keywords::AUTO;
                    styleSet = true;
                } else if (not colorSet) {
                    value.color = Keywords::AUTO;
                    colorSet = true;
                } else {
                    return Error::invalidData("duplicated outline auto");
                }
            }

            return Ok(makeRc<OutlineProperty>(self(), value));
        }
    };

    Value _value;

    OutlineProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<OutlineWidthProperty>(registry.resolveRegistration(Properties::OUTLINE_WIDTH, {}).expect(), _value.width),
            makeRc<OutlineStyleProperty>(registry.resolveRegistration(Properties::OUTLINE_STYLE, {}).expect(), _value.style),
            makeRc<OutlineColorProperty>(registry.resolveRegistration(Properties::OUTLINE_COLOR, {}).expect(), _value.color),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
