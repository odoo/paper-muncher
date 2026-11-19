module;

#include <karm/macros>

export module Vaev.Engine:props.border;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.computed;
import :style.values.keywords;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

// MARK: Borders ---------------------------------------------------------------

// https://drafts.csswg.org/css-backgrounds/#propdef-border-top-color
export struct BorderTopColorProperty : Property {
    using Value = Css::Color;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_TOP_COLOR) {}

        InitialValue initialValue() const override {
            return "currentColor"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopColorProperty>(self(), specifyValue<Value>(c.borders->top.color));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopColorProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderTopColorProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().top.color = computeValue(_value, c.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#propdef-border-right-color
export struct BorderRightColorProperty : Property {
    using Value = Css::Color;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_RIGHT_COLOR) {}

        InitialValue initialValue() const override {
            return "currentColor"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRightColorProperty>(self(), specifyValue<Value>(c.borders->end.color));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightColorProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderRightColorProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().end.color = computeValue(_value, c.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#propdef-border-bottom-color
export struct BorderBottomColorProperty : Property {
    using Value = Css::Color;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_BOTTOM_COLOR) {}

        InitialValue initialValue() const override {
            return "currentColor"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomColorProperty>(self(), specifyValue<Value>(c.borders->bottom.color));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomColorProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderBottomColorProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().bottom.color = computeValue(_value, c.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#propdef-border-left-color
export struct BorderLeftColorProperty : Property {
    using Value = Css::Color;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_LEFT_COLOR) {}

        InitialValue initialValue() const override {
            return "currentColor"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderLeftColorProperty>(self(), specifyValue<Value>(c.borders->start.color));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftColorProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderLeftColorProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().start.color = computeValue(_value, c.color);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct BorderColorProperty : Property {
    using Value = Math::Insets<Css::Color>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_COLOR, SHORTHAND) {}

        InitialValue initialValue() const override {
            return "currentColor"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderColorProperty>(
                self(),
                Math::Insets{
                    specifyValue<Css::Color>(c.borders->top.color),
                    specifyValue<Css::Color>(c.borders->end.color),
                    specifyValue<Css::Color>(c.borders->bottom.color),
                    specifyValue<Css::Color>(c.borders->start.color),
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderColorProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderColorProperty(Rc<Property::Registration> registration, Value value)
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

// https://drafts.csswg.org/css-backgrounds/#propdef-border-left-style
export struct BorderLeftStyleProperty : Property {
    using Value = Gfx::BorderStyle;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_LEFT_STYLE) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderLeftStyleProperty>(self(), c.borders->start.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftStyleProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderLeftStyleProperty(Rc<Property::Registration> registration, Value value)
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

// https://drafts.csswg.org/css-backgrounds/#propdef-border-top-style
export struct BorderTopStyleProperty : Property {
    using Value = Gfx::BorderStyle;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_TOP_STYLE) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopStyleProperty>(self(), c.borders->top.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopStyleProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderTopStyleProperty(Rc<Property::Registration> registration, Value value)
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

// https://drafts.csswg.org/css-backgrounds/#propdef-border-right-style
export struct BorderRightStyleProperty : Property {
    using Value = Gfx::BorderStyle;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_RIGHT_STYLE) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRightStyleProperty>(self(), c.borders->end.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightStyleProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderRightStyleProperty(Rc<Property::Registration> registration, Value value)
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

// https://drafts.csswg.org/css-backgrounds/#propdef-border-bottom-style
export struct BorderBottomStyleProperty : Property {
    using Value = Gfx::BorderStyle;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_BOTTOM_STYLE) {}

        InitialValue initialValue() const override {
            return "none"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomStyleProperty>(self(), c.borders->bottom.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomStyleProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderBottomStyleProperty(Rc<Property::Registration> registration, Value value)
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

// https://drafts.csswg.org/css-backgrounds/#propdef-border-style
export struct BorderStyleProperty : Property {
    using Value = Math::Insets<Gfx::BorderStyle>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_STYLE, SHORTHAND) {}

        InitialValue initialValue() const override {
            return "none"s;
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
            return Ok(makeRc<BorderStyleProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderStyleProperty(Rc<Property::Registration> registration, Value value)
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

export struct BorderTopWidthProperty : Property {
    using Value = Css::LineWidth;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_TOP_WIDTH) {}

        InitialValue initialValue() const override {
            return "medium"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopWidthProperty>(self(), specifyValue<Value>(c.borders->top.width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopWidthProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderTopWidthProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().top.width = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderRightWidthProperty : Property {
    using Value = Css::LineWidth;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_RIGHT_WIDTH) {}

        InitialValue initialValue() const override {
            return "medium"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRightWidthProperty>(self(), specifyValue<Value>(c.borders->end.width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightWidthProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderRightWidthProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().end.width = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderBottomWidthProperty : Property {
    using Value = Css::LineWidth;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_BOTTOM_WIDTH) {}

        InitialValue initialValue() const override {
            return "medium"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomWidthProperty>(self(), specifyValue<Value>(c.borders->bottom.width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomWidthProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderBottomWidthProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().bottom.width = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderLeftWidthProperty : Property {
    using Value = Css::LineWidth;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_LEFT_WIDTH) {}

        InitialValue initialValue() const override {
            return "medium"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderLeftWidthProperty>(self(), specifyValue<Value>(c.borders->start.width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftWidthProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderLeftWidthProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().start.width = computeValue(_value, cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderTopRightRadiusProperty : Property {
    using Value = Array<Css::LengthPercentage, 2>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_TOP_RIGHT_RADIUS) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopRightRadiusProperty>(
                self(),
                Array{
                    specifyValue<Css::LengthPercentage>(c.borders->radii.c),
                    specifyValue<Css::LengthPercentage>(c.borders->radii.d),
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto first = try$(parseValue<Css::LengthPercentage>(c));
            Array value{first, first};
            if (not c.ended())
                value[1] = try$(parseValue<Css::LengthPercentage>(c));
            return Ok(makeRc<BorderTopRightRadiusProperty>(self(), value));
        }
    };

    Value _value;

    BorderTopRightRadiusProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().radii.c = computeValue(_value[0], cx);
        c.borders.cow().radii.d = computeValue(_value[1], cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderTopLeftRadiusProperty : Property {
    using Value = Array<Css::LengthPercentage, 2>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_TOP_LEFT_RADIUS) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopLeftRadiusProperty>(
                self(),
                Array{
                    specifyValue<Css::LengthPercentage>(c.borders->radii.b),
                    specifyValue<Css::LengthPercentage>(c.borders->radii.a),
                }
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

    Value _value;

    BorderTopLeftRadiusProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().radii.a = computeValue(_value[1], cx);
        c.borders.cow().radii.b = computeValue(_value[0], cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderBottomRightRadiusProperty : Property {
    using Value = Array<Css::LengthPercentage, 2>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_BOTTOM_RIGHT_RADIUS) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomRightRadiusProperty>(
                self(),
                Array{
                    specifyValue<Css::LengthPercentage>(c.borders->radii.f),
                    specifyValue<Css::LengthPercentage>(c.borders->radii.e),
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto first = try$(parseValue<Css::LengthPercentage>(c));
            Array value{first, first};
            if (not c.ended())
                value[1] = try$(parseValue<Css::LengthPercentage>(c));
            return Ok(makeRc<BorderBottomRightRadiusProperty>(self(), value));
        }
    };

    Value _value;

    BorderBottomRightRadiusProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().radii.e = computeValue(_value[1], cx);
        c.borders.cow().radii.f = computeValue(_value[0], cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderBottomLeftRadiusProperty : Property {
    using Value = Array<Css::LengthPercentage, 2>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_BOTTOM_LEFT_RADIUS) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomLeftRadiusProperty>(
                self(),
                Array{
                    specifyValue<Css::LengthPercentage>(c.borders->radii.g),
                    specifyValue<Css::LengthPercentage>(c.borders->radii.h),
                }
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

    Value _value;

    BorderBottomLeftRadiusProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().radii.g = computeValue(_value[0], cx);
        c.borders.cow().radii.h = computeValue(_value[1], cx);
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderRadiusProperty : Property {
    using Value = Math::Radii<Css::LengthPercentage>;

    struct Registration : Property::Registration {
        Registration() : Property::Registration(Properties::BORDER_RADIUS, SHORTHAND) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRadiusProperty>(self(), specifyValue<Value>(c.borders->radii));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto radii = try$(parseValue<Math::Radii<Css::LengthPercentage>>(c));
            return Ok(makeRc<BorderRadiusProperty>(self(), radii));
        }
    };

    Value _value;

    BorderRadiusProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().radii = computeValue(_value, cx);
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
    using Value = Css::Border;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_TOP, SHORTHAND) {}

        InitialValue initialValue() const override {
            return "medium none currentcolor"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopProperty>(self(), specifyValue<Value>(c.borders->top));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderTopProperty(Rc<Property::Registration> registration, Value value)
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
    using Value = BorderTopProperty::Value;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_RIGHT, SHORTHAND) {}

        InitialValue initialValue() const override {
            return "medium none currentcolor"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRightProperty>(self(), specifyValue<Value>(c.borders->end));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderRightProperty(Rc<Property::Registration> registration, Value value)
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
    using Value = BorderTopProperty::Value;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_BOTTOM, SHORTHAND) {}

        InitialValue initialValue() const override {
            return "medium none currentcolor"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomProperty>(self(), specifyValue<Value>(c.borders->bottom));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderBottomProperty(Rc<Property::Registration> registration, Value value)
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
    using Value = BorderTopProperty::Value;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_LEFT, SHORTHAND) {}

        InitialValue initialValue() const override {
            return "medium none currentcolor"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderLeftProperty>(self(), specifyValue<Value>(c.borders->start));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderLeftProperty(Rc<Property::Registration> registration, Value value)
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
    using Value = BorderTopProperty::Value;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER, SHORTHAND) {}

        InitialValue initialValue() const override {
            return "medium none currentcolor"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderProperty>(self(), specifyValue<Value>(c.borders->top));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderProperty(Rc<Property::Registration> registration, Value value)
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
    using Value = Math::Insets<Css::LineWidth>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_WIDTH, SHORTHAND) {}

        InitialValue initialValue() const override {
            return "medium"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderWidthProperty>(
                self(),
                Math::Insets<Css::LineWidth>{
                    specifyValue<Css::LineWidth>(c.borders->top.width),
                    specifyValue<Css::LineWidth>(c.borders->end.width),
                    specifyValue<Css::LineWidth>(c.borders->bottom.width),
                    specifyValue<Css::LineWidth>(c.borders->start.width),
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderWidthProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderWidthProperty(Rc<Property::Registration> registration, Value value)
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

// https://drafts.csswg.org/css-tables-3/#border-collapse-property
export struct BorderCollapseProperty : Property {
    using Value = Css::BorderCollapse;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_COLLAPSE, {INHERITED, BULK_INHERITED}) {}

        InitialValue initialValue() const override {
            return "separate"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderCollapseProperty>(self(), c.table->collapse);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderCollapseProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderCollapseProperty(Rc<Property::Registration> registration, Value value)
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

// https://drafts.csswg.org/css-tables-3/#propdef-border-spacing
export struct BorderSpacingProperty : Property {
    using Value = Pair<Css::Length>;

    struct Registration : Property::Registration {
        Registration()
            : Property::Registration(Properties::BORDER_SPACING, {INHERITED, BULK_INHERITED}) {}

        InitialValue initialValue() const override {
            return "0"s;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderSpacingProperty>(
                self(),
                Value{
                    specifyValue<Css::Length>(c.table->spacing.horizontal),
                    specifyValue<Css::Length>(c.table->spacing.vertical),
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderSpacingProperty>(self(), try$(parseValue<Value>(c))));
        }
    };

    Value _value;

    BorderSpacingProperty(Rc<Property::Registration> registration, Value value)
        : Property(registration), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.table.cow().spacing = {
            computeValue(_value.v0, cx),
            computeValue(_value.v1, cx),
        };
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return Css::serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
