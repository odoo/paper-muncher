module;

#include <karm/macros>

export module Vaev.Engine:props.border;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;

using namespace Karm;

namespace Vaev::Style {

// MARK: Borders ---------------------------------------------------------------

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
export struct BorderTopColorProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_TOP_COLOR) {}

        Rc<Property> initial() const override {
            return makeRc<BorderTopColorProperty>(self(), Css::BLACK);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopColorProperty>(self(), toSpecified(c.borders->top.color));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopColorProperty>(self(), try$(parseValue<Vaev::Css::Color>(c))));
        }
    };

    Vaev::Css::Color _value;

    BorderTopColorProperty(Rc<Property::Registration> registration, Vaev::Css::Color value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().top.color = toComputed(_value, c.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
export struct BorderRightColorProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_RIGHT_COLOR) {}

        Rc<Property> initial() const override {
            return makeRc<BorderRightColorProperty>(self(), Css::BLACK);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRightColorProperty>(self(), toSpecified(c.borders->end.color));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightColorProperty>(self(), try$(parseValue<Vaev::Css::Color>(c))));
        }
    };

    Vaev::Css::Color _value;

    BorderRightColorProperty(Rc<Property::Registration> registration, Vaev::Css::Color value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().end.color = toComputed(_value, c.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
export struct BorderBottomColorProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_BOTTOM_COLOR) {}

        Rc<Property> initial() const override {
            return makeRc<BorderBottomColorProperty>(self(), Css::BLACK);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomColorProperty>(self(), toSpecified(c.borders->bottom.color));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomColorProperty>(self(), try$(parseValue<Vaev::Css::Color>(c))));
        }
    };

    Vaev::Css::Color _value;

    BorderBottomColorProperty(Rc<Property::Registration> registration, Vaev::Css::Color value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().bottom.color = toComputed(_value, c.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
export struct BorderLeftColorProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_LEFT_COLOR) {}

        Rc<Property> initial() const override {
            return makeRc<BorderLeftColorProperty>(self(), Css::BLACK);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderLeftColorProperty>(self(), toSpecified(c.borders->start.color));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftColorProperty>(self(), try$(parseValue<Vaev::Css::Color>(c))));
        }
    };

    Vaev::Css::Color _value;

    BorderLeftColorProperty(Rc<Property::Registration> registration, Vaev::Css::Color value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().start.color = toComputed(_value, c.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct BorderColorProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_COLOR, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<BorderColorProperty>(self(), Math::Insets<Vaev::Css::Color>{Css::BLACK});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderColorProperty>(
                self(),
                Math::Insets<Vaev::Css::Color>{
                    toSpecified(c.borders->top.color),
                    toSpecified(c.borders->end.color),
                    toSpecified(c.borders->bottom.color),
                    toSpecified(c.borders->start.color),
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderColorProperty>(self(), try$(parseValue<Math::Insets<Vaev::Css::Color>>(c))));
        }
    };

    Math::Insets<Vaev::Css::Color> _value;

    BorderColorProperty(Rc<Property::Registration> registration, Math::Insets<Vaev::Css::Color> value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderTopColorProperty>(registry.resolveRegistration(Properties::BORDER_TOP_COLOR, {}).expect(), _value.top),
            makeRc<BorderRightColorProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_COLOR, {}).expect(), _value.end),
            makeRc<BorderBottomColorProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_COLOR, {}).expect(), _value.bottom),
            makeRc<BorderLeftColorProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_COLOR, {}).expect(), _value.start),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderLeftStyleProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_LEFT_STYLE) {}

        Rc<Property> initial() const override {
            return makeRc<BorderLeftStyleProperty>(self(), Gfx::BorderStyle::NONE);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderLeftStyleProperty>(self(), c.borders->start.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftStyleProperty>(self(), try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderLeftStyleProperty(Rc<Property::Registration> registration, Gfx::BorderStyle value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().start.style = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderTopStyleProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_TOP_STYLE) {}

        Rc<Property> initial() const override {
            return makeRc<BorderTopStyleProperty>(self(), Gfx::BorderStyle::NONE);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopStyleProperty>(self(), c.borders->top.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopStyleProperty>(self(), try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderTopStyleProperty(Rc<Property::Registration> registration, Gfx::BorderStyle value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().top.style = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderRightStyleProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_RIGHT_STYLE) {}

        Rc<Property> initial() const override {
            return makeRc<BorderRightStyleProperty>(self(), Gfx::BorderStyle::NONE);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRightStyleProperty>(self(), c.borders->end.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightStyleProperty>(self(), try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderRightStyleProperty(Rc<Property::Registration> registration, Gfx::BorderStyle value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().end.style = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderBottomStyleProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_BOTTOM_STYLE) {}

        Rc<Property> initial() const override {
            return makeRc<BorderBottomStyleProperty>(self(), Gfx::BorderStyle::NONE);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomStyleProperty>(self(), c.borders->bottom.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomStyleProperty>(self(), try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderBottomStyleProperty(Rc<Property::Registration> registration, Gfx::BorderStyle value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().bottom.style = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties

export struct BorderStyleProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_STYLE, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<BorderStyleProperty>(self(), Math::Insets{Gfx::BorderStyle::NONE});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderStyleProperty>(
                self(),
                Math::Insets{
                    c.borders->start.style,
                    c.borders->end.style,
                    c.borders->top.style,
                    c.borders->bottom.style,
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderStyleProperty>(self(), try$(parseValue<Math::Insets<Gfx::BorderStyle>>(c))));
        }
    };

    Math::Insets<Gfx::BorderStyle> _value;

    BorderStyleProperty(Rc<Property::Registration> registration, Math::Insets<Gfx::BorderStyle> value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderTopStyleProperty>(registry.resolveRegistration(Properties::BORDER_TOP_STYLE, {}).expect(), _value.top),
            makeRc<BorderRightStyleProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_STYLE, {}).expect(), _value.end),
            makeRc<BorderBottomStyleProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_STYLE, {}).expect(), _value.bottom),
            makeRc<BorderLeftStyleProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_STYLE, {}).expect(), _value.start),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderTopWidthProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_TOP_WIDTH) {}

        Rc<Property> initial() const override {
            return makeRc<BorderTopWidthProperty>(self(), Keywords::MEDIUM);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopWidthProperty>(self(), toSpecified(c.borders->top.width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopWidthProperty>(self(), try$(parseValue<Css::LineWidth>(c))));
        }
    };

    Css::LineWidth _value;

    BorderTopWidthProperty(Rc<Property::Registration> registration, Css::LineWidth value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().top.width = resolve(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderRightWidthProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_RIGHT_WIDTH) {}

        Rc<Property> initial() const override {
            return makeRc<BorderRightWidthProperty>(self(), Keywords::MEDIUM);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRightWidthProperty>(self(), toSpecified(c.borders->end.width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightWidthProperty>(self(), try$(parseValue<Css::LineWidth>(c))));
        }
    };

    Css::LineWidth _value;

    BorderRightWidthProperty(Rc<Property::Registration> registration, Css::LineWidth value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().end.width = resolve(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderBottomWidthProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_BOTTOM_WIDTH) {}

        Rc<Property> initial() const override {
            return makeRc<BorderBottomWidthProperty>(self(), Keywords::MEDIUM);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomWidthProperty>(self(), toSpecified(c.borders->bottom.width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomWidthProperty>(self(), try$(parseValue<Css::LineWidth>(c))));
        }
    };

    Css::LineWidth _value;

    BorderBottomWidthProperty(Rc<Property::Registration> registration, Css::LineWidth value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().bottom.width = resolve(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderLeftWidthProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_LEFT_WIDTH) {}

        Rc<Property> initial() const override {
            return makeRc<BorderLeftWidthProperty>(self(), Keywords::MEDIUM);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderLeftWidthProperty>(self(), toSpecified(c.borders->start.width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftWidthProperty>(self(), try$(parseValue<Css::LineWidth>(c))));
        }
    };

    Css::LineWidth _value;

    BorderLeftWidthProperty(Rc<Property::Registration> registration, Css::LineWidth value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().start.width = resolve(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderTopRightRadiusProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_TOP_RIGHT_RADIUS) {}

        Rc<Property> initial() const override {
            return makeRc<BorderTopRightRadiusProperty>(self(), makeArray<Css::LengthPercentage, 2>(Css::LengthPercentage::fromCanonical(0)));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopRightRadiusProperty>(self(), Array{toSpecified(c.borders->radii.c), toSpecified(c.borders->radii.d)});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto first = try$(parseValue<Css::LengthPercentage>(c));
            Array value{first, first};
            if (not c.ended())
                value[1] = try$(parseValue<Css::LengthPercentage>(c));
            return Ok(makeRc<BorderTopRightRadiusProperty>(self(), value));
        }
    };

    Array<Css::LengthPercentage, 2> _value;

    BorderTopRightRadiusProperty(Rc<Property::Registration> registration, Array<Css::LengthPercentage, 2> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().radii.c = resolve(_value[0], cx);
        c.borders.cow().radii.d = resolve(_value[1], cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderTopLeftRadiusProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_TOP_LEFT_RADIUS) {}

        Rc<Property> initial() const override {
            return makeRc<BorderTopLeftRadiusProperty>(self(), makeArray<Css::LengthPercentage, 2>(Css::LengthPercentage::fromCanonical(0)));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopLeftRadiusProperty>(
                self(),
                Array{toSpecified(c.borders->radii.b), toSpecified(c.borders->radii.a)}
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto first = try$(parseValue<Css::LengthPercentage>(c));
            Array value{first, first};
            if (not c.ended())
                value[1] = try$(parseValue<Css::LengthPercentage>(c));
            return Ok(makeRc<BorderTopLeftRadiusProperty>(self(), value));
        }
    };

    Array<Css::LengthPercentage, 2> _value;

    BorderTopLeftRadiusProperty(Rc<Property::Registration> registration, Array<Css::LengthPercentage, 2> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().radii.a = resolve(_value[1], cx);
        c.borders.cow().radii.b = resolve(_value[0], cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderBottomRightRadiusProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_BOTTOM_RIGHT_RADIUS) {}

        Rc<Property> initial() const override {
            return makeRc<BorderBottomRightRadiusProperty>(self(), makeArray<Css::LengthPercentage, 2>(Css::LengthPercentage::fromCanonical(0)));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomRightRadiusProperty>(self(), Array{toSpecified(c.borders->radii.f), toSpecified(c.borders->radii.e)});
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto first = try$(parseValue<Css::LengthPercentage>(c));
            Array value{first, first};
            if (not c.ended())
                value[1] = try$(parseValue<Css::LengthPercentage>(c));
            return Ok(makeRc<BorderBottomRightRadiusProperty>(self(), value));
        }
    };

    Array<Css::LengthPercentage, 2> _value;

    BorderBottomRightRadiusProperty(Rc<Property::Registration> registration, Array<Css::LengthPercentage, 2> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().radii.e = resolve(_value[1], cx);
        c.borders.cow().radii.f = resolve(_value[0], cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderBottomLeftRadiusProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_BOTTOM_LEFT_RADIUS) {}

        Rc<Property> initial() const override {
            return makeRc<BorderBottomLeftRadiusProperty>(self(), makeArray<Css::LengthPercentage, 2>(Css::LengthPercentage::fromCanonical(0)));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomLeftRadiusProperty>(
                self(),
                Array{toSpecified(c.borders->radii.g), toSpecified(c.borders->radii.h)}
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto first = try$(parseValue<Css::LengthPercentage>(c));
            Array value{first, first};
            if (not c.ended())
                value[1] = try$(parseValue<Css::LengthPercentage>(c));
            return Ok(makeRc<BorderBottomLeftRadiusProperty>(self(), value));
        }
    };

    Array<Css::LengthPercentage, 2> _value;

    BorderBottomLeftRadiusProperty(Rc<Property::Registration> registration, Array<Css::LengthPercentage, 2> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().radii.g = resolve(_value[0], cx);
        c.borders.cow().radii.h = resolve(_value[1], cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderRadiusProperty : Property {
    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_RADIUS, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<BorderRadiusProperty>(self(), Math::Radii<Css::LengthPercentage>{Css::LengthPercentage::fromCanonical(0)});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRadiusProperty>(self(), toSpecified(c.borders->radii));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto radii = try$(parseValue<Math::Radii<Css::LengthPercentage>>(c));
            return Ok(makeRc<BorderRadiusProperty>(self(), radii));
        }
    };

    Math::Radii<Css::LengthPercentage> _value;

    BorderRadiusProperty(Rc<Property::Registration> registration, Math::Radii<Css::LengthPercentage> value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().radii = resolve(_value, cx);
    }

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderTopLeftRadiusProperty>(registry.resolveRegistration(Properties::BORDER_TOP_LEFT_RADIUS, {}).expect(), Array{_value.b, _value.a}),
            makeRc<BorderTopRightRadiusProperty>(registry.resolveRegistration(Properties::BORDER_TOP_RIGHT_RADIUS, {}).expect(), Array{_value.c, _value.d}),
            makeRc<BorderBottomRightRadiusProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_RIGHT_RADIUS, {}).expect(), Array{_value.f, _value.e}),
            makeRc<BorderBottomLeftRadiusProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_LEFT_RADIUS, {}).expect(), Array{_value.g, _value.h}),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderTopProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_TOP, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<BorderTopProperty>(self(), Css::SpecifiedBorder{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopProperty>(self(), c.borders->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopProperty>(self(), try$(parseValue<Css::SpecifiedBorder>(c))));
        }
    };

    Css::SpecifiedBorder _value;

    BorderTopProperty(Rc<Property::Registration> registration, Css::SpecifiedBorder value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderTopWidthProperty>(registry.resolveRegistration(Properties::BORDER_TOP_WIDTH, {}).expect(), _value.width),
            makeRc<BorderTopStyleProperty>(registry.resolveRegistration(Properties::BORDER_TOP_STYLE, {}).expect(), _value.style),
            makeRc<BorderTopColorProperty>(registry.resolveRegistration(Properties::BORDER_TOP_COLOR, {}).expect(), _value.color),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderRightProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_RIGHT, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<BorderRightProperty>(self(), Css::SpecifiedBorder{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRightProperty>(self(), c.borders->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightProperty>(self(), try$(parseValue<Css::SpecifiedBorder>(c))));
        }
    };

    Css::SpecifiedBorder _value;

    BorderRightProperty(Rc<Property::Registration> registration, Css::SpecifiedBorder value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderRightWidthProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_WIDTH, {}).expect(), _value.width),
            makeRc<BorderRightStyleProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_STYLE, {}).expect(), _value.style),
            makeRc<BorderRightColorProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_COLOR, {}).expect(), _value.color),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderBottomProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_BOTTOM, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<BorderBottomProperty>(self(), Css::SpecifiedBorder{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomProperty>(self(), c.borders->bottom);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomProperty>(self(), try$(parseValue<Css::SpecifiedBorder>(c))));
        }
    };

    Css::SpecifiedBorder _value;

    BorderBottomProperty(Rc<Property::Registration> registration, Css::SpecifiedBorder value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderBottomWidthProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_WIDTH, {}).expect(), _value.width),
            makeRc<BorderBottomStyleProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_STYLE, {}).expect(), _value.style),
            makeRc<BorderBottomColorProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_COLOR, {}).expect(), _value.color),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderLeftProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_LEFT, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<BorderLeftProperty>(self(), Css::SpecifiedBorder{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderLeftProperty>(self(), c.borders->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftProperty>(self(), try$(parseValue<Css::SpecifiedBorder>(c))));
        }
    };

    Css::SpecifiedBorder _value;

    BorderLeftProperty(Rc<Property::Registration> registration, Css::SpecifiedBorder value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderLeftWidthProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_WIDTH, {}).expect(), _value.width),
            makeRc<BorderLeftStyleProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_STYLE, {}).expect(), _value.style),
            makeRc<BorderLeftColorProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_COLOR, {}).expect(), _value.color),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<BorderProperty>(self(), Css::SpecifiedBorder{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderProperty>(self(), c.borders->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderProperty>(self(), try$(parseValue<Css::SpecifiedBorder>(c))));
        }
    };

    Css::SpecifiedBorder _value;

    BorderProperty(Rc<Property::Registration> registration, Css::SpecifiedBorder value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderTopWidthProperty>(registry.resolveRegistration(Properties::BORDER_TOP_WIDTH, {}).expect(), _value.width),
            makeRc<BorderTopStyleProperty>(registry.resolveRegistration(Properties::BORDER_TOP_STYLE, {}).expect(), _value.style),
            makeRc<BorderTopColorProperty>(registry.resolveRegistration(Properties::BORDER_TOP_COLOR, {}).expect(), _value.color),

            makeRc<BorderRightWidthProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_WIDTH, {}).expect(), _value.width),
            makeRc<BorderRightStyleProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_STYLE, {}).expect(), _value.style),
            makeRc<BorderRightColorProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_COLOR, {}).expect(), _value.color),

            makeRc<BorderBottomWidthProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_WIDTH, {}).expect(), _value.width),
            makeRc<BorderBottomStyleProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_STYLE, {}).expect(), _value.style),
            makeRc<BorderBottomColorProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_COLOR, {}).expect(), _value.color),

            makeRc<BorderLeftWidthProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_WIDTH, {}).expect(), _value.width),
            makeRc<BorderLeftStyleProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_STYLE, {}).expect(), _value.style),
            makeRc<BorderLeftColorProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_COLOR, {}).expect(), _value.color),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderWidthProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_WIDTH, SHORTHAND) {}

        Rc<Property> initial() const override {
            return makeRc<BorderWidthProperty>(self(), Math::Insets{Css::LineWidth{Keywords::MEDIUM}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderWidthProperty>(
                self(),
                Math::Insets<Css::LineWidth>{
                    toSpecified(c.borders->top.width),
                    toSpecified(c.borders->end.width),
                    toSpecified(c.borders->bottom.width),
                    toSpecified(c.borders->start.width),
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderWidthProperty>(self(), try$(parseValue<Math::Insets<Css::LineWidth>>(c))));
        }
    };

    Math::Insets<Css::LineWidth> _value;

    BorderWidthProperty(Rc<Property::Registration> registration, Math::Insets<Css::LineWidth> value)
        : Property(registration), _value(value) {}

    Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderTopWidthProperty>(registry.resolveRegistration(Properties::BORDER_TOP_WIDTH, {}).expect(), _value.top),
            makeRc<BorderRightWidthProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_WIDTH, {}).expect(), _value.end),
            makeRc<BorderBottomWidthProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_WIDTH, {}).expect(), _value.bottom),
            makeRc<BorderLeftWidthProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_WIDTH, {}).expect(), _value.start),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// MARK: Borders - Table -------------------------------------------------------

// https://www.w3.org/TR/CSS22/tables.html#propdef-border-collapse
export struct BorderCollapseProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_COLLAPSE, {INHERITED, BULK_INHERITED}) {}

        Rc<Property> initial() const override {
            return makeRc<BorderCollapseProperty>(self(), Keywords::SEPARATE);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderCollapseProperty>(self(), c.table->collapse);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderCollapseProperty>(self(), try$(parseValue<Css::BorderCollapse>(c))));
        }
    };

    Css::BorderCollapse _value;

    BorderCollapseProperty(Rc<Property::Registration> registration, Css::BorderCollapse value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.table.cow().collapse = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/tables.html#propdef-border-spacing
export struct BorderSpacingProperty : Property {
    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_SPACING, {INHERITED, BULK_INHERITED}) {}

        Rc<Property> initial() const override {
            return makeRc<BorderSpacingProperty>(self(), Vaev::Css::BorderSpacing{Css::Length::fromCanonical(0), Css::Length::fromCanonical(0)});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderSpacingProperty>(self(), toSpecified(c.table->spacing));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderSpacingProperty>(self(), try$(parseValue<Vaev::Css::BorderSpacing>(c))));
        }
    };

    Vaev::Css::BorderSpacing _value;

    BorderSpacingProperty(Rc<Property::Registration> registration, Vaev::Css::BorderSpacing value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.table.cow().spacing = resolve(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
