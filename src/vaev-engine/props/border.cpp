module;

#include <karm-core/macros.h>

export module Vaev.Engine:props.border;

import Karm.Core;
import :props.base;
import :css.parser;
import :style.specified;

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
            return makeRc<BorderTopColorProperty>(self(), BLACK);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderTopColorProperty>(self(), c.borders->top.color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopColorProperty>(self(), try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    BorderTopColorProperty(Rc<Property::Registration> registration, Color value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().top.color = _value;
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
            return makeRc<BorderRightColorProperty>(self(), BLACK);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRightColorProperty>(self(), c.borders->end.color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightColorProperty>(self(), try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    BorderRightColorProperty(Rc<Property::Registration> registration, Color value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().end.color = _value;
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
            return makeRc<BorderBottomColorProperty>(self(), BLACK);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderBottomColorProperty>(self(), c.borders->bottom.color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomColorProperty>(self(), try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    BorderBottomColorProperty(Rc<Property::Registration> registration, Color value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().bottom.color = _value;
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
            return makeRc<BorderLeftColorProperty>(self(), BLACK);
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderLeftColorProperty>(self(), c.borders->start.color);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftColorProperty>(self(), try$(parseValue<Color>(c))));
        }
    };

    Color _value;

    BorderLeftColorProperty(Rc<Property::Registration> registration, Color value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().start.color = _value;
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
            return makeRc<BorderColorProperty>(self(), Math::Insets<Color>{BLACK});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderColorProperty>(
                self(),
                Math::Insets{
                    c.borders->start.color,
                    c.borders->end.color,
                    c.borders->top.color,
                    c.borders->bottom.color,
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderColorProperty>(self(), try$(parseValue<Math::Insets<Color>>(c))));
        }
    };

    Math::Insets<Color> _value;

    BorderColorProperty(Rc<Property::Registration> registration, Math::Insets<Color> value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
        return {
            makeRc<BorderTopColorProperty>(registry.resolveRegistration(Properties::BORDER_TOP_COLOR, {}).unwrap(), _value.top),
            makeRc<BorderRightColorProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_COLOR, {}).unwrap(), _value.end),
            makeRc<BorderBottomColorProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_COLOR, {}).unwrap(), _value.bottom),
            makeRc<BorderLeftColorProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_COLOR, {}).unwrap(), _value.start),
        };
    }

    void apply(SpecifiedValues& c) const override {
        auto& borders = c.borders.cow();
        borders.start.color = _value.start;
        borders.end.color = _value.end;
        borders.top.color = _value.top;
        borders.bottom.color = _value.bottom;
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderLeftStyleProperty>(self(), c.borders->start.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftStyleProperty>(self(), try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderLeftStyleProperty(Rc<Property::Registration> registration, Gfx::BorderStyle value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().start.style = _value;
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderTopStyleProperty>(self(), c.borders->top.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopStyleProperty>(self(), try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderTopStyleProperty(Rc<Property::Registration> registration, Gfx::BorderStyle value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().top.style = _value;
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRightStyleProperty>(self(), c.borders->end.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightStyleProperty>(self(), try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderRightStyleProperty(Rc<Property::Registration> registration, Gfx::BorderStyle value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().end.style = _value;
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderBottomStyleProperty>(self(), c.borders->bottom.style);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomStyleProperty>(self(), try$(parseValue<Gfx::BorderStyle>(c))));
        }
    };

    Gfx::BorderStyle _value;

    BorderBottomStyleProperty(Rc<Property::Registration> registration, Gfx::BorderStyle value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
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
            return makeRc<BorderStyleProperty>(self(), Math::Insets{Gfx::BorderStyle::NONE});
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
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

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
        return {
            makeRc<BorderTopStyleProperty>(registry.resolveRegistration(Properties::BORDER_TOP_STYLE, {}).unwrap(), _value.top),
            makeRc<BorderRightStyleProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_STYLE, {}).unwrap(), _value.end),
            makeRc<BorderBottomStyleProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_STYLE, {}).unwrap(), _value.bottom),
            makeRc<BorderLeftStyleProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_STYLE, {}).unwrap(), _value.start),
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderTopWidthProperty>(self(), c.borders->top.width);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderTopWidthProperty>(self(), try$(parseValue<LineWidth>(c))));
        }
    };

    LineWidth _value;

    BorderTopWidthProperty(Rc<Property::Registration> registration, LineWidth value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().top.width = _value;
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRightWidthProperty>(self(), c.borders->end.width);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRightWidthProperty>(self(), try$(parseValue<LineWidth>(c))));
        }
    };

    LineWidth _value;

    BorderRightWidthProperty(Rc<Property::Registration> registration, LineWidth value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().end.width = _value;
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderBottomWidthProperty>(self(), c.borders->bottom.width);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderBottomWidthProperty>(self(), try$(parseValue<CalcValue<Length>>(c))));
        }
    };

    LineWidth _value;

    BorderBottomWidthProperty(Rc<Property::Registration> registration, LineWidth value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().bottom.width = _value;
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderLeftWidthProperty>(self(), c.borders->start.width);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderLeftWidthProperty>(self(), try$(parseValue<LineWidth>(c))));
        }
    };

    LineWidth _value;

    BorderLeftWidthProperty(Rc<Property::Registration> registration, LineWidth value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().start.width = _value;
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
            return makeRc<BorderRadiusTopRightProperty>(self(), makeArray<CalcValue<PercentOr<Length>>, 2>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRadiusTopRightProperty>(self(), Array{
                                                                    c.borders->radii.c,
                                                                    c.borders->radii.d,
                                                                });
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto first = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            Array value{first, first};
            if (not c.ended())
                value[1] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            return Ok(makeRc<BorderRadiusTopRightProperty>(self(), value));
        }
    };

    Array<CalcValue<PercentOr<Length>>, 2> _value;

    BorderRadiusTopRightProperty(Rc<Property::Registration> registration, Array<CalcValue<PercentOr<Length>>, 2> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().radii.c = _value[0];
        c.borders.cow().radii.d = _value[1];
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
            return makeRc<BorderRadiusTopLeftProperty>(self(), makeArray<CalcValue<PercentOr<Length>>, 2>(Length{}));
        }

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRadiusTopLeftProperty>(
                self(),
                Array{
                    c.borders->radii.a,
                    c.borders->radii.b,
                }
            );
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            auto first = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            Array value{first, first};
            if (not c.ended())
                value[1] = try$(parseValue<CalcValue<PercentOr<Length>>>(c));
            return Ok(makeRc<BorderRadiusTopLeftProperty>(self(), value));
        }
    };

    Array<CalcValue<PercentOr<Length>>, 2> _value;

    BorderRadiusTopLeftProperty(Rc<Property::Registration> registration, Array<CalcValue<PercentOr<Length>>, 2> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.borders.cow().radii.a = _value[1];
        c.borders.cow().radii.b = _value[0];
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

        Rc<Property> load(SpecifiedValues const& c) const override {
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

    void apply(SpecifiedValues& c) const override {
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

        Rc<Property> load(SpecifiedValues const& c) const override {
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

    void apply(SpecifiedValues& c) const override {
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderRadiusProperty>(self(), c.borders->radii);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderRadiusProperty>(self(), try$(parseValue<Math::Radii<CalcValue<PercentOr<Length>>>>(c))));
        }
    };

    Math::Radii<CalcValue<PercentOr<Length>>> _value;

    BorderRadiusProperty(Rc<Property::Registration> registration, Math::Radii<CalcValue<PercentOr<Length>>> value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
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

        Rc<Property> load(SpecifiedValues const& c) const override {
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

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
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

        Rc<Property> load(SpecifiedValues const& c) const override {
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

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
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

        Rc<Property> load(SpecifiedValues const& c) const override {
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

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
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

        Rc<Property> load(SpecifiedValues const& c) const override {
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

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderProperty>(self(), c.borders->top);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderProperty>(self(), try$(parseValue<Border>(c))));
        }
    };

    Border _value;

    BorderProperty(Rc<Property::Registration> registration, Border value)
        : Property(registration), _value(value) {}

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
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

        Rc<Property> load(SpecifiedValues const& c) const override {
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

    Vec<Rc<Property>> expandShorthand(PropertyRegistry& registry, SpecifiedValues const&, SpecifiedValues&) const override {
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderCollapseProperty>(self(), c.table->collapse);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderCollapseProperty>(self(), try$(parseValue<BorderCollapse>(c))));
        }
    };

    BorderCollapse _value;

    BorderCollapseProperty(Rc<Property::Registration> registration, BorderCollapse value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
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

        Rc<Property> load(SpecifiedValues const& c) const override {
            return makeRc<BorderSpacingProperty>(self(), c.table->spacing);
        }

        Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
            return Ok(makeRc<BorderSpacingProperty>(self(), try$(parseValue<BorderSpacing>(c))));
        }
    };

    BorderSpacing _value;

    BorderSpacingProperty(Rc<Property::Registration> registration, BorderSpacing value)
        : Property(registration), _value(value) {}

    void apply(SpecifiedValues& c) const override {
        c.table.cow().spacing = _value;
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
