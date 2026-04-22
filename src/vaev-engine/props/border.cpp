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
        Symbol name() const override {
            return Properties::BORDER_TOP_COLOR;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderTopColorProperty>(self(), Keywords::CURRENT_COLOR);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopColorProperty>(self(), valueFromComputed<Color>(c.borders->top.color));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopColorProperty>(self(), try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    BorderTopColorProperty(Rc<Property::Registration> registration, Color value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const& ctx) const override {
        c.borders.cow().top.color = computeValue(_value, ctx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
export struct BorderRightColorProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_RIGHT_COLOR;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRightColorProperty>(self(), Keywords::CURRENT_COLOR);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRightColorProperty>(self(), valueFromComputed<Color>(c.borders->right.color));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightColorProperty>(self(), try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    BorderRightColorProperty(Rc<Property::Registration> registration, Color value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const& ctx) const override {
        c.borders.cow().right.color = computeValue(_value, ctx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
export struct BorderBottomColorProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_BOTTOM_COLOR;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderBottomColorProperty>(self(), Keywords::CURRENT_COLOR);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomColorProperty>(self(), valueFromComputed<Color>(c.borders->bottom.color));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomColorProperty>(self(), try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    BorderBottomColorProperty(Rc<Property::Registration> registration, Color value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const& ctx) const override {
        c.borders.cow().bottom.color = computeValue(_value, ctx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#propdef-border-color
export struct BorderLeftColorProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_LEFT_COLOR;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderLeftColorProperty>(self(), Keywords::CURRENT_COLOR);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderLeftColorProperty>(self(), valueFromComputed<Color>(c.borders->left.color));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftColorProperty>(self(), try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    BorderLeftColorProperty(Rc<Property::Registration> registration, Color value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const& ctx) const override {
        c.borders.cow().left.color = computeValue(_value, ctx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

export struct BorderColorProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_COLOR;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<BorderColorProperty>(self(), Edges<Color>{Keywords::CURRENT_COLOR});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderColorProperty>(
                self(),
                Edges{
                    valueFromComputed<Color>(c.borders->top.color),
                    valueFromComputed<Color>(c.borders->right.color),
                    valueFromComputed<Color>(c.borders->bottom.color),
                    valueFromComputed<Color>(c.borders->left.color),
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderColorProperty>(self(), try$(parseValue<Edges<Color>>(c))));
        }
    };

    Edges<Color> _value;

    BorderColorProperty(Rc<Property::Registration> registration, Edges<Color> value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderTopColorProperty>(registry.resolveRegistration(Properties::BORDER_TOP_COLOR, {}).unwrap(), _value.top),
            makeRc<BorderRightColorProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_COLOR, {}).unwrap(), _value.right),
            makeRc<BorderBottomColorProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_COLOR, {}).unwrap(), _value.bottom),
            makeRc<BorderLeftColorProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_COLOR, {}).unwrap(), _value.left),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderLeftStyleProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_LEFT_STYLE;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderLeftStyleProperty>(self(), Gfx::BorderStyle::NONE);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderLeftStyleProperty>(self(), valueFromComputed<Gfx::BorderStyle>(c.borders->left.style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftStyleProperty>(self(), try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderLeftStyleProperty(Rc<Property::Registration> registration, Gfx::BorderStyle value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const& ctx) const override {
        c.borders.cow().left.style = computeValue(_value, ctx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderTopStyleProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_TOP_STYLE;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderTopStyleProperty>(self(), Gfx::BorderStyle::NONE);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopStyleProperty>(self(), valueFromComputed<Gfx::BorderStyle>(c.borders->top.style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopStyleProperty>(self(), try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderTopStyleProperty(Rc<Property::Registration> registration, Gfx::BorderStyle value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const& ctx) const override {
        c.borders.cow().top.style = computeValue(_value, ctx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderRightStyleProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_RIGHT_STYLE;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRightStyleProperty>(self(), Gfx::BorderStyle::NONE);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRightStyleProperty>(self(), valueFromComputed<Gfx::BorderStyle>(c.borders->right.style));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightStyleProperty>(self(), try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderRightStyleProperty(Rc<Property::Registration> registration, Gfx::BorderStyle value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const& ctx) const override {
        c.borders.cow().right.style = computeValue(_value, ctx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderBottomStyleProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_BOTTOM_STYLE;
        }

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

    void apply(ComputedValues& c, ComputationContext const&) const override {
        c.borders.cow().bottom.style = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties

export struct BorderStyleProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_STYLE;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<BorderStyleProperty>(self(), Edges{Gfx::BorderStyle::NONE});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderStyleProperty>(
                self(),
                Edges{
                    c.borders->top.style,
                    c.borders->right.style,
                    c.borders->bottom.style,
                    c.borders->left.style,
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderStyleProperty>(self(), try$(parseValue<Edges<Gfx::BorderStyle>>(c))));
        }
    };

    Edges<Gfx::BorderStyle> _value;

    BorderStyleProperty(Rc<Property::Registration> registration, Edges<Gfx::BorderStyle> value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderTopStyleProperty>(registry.resolveRegistration(Properties::BORDER_TOP_STYLE, {}).unwrap(), _value.top),
            makeRc<BorderRightStyleProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_STYLE, {}).unwrap(), _value.right),
            makeRc<BorderBottomStyleProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_STYLE, {}).unwrap(), _value.bottom),
            makeRc<BorderLeftStyleProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_STYLE, {}).unwrap(), _value.left),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderTopWidthProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_TOP_WIDTH;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderTopWidthProperty>(self(), Keywords::MEDIUM);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopWidthProperty>(self(), valueFromComputed<LineWidth>(c.borders->top.width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopWidthProperty>(self(), try$(parseValue<LineWidth>(c))));
        }
    };

    LineWidth _value;

    BorderTopWidthProperty(Rc<Property::Registration> registration, LineWidth value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const& ctx) const override {
        c.borders.cow().top.width = computeValue(_value, ctx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderRightWidthProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_RIGHT_WIDTH;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRightWidthProperty>(self(), Keywords::MEDIUM);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRightWidthProperty>(self(), valueFromComputed<LineWidth>(c.borders->right.width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightWidthProperty>(self(), try$(parseValue<LineWidth>(c))));
        }
    };

    LineWidth _value;

    BorderRightWidthProperty(Rc<Property::Registration> registration, LineWidth value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const& ctx) const override {
        c.borders.cow().right.width = computeValue(_value, ctx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderBottomWidthProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_BOTTOM_WIDTH;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderBottomWidthProperty>(self(), Keywords::MEDIUM);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomWidthProperty>(self(), valueFromComputed<LineWidth>(c.borders->bottom.width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomWidthProperty>(self(), try$(parseValue<LineWidth>(c))));
        }
    };

    LineWidth _value;

    BorderBottomWidthProperty(Rc<Property::Registration> registration, LineWidth value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const& ctx) const override {
        c.borders.cow().bottom.width = computeValue(_value, ctx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderLeftWidthProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_LEFT_WIDTH;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderLeftWidthProperty>(self(), Keywords::MEDIUM);
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderLeftWidthProperty>(self(), valueFromComputed<LineWidth>(c.borders->left.width));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftWidthProperty>(self(), try$(parseValue<LineWidth>(c))));
        }
    };

    LineWidth _value;

    BorderLeftWidthProperty(Rc<Property::Registration> registration, LineWidth value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const& ctx) const override {
        c.borders.cow().left.width = computeValue(_value, ctx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderRadiusTopRightProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_TOP_RIGHT_RADIUS;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRadiusTopRightProperty>(self(), BorderRadius{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRadiusTopRightProperty>(self(), valueFromComputed<BorderRadius>(c.borders->radii.topRight));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRadiusTopRightProperty>(self(), try$(parseValue<BorderRadius>(c))));
        }
    };

    BorderRadius _value;

    BorderRadiusTopRightProperty(Rc<Property::Registration> registration, Pair<CalcValue<PercentOr<Length>>> value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const& ctx) const override {
        c.borders.cow().radii.topRight = computeValue(_value, ctx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderRadiusTopLeftProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_TOP_LEFT_RADIUS;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRadiusTopLeftProperty>(self(), BorderRadius{Length{}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRadiusTopLeftProperty>(self(), valueFromComputed<BorderRadius>(c.borders->radii.topRight));
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRadiusTopLeftProperty>(self(), try$(parseValue<BorderRadius>(c))));
        }
    };

    BorderRadius _value;

    BorderRadiusTopLeftProperty(Rc<Property::Registration> registration, BorderRadius value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const& ctx) const override {
        c.borders.cow().radii.topLeft = computeValue(_value, ctx);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderRadiusBottomRightProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_BOTTOM_RIGHT_RADIUS;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRadiusBottomRightProperty>(self(), makeArray<CalcValue<PercentOr<Length>>, 2>(Length{}));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRadiusBottomRightProperty>(self(), Array{
                                                                       c.borders->radii.e,
                                                                       c.borders->radii.f,
                                                                   });
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto first = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            Array value{first, first};
            if (not c.ended())
                value[1] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            return Ok(makeRc<BorderRadiusBottomRightProperty>(self(), value));
        }
    };

    Array<CalcValue<PercentOr<Length>>, 2> _value;

    BorderRadiusBottomRightProperty(Rc<Property::Registration> registration, Array<CalcValue<PercentOr<Length>>, 2> value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const&) const override {
        c.borders.cow().radii.e = _value[1];
        c.borders.cow().radii.f = _value[0];
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderRadiusBottomLeftProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_BOTTOM_LEFT_RADIUS;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRadiusBottomLeftProperty>(self(), makeArray<CalcValue<PercentOr<Length>>, 2>(Length{}));
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRadiusBottomLeftProperty>(
                self(),
                Array{
                    c.borders->radii.g,
                    c.borders->radii.h,
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto first = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            Array value{first, first};
            if (not c.ended())
                value[1] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            return Ok(makeRc<BorderRadiusBottomLeftProperty>(self(), value));
        }
    };

    Array<CalcValue<PercentOr<Length>>, 2> _value;

    BorderRadiusBottomLeftProperty(Rc<Property::Registration> registration, Array<CalcValue<PercentOr<Length>>, 2> value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const&) const override {
        c.borders.cow().radii.g = _value[0];
        c.borders.cow().radii.h = _value[1];
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderRadiusProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_RADIUS;
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRadiusProperty>(self(), Math::Radii{CalcValue<PercentOr<Length>>(Length{})});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRadiusProperty>(self(), c.borders->radii);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRadiusProperty>(self(), try$(parseValue<Math::Radii<CalcValue<PercentOr<Length>>>>(c))));
        }
    };

    Math::Radii<CalcValue<PercentOr<Length>>> _value;

    BorderRadiusProperty(Rc<Property::Registration> registration, Math::Radii<CalcValue<PercentOr<Length>>> value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const&) const override {
        c.borders.cow().radii = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderTopProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_TOP;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<BorderTopProperty>(self(), Border{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderTopProperty>(self(), c.borders->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Border value;
            while (not c.ended()) {
                auto width = parseValue<CalcValue<Length>>(c);
                if (width) {
                    value.width = width.unwrap();
                    continue;
                }

                auto color = parseValue<Color>(c);
                if (color) {
                    value.color = color.unwrap();
                    continue;
                }

                auto style = parseValue<Gfx::BorderStyle>(c);
                if (style) {
                    value.style = style.unwrap();
                    continue;
                }

                break;
            }
            return Ok(makeRc<BorderTopProperty>(self(), value));
        }
    };

    Border _value;

    BorderTopProperty(Rc<Property::Registration> registration, Border value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderTopWidthProperty>(registry.resolveRegistration(Properties::BORDER_TOP_WIDTH, {}).unwrap(), _value.width),
            makeRc<BorderTopStyleProperty>(registry.resolveRegistration(Properties::BORDER_TOP_STYLE, {}).unwrap(), _value.style),
            makeRc<BorderTopColorProperty>(registry.resolveRegistration(Properties::BORDER_TOP_COLOR, {}).unwrap(), _value.color),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderRightProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_RIGHT;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<BorderRightProperty>(self(), Border{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderRightProperty>(self(), c.borders->end);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Border value;
            while (not c.ended()) {
                auto width = parseValue<CalcValue<Length>>(c);
                if (width) {
                    value.width = width.unwrap();
                    continue;
                }

                auto color = parseValue<Color>(c);
                if (color) {
                    value.color = color.unwrap();
                    continue;
                }

                auto style = parseValue<Gfx::BorderStyle>(c);
                if (style) {
                    value.style = style.unwrap();
                    continue;
                }

                break;
            }
            return Ok(makeRc<BorderRightProperty>(self(), value));
        }
    };

    Border _value;

    BorderRightProperty(Rc<Property::Registration> registration, Border value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderRightWidthProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_WIDTH, {}).unwrap(), _value.width),
            makeRc<BorderRightStyleProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_STYLE, {}).unwrap(), _value.style),
            makeRc<BorderRightColorProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_COLOR, {}).unwrap(), _value.color),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderBottomProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_BOTTOM;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<BorderBottomProperty>(self(), Border{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderBottomProperty>(self(), c.borders->bottom);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Border value;
            while (not c.ended()) {
                auto width = parseValue<CalcValue<Length>>(c);
                if (width) {
                    value.width = width.unwrap();
                    continue;
                }

                auto color = parseValue<Color>(c);
                if (color) {
                    value.color = color.unwrap();
                    continue;
                }

                auto style = parseValue<Gfx::BorderStyle>(c);
                if (style) {
                    value.style = style.unwrap();
                    continue;
                }

                break;
            }
            return Ok(makeRc<BorderBottomProperty>(self(), value));
        }
    };

    Border _value;

    BorderBottomProperty(Rc<Property::Registration> registration, Border value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderBottomWidthProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_WIDTH, {}).unwrap(), _value.width),
            makeRc<BorderBottomStyleProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_STYLE, {}).unwrap(), _value.style),
            makeRc<BorderBottomColorProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_COLOR, {}).unwrap(), _value.color),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderLeftProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_LEFT;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<BorderLeftProperty>(self(), Border{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderLeftProperty>(self(), c.borders->start);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            Border value;
            while (not c.ended()) {
                auto width = parseValue<CalcValue<Length>>(c);
                if (width) {
                    value.width = width.unwrap();
                    continue;
                }

                auto color = parseValue<Color>(c);
                if (color) {
                    value.color = color.unwrap();
                    continue;
                }

                auto style = parseValue<Gfx::BorderStyle>(c);
                if (style) {
                    value.style = style.unwrap();
                    continue;
                }

                break;
            }
            return Ok(makeRc<BorderLeftProperty>(self(), value));
        }
    };

    Border _value;

    BorderLeftProperty(Rc<Property::Registration> registration, Border value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderLeftWidthProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_WIDTH, {}).unwrap(), _value.width),
            makeRc<BorderLeftStyleProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_STYLE, {}).unwrap(), _value.style),
            makeRc<BorderLeftColorProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_COLOR, {}).unwrap(), _value.color),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<BorderProperty>(self(), Border{});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderProperty>(self(), c.borders->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderProperty>(self(), try$(parseValue<Border>(c))));
        }
    };

    Border _value;

    BorderProperty(Rc<Property::Registration> registration, Border value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderTopWidthProperty>(registry.resolveRegistration(Properties::BORDER_TOP_WIDTH, {}).unwrap(), _value.width),
            makeRc<BorderTopStyleProperty>(registry.resolveRegistration(Properties::BORDER_TOP_STYLE, {}).unwrap(), _value.style),
            makeRc<BorderTopColorProperty>(registry.resolveRegistration(Properties::BORDER_TOP_COLOR, {}).unwrap(), _value.color),

            makeRc<BorderRightWidthProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_WIDTH, {}).unwrap(), _value.width),
            makeRc<BorderRightStyleProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_STYLE, {}).unwrap(), _value.style),
            makeRc<BorderRightColorProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_COLOR, {}).unwrap(), _value.color),

            makeRc<BorderBottomWidthProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_WIDTH, {}).unwrap(), _value.width),
            makeRc<BorderBottomStyleProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_STYLE, {}).unwrap(), _value.style),
            makeRc<BorderBottomColorProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_COLOR, {}).unwrap(), _value.color),

            makeRc<BorderLeftWidthProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_WIDTH, {}).unwrap(), _value.width),
            makeRc<BorderLeftStyleProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_STYLE, {}).unwrap(), _value.style),
            makeRc<BorderLeftColorProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_COLOR, {}).unwrap(), _value.color),
        };
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderWidthProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_WIDTH;
        }

        Flags<Options> flags() const override {
            return {SHORTHAND_PROPERTY};
        }

        Rc<Property> initial() const override {
            return makeRc<BorderWidthProperty>(self(), Math::Insets{LineWidth{Keywords::MEDIUM}});
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderWidthProperty>(
                self(),
                Math::Insets{
                    c.borders->start.width,
                    c.borders->end.width,
                    c.borders->top.width,
                    c.borders->bottom.width,
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderWidthProperty>(self(), try$(parseValue<Math::Insets<LineWidth>>(c))));
        }
    };

    Math::Insets<LineWidth> _value;

    BorderWidthProperty(Rc<Property::Registration> registration, Math::Insets<LineWidth> value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
        return {
            makeRc<BorderTopWidthProperty>(registry.resolveRegistration(Properties::BORDER_TOP_WIDTH, {}).unwrap(), _value.top),
            makeRc<BorderRightWidthProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_WIDTH, {}).unwrap(), _value.end),
            makeRc<BorderBottomWidthProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_WIDTH, {}).unwrap(), _value.bottom),
            makeRc<BorderLeftWidthProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_WIDTH, {}).unwrap(), _value.start),
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
        Symbol name() const override {
            return Properties::BORDER_COLLAPSE;
        }

        Flags<Options> flags() const override {
            return {INHERITED};
        }

        Rc<Property> initial() const override {
            return makeRc<BorderCollapseProperty>(self(), BorderCollapse::SEPARATE);
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.table.cow().collapse = parent.table->collapse;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderCollapseProperty>(self(), c.table->collapse);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderCollapseProperty>(self(), try$(parseValue<BorderCollapse>(c))));
        }
    };

    BorderCollapse _value;

    BorderCollapseProperty(Rc<Property::Registration> registration, BorderCollapse value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const&) const override {
        c.table.cow().collapse = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/tables.html#propdef-border-spacing
export struct BorderSpacingProperty : Property {
    struct Registration : Property::Registration {
        Symbol name() const override {
            return Properties::BORDER_SPACING;
        }

        Flags<Options> flags() const override {
            return {INHERITED};
        }

        Rc<Property> initial() const override {
            return makeRc<BorderSpacingProperty>(self(), BorderSpacing{0_au, 0_au});
        }

        void inherit(ComputedValues const& parent, ComputedValues& child) const override {
            child.table.cow().spacing = parent.table->spacing;
        }

        Rc<Property> load(ComputedValues const& c) const override {
            return makeRc<BorderSpacingProperty>(self(), c.table->spacing);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderSpacingProperty>(self(), try$(parseValue<BorderSpacing>(c))));
        }
    };

    BorderSpacing _value;

    BorderSpacingProperty(Rc<Property::Registration> registration, BorderSpacing value)
        : Property(registration), _value(value) {}

    void apply(ComputedValues& c, ComputationContext const&) const override {
        c.table.cow().spacing = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
