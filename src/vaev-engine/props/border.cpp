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
    static constexpr auto ID = LonghandId::BORDER_TOP_COLOR;

    static Rc<Property> initial() {
        return makeRc<BorderTopColorProperty>(BLACK);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderTopColorProperty>(c.borders->top.color);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BorderTopColorProperty>(try$(parseValue<Color>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_TOP_COLOR) {}
    };

    Color _value;

    BorderTopColorProperty(Color value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().top.color = _value;
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
    static constexpr auto ID = LonghandId::BORDER_RIGHT_COLOR;

    static Rc<Property> initial() {
        return makeRc<BorderRightColorProperty>(BLACK);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderRightColorProperty>(c.borders->end.color);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BorderRightColorProperty>(try$(parseValue<Color>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_RIGHT_COLOR) {}
    };

    Color _value;

    BorderRightColorProperty(Color value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().end.color = _value;
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
    static constexpr auto ID = LonghandId::BORDER_BOTTOM_COLOR;

    static Rc<Property> initial() {
        return makeRc<BorderBottomColorProperty>(BLACK);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderBottomColorProperty>(c.borders->bottom.color);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BorderBottomColorProperty>(try$(parseValue<Color>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_BOTTOM_COLOR) {}
    };

    Color _value;

    BorderBottomColorProperty(Color value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().bottom.color = _value;
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
    static constexpr auto ID = LonghandId::BORDER_LEFT_COLOR;

    static Rc<Property> initial() {
        return makeRc<BorderLeftColorProperty>(BLACK);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderLeftColorProperty>(c.borders->start.color);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BorderLeftColorProperty>(try$(parseValue<Color>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_LEFT_COLOR) {}
    };

    Color _value;

    BorderLeftColorProperty(Color value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().start.color = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// export struct BorderColorProperty : Property {
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::BORDER_COLOR, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<BorderColorProperty>(self(), Math::Insets<Color>{BLACK});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<BorderColorProperty>(
//                 self(),
//                 Math::Insets{
//                     c.borders->start.color,
//                     c.borders->end.color,
//                     c.borders->top.color,
//                     c.borders->bottom.color,
//                 }
//                     .cast<Color>()
//             );
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             return Ok(makeRc<BorderColorProperty>(self(), try$(parseValue<Math::Insets<Color>>(c))));
//         }
//     };
//
//     Math::Insets<Color> _value;
//
//     BorderColorProperty(Rc<Property::Registration> registration, Math::Insets<Color> value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<BorderTopColorProperty>(registry.resolveRegistration(Properties::BORDER_TOP_COLOR, {}).expect(), _value.top),
//             makeRc<BorderRightColorProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_COLOR, {}).expect(), _value.end),
//             makeRc<BorderBottomColorProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_COLOR, {}).expect(), _value.bottom),
//             makeRc<BorderLeftColorProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_COLOR, {}).expect(), _value.start),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderLeftStyleProperty : Property {
    static constexpr auto ID = LonghandId::BORDER_LEFT_STYLE;

    static Rc<Property> initial() {
        return makeRc<BorderLeftStyleProperty>(Gfx::BorderStyle::NONE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderLeftStyleProperty>(c.borders->start.style);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BorderLeftStyleProperty>(try$(parseValue<Gfx::BorderStyle>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_LEFT_STYLE) {}
    };

    Gfx::BorderStyle _value;

    BorderLeftStyleProperty(Gfx::BorderStyle value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().start.style = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderTopStyleProperty : Property {
    static constexpr auto ID = LonghandId::BORDER_TOP_STYLE;

    static Rc<Property> initial() {
        return makeRc<BorderTopStyleProperty>(Gfx::BorderStyle::NONE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderTopStyleProperty>(c.borders->top.style);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BorderTopStyleProperty>(try$(parseValue<Gfx::BorderStyle>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_TOP_STYLE) {}
    };

    Gfx::BorderStyle _value;

    BorderTopStyleProperty(Gfx::BorderStyle value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().top.style = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderRightStyleProperty : Property {
    static constexpr auto ID = LonghandId::BORDER_RIGHT_STYLE;

    static Rc<Property> initial() {
        return makeRc<BorderRightStyleProperty>(Gfx::BorderStyle::NONE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderRightStyleProperty>(c.borders->end.style);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BorderRightStyleProperty>(try$(parseValue<Gfx::BorderStyle>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_RIGHT_STYLE) {}
    };

    Gfx::BorderStyle _value;

    BorderRightStyleProperty(Gfx::BorderStyle value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().end.style = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties
export struct BorderBottomStyleProperty : Property {
    static constexpr auto ID = LonghandId::BORDER_BOTTOM_STYLE;

    static Rc<Property> initial() {
        return makeRc<BorderBottomStyleProperty>(Gfx::BorderStyle::NONE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderBottomStyleProperty>(c.borders->bottom.style);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BorderBottomStyleProperty>(try$(parseValue<Gfx::BorderStyle>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_BOTTOM_STYLE) {}
    };

    Gfx::BorderStyle _value;

    BorderBottomStyleProperty(Gfx::BorderStyle value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().bottom.style = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/box.html#border-style-properties

// export struct BorderStyleProperty : Property {
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::BORDER_STYLE, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<BorderStyleProperty>(self(), Math::Insets{Gfx::BorderStyle::NONE});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<BorderStyleProperty>(
//                 self(),
//                 Math::Insets{
//                     c.borders->start.style,
//                     c.borders->end.style,
//                     c.borders->top.style,
//                     c.borders->bottom.style,
//                 }
//             );
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             return Ok(makeRc<BorderStyleProperty>(self(), try$(parseValue<Math::Insets<Gfx::BorderStyle>>(c))));
//         }
//     };
//
//     Math::Insets<Gfx::BorderStyle> _value;
//
//     BorderStyleProperty(Rc<Property::Registration> registration, Math::Insets<Gfx::BorderStyle> value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<BorderTopStyleProperty>(registry.resolveRegistration(Properties::BORDER_TOP_STYLE, {}).expect(), _value.top),
//             makeRc<BorderRightStyleProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_STYLE, {}).expect(), _value.end),
//             makeRc<BorderBottomStyleProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_STYLE, {}).expect(), _value.bottom),
//             makeRc<BorderLeftStyleProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_STYLE, {}).expect(), _value.start),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderTopWidthProperty : Property {
    static constexpr auto ID = LonghandId::BORDER_TOP_WIDTH;

    static Rc<Property> initial() {
        return makeRc<BorderTopWidthProperty>(Keywords::MEDIUM);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderTopWidthProperty>(c.borders->top.width);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BorderTopWidthProperty>(try$(parseValue<LineWidth>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_TOP_WIDTH) {}
    };

    LineWidth _value;

    BorderTopWidthProperty(LineWidth value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().top.width = _value;
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
    static constexpr auto ID = LonghandId::BORDER_RIGHT_WIDTH;

    static Rc<Property> initial() {
        return makeRc<BorderRightWidthProperty>(Keywords::MEDIUM);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderRightWidthProperty>(c.borders->end.width);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BorderRightWidthProperty>(try$(parseValue<LineWidth>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_RIGHT_WIDTH) {}
    };

    LineWidth _value;

    BorderRightWidthProperty(LineWidth value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().end.width = _value;
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
    static constexpr auto ID = LonghandId::BORDER_BOTTOM_WIDTH;

    static Rc<Property> initial() {
        return makeRc<BorderBottomWidthProperty>(Keywords::MEDIUM);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderBottomWidthProperty>(c.borders->bottom.width);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BorderBottomWidthProperty>(try$(parseValue<Calc<Length>>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_BOTTOM_WIDTH) {}
    };

    LineWidth _value;

    BorderBottomWidthProperty(LineWidth value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().bottom.width = _value;
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
    static constexpr auto ID = LonghandId::BORDER_LEFT_WIDTH;

    static Rc<Property> initial() {
        return makeRc<BorderLeftWidthProperty>(Keywords::MEDIUM);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderLeftWidthProperty>(c.borders->start.width);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BorderLeftWidthProperty>(try$(parseValue<LineWidth>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_LEFT_WIDTH) {}
    };

    LineWidth _value;

    BorderLeftWidthProperty(LineWidth value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().start.width = _value;
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
    static constexpr auto ID = LonghandId::BORDER_TOP_RIGHT_RADIUS;

    static Rc<Property> initial() {
        return makeRc<BorderTopRightRadiusProperty>(makeArray<Calc<PercentOr<Length>>, 2>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderTopRightRadiusProperty>(Array{
                                                                c.borders->radii.c,
                                                                c.borders->radii.d,
                                                            });
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        auto first = try$(parseValue<Calc<PercentOr<Length>>>(c));
        Array value{first, first};
        if (not c.ended())
            value[1] = try$(parseValue<Calc<PercentOr<Length>>>(c));
        return Ok(makeRc<BorderTopRightRadiusProperty>(value));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_TOP_RIGHT_RADIUS) {}
    };

    Array<Calc<PercentOr<Length>>, 2> _value;

    BorderTopRightRadiusProperty(Array<Calc<PercentOr<Length>>, 2> value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().radii.c = _value[0];
        c.borders.cow().radii.d = _value[1];
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
    static constexpr auto ID = LonghandId::BORDER_TOP_LEFT_RADIUS;

    static Rc<Property> initial() {
        return makeRc<BorderTopLeftRadiusProperty>(makeArray<Calc<PercentOr<Length>>, 2>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderTopLeftRadiusProperty>(Array{
                c.borders->radii.a,
                c.borders->radii.b,
            }
        );
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        auto first = try$(parseValue<Calc<PercentOr<Length>>>(c));
        Array value{first, first};
        if (not c.ended())
            value[1] = try$(parseValue<Calc<PercentOr<Length>>>(c));
        return Ok(makeRc<BorderTopLeftRadiusProperty>(value));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_TOP_LEFT_RADIUS) {}
    };

    Array<Calc<PercentOr<Length>>, 2> _value;

    BorderTopLeftRadiusProperty(Array<Calc<PercentOr<Length>>, 2> value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().radii.a = _value[1];
        c.borders.cow().radii.b = _value[0];
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
    static constexpr auto ID = LonghandId::BORDER_BOTTOM_RIGHT_RADIUS;

    static Rc<Property> initial() {
        return makeRc<BorderBottomRightRadiusProperty>(makeArray<Calc<PercentOr<Length>>, 2>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderBottomRightRadiusProperty>(Array{
                                                                   c.borders->radii.e,
                                                                   c.borders->radii.f,
                                                               });
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        auto first = try$(parseValue<Calc<PercentOr<Length>>>(c));
        Array value{first, first};
        if (not c.ended())
            value[1] = try$(parseValue<Calc<PercentOr<Length>>>(c));
        return Ok(makeRc<BorderBottomRightRadiusProperty>(value));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_BOTTOM_RIGHT_RADIUS) {}
    };

    Array<Calc<PercentOr<Length>>, 2> _value;

    BorderBottomRightRadiusProperty(Array<Calc<PercentOr<Length>>, 2> value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().radii.e = _value[1];
        c.borders.cow().radii.f = _value[0];
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
    static constexpr auto ID = LonghandId::BORDER_BOTTOM_LEFT_RADIUS;

    static Rc<Property> initial() {
        return makeRc<BorderBottomLeftRadiusProperty>(makeArray<Calc<PercentOr<Length>>, 2>(Length{}));
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderBottomLeftRadiusProperty>(Array{
                c.borders->radii.g,
                c.borders->radii.h,
            }
        );
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        auto first = try$(parseValue<Calc<PercentOr<Length>>>(c));
        Array value{first, first};
        if (not c.ended())
            value[1] = try$(parseValue<Calc<PercentOr<Length>>>(c));
        return Ok(makeRc<BorderBottomLeftRadiusProperty>(value));
    }

    struct Registration : LonghandRegistration {
        Registration() : LonghandRegistration(Properties::BORDER_BOTTOM_LEFT_RADIUS) {}
    };

    Array<Calc<PercentOr<Length>>, 2> _value;

    BorderBottomLeftRadiusProperty(Array<Calc<PercentOr<Length>>, 2> value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.borders.cow().radii.g = _value[0];
        c.borders.cow().radii.h = _value[1];
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// // https://drafts.csswg.org/css-backgrounds/#the-border-radius
// export struct BorderRadiusProperty : Property {
//     struct Registration : Property::Registration {
//         Registration() : Property::Registration(Properties::BORDER_RADIUS, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<BorderRadiusProperty>(self(), Math::Radii{Calc<PercentOr<Length>>(Length{})});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<BorderRadiusProperty>(self(), c.borders->radii);
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             auto radii = try$(parseValue<Math::Radii<Calc<PercentOr<Length>>>>(c));
//             return Ok(makeRc<BorderRadiusProperty>(self(), radii));
//         }
//     };
//
//     Math::Radii<Calc<PercentOr<Length>>> _value;
//
//     BorderRadiusProperty(Rc<Property::Registration> registration, Math::Radii<Calc<PercentOr<Length>>> value)
//         : Property(registration), _value(value) {}
//
//     void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
//         c.borders.cow().radii = _value;
//     }
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<BorderTopLeftRadiusProperty>(registry.resolveRegistration(Properties::BORDER_TOP_LEFT_RADIUS, {}).expect(), Array{_value.a, _value.b}),
//             makeRc<BorderTopRightRadiusProperty>(registry.resolveRegistration(Properties::BORDER_TOP_RIGHT_RADIUS, {}).expect(), Array{_value.c, _value.d}),
//             makeRc<BorderBottomRightRadiusProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_RIGHT_RADIUS, {}).expect(), Array{_value.e, _value.f}),
//             makeRc<BorderBottomLeftRadiusProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_LEFT_RADIUS, {}).expect(), Array{_value.g, _value.h}),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

// // https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
// export struct BorderTopProperty : Property {
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::BORDER_TOP, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<BorderTopProperty>(self(), SpecifiedBorder{});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<BorderTopProperty>(self(), c.borders->top);
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             SpecifiedBorder value;
//             while (not c.ended()) {
//                 auto width = parseValue<Calc<Length>>(c);
//                 if (width) {
//                     value.width = width.expect();
//                     continue;
//                 }
//
//                 auto color = parseValue<Color>(c);
//                 if (color) {
//                     value.color = color.expect();
//                     continue;
//                 }
//
//                 auto style = parseValue<Gfx::BorderStyle>(c);
//                 if (style) {
//                     value.style = style.expect();
//                     continue;
//                 }
//
//                 break;
//             }
//             return Ok(makeRc<BorderTopProperty>(self(), value));
//         }
//     };
//
//     SpecifiedBorder _value;
//
//     BorderTopProperty(Rc<Property::Registration> registration, SpecifiedBorder value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<BorderTopWidthProperty>(registry.resolveRegistration(Properties::BORDER_TOP_WIDTH, {}).expect(), _value.width),
//             makeRc<BorderTopStyleProperty>(registry.resolveRegistration(Properties::BORDER_TOP_STYLE, {}).expect(), _value.style),
//             makeRc<BorderTopColorProperty>(registry.resolveRegistration(Properties::BORDER_TOP_COLOR, {}).expect(), _value.color),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

// // https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
// export struct BorderRightProperty : Property {
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::BORDER_RIGHT, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<BorderRightProperty>(self(), SpecifiedBorder{});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<BorderRightProperty>(self(), c.borders->end);
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             return Ok(makeRc<BorderRightProperty>(self(), try$(parseValue<SpecifiedBorder>(c))));
//         }
//     };
//
//     SpecifiedBorder _value;
//
//     BorderRightProperty(Rc<Property::Registration> registration, SpecifiedBorder value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<BorderRightWidthProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_WIDTH, {}).expect(), _value.width),
//             makeRc<BorderRightStyleProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_STYLE, {}).expect(), _value.style),
//             makeRc<BorderRightColorProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_COLOR, {}).expect(), _value.color),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

// // https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
// export struct BorderBottomProperty : Property {
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::BORDER_BOTTOM, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<BorderBottomProperty>(self(), SpecifiedBorder{});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<BorderBottomProperty>(self(), c.borders->bottom);
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             return Ok(makeRc<BorderBottomProperty>(self(), try$(parseValue<SpecifiedBorder>(c))));
//         }
//     };
//
//     SpecifiedBorder _value;
//
//     BorderBottomProperty(Rc<Property::Registration> registration, SpecifiedBorder value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<BorderBottomWidthProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_WIDTH, {}).expect(), _value.width),
//             makeRc<BorderBottomStyleProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_STYLE, {}).expect(), _value.style),
//             makeRc<BorderBottomColorProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_COLOR, {}).expect(), _value.color),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

// // https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
// export struct BorderLeftProperty : Property {
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::BORDER_LEFT, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<BorderLeftProperty>(self(), SpecifiedBorder{});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<BorderLeftProperty>(self(), c.borders->start);
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             return Ok(makeRc<BorderLeftProperty>(self(), try$(parseValue<SpecifiedBorder>(c))));
//         }
//     };
//
//     SpecifiedBorder _value;
//
//     BorderLeftProperty(Rc<Property::Registration> registration, SpecifiedBorder value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<BorderLeftWidthProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_WIDTH, {}).expect(), _value.width),
//             makeRc<BorderLeftStyleProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_STYLE, {}).expect(), _value.style),
//             makeRc<BorderLeftColorProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_COLOR, {}).expect(), _value.color),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

// // https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
// export struct BorderProperty : Property {
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::BORDER, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<BorderProperty>(self(), SpecifiedBorder{});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<BorderProperty>(self(), c.borders->top);
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             return Ok(makeRc<BorderProperty>(self(), try$(parseValue<SpecifiedBorder>(c))));
//         }
//     };
//
//     SpecifiedBorder _value;
//
//     BorderProperty(Rc<Property::Registration> registration, SpecifiedBorder value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<BorderTopWidthProperty>(registry.resolveRegistration(Properties::BORDER_TOP_WIDTH, {}).expect(), _value.width),
//             makeRc<BorderTopStyleProperty>(registry.resolveRegistration(Properties::BORDER_TOP_STYLE, {}).expect(), _value.style),
//             makeRc<BorderTopColorProperty>(registry.resolveRegistration(Properties::BORDER_TOP_COLOR, {}).expect(), _value.color),
//
//             makeRc<BorderRightWidthProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_WIDTH, {}).expect(), _value.width),
//             makeRc<BorderRightStyleProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_STYLE, {}).expect(), _value.style),
//             makeRc<BorderRightColorProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_COLOR, {}).expect(), _value.color),
//
//             makeRc<BorderBottomWidthProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_WIDTH, {}).expect(), _value.width),
//             makeRc<BorderBottomStyleProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_STYLE, {}).expect(), _value.style),
//             makeRc<BorderBottomColorProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_COLOR, {}).expect(), _value.color),
//
//             makeRc<BorderLeftWidthProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_WIDTH, {}).expect(), _value.width),
//             makeRc<BorderLeftStyleProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_STYLE, {}).expect(), _value.style),
//             makeRc<BorderLeftColorProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_COLOR, {}).expect(), _value.color),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

// // https://www.w3.org/TR/css-backgrounds-3/#border-width
// export struct BorderWidthProperty : Property {
//     struct Registration : Property::Registration {
//         Registration()
//             : Property::Registration(Properties::BORDER_WIDTH, SHORTHAND) {}
//
//         Rc<Property> initial() const override {
//             return makeRc<BorderWidthProperty>(self(), Math::Insets{LineWidth{Keywords::MEDIUM}});
//         }
//
//         Rc<Property> load(ComputedValues const& c) const override {
//             return makeRc<BorderWidthProperty>(
//                 self(),
//                 Math::Insets{
//                     c.borders->start.width,
//                     c.borders->end.width,
//                     c.borders->top.width,
//                     c.borders->bottom.width,
//                 }
//             );
//         }
//
//         Res<Rc<Property>> parse(Cursor<Css::Sst>& c) const override {
//             return Ok(makeRc<BorderWidthProperty>(self(), try$(parseValue<Math::Insets<LineWidth>>(c))));
//         }
//     };
//
//     Math::Insets<LineWidth> _value;
//
//     BorderWidthProperty(Rc<Property::Registration> registration, Math::Insets<LineWidth> value)
//         : Property(registration), _value(value) {}
//
//     Longhands expand(RegisteredPropertySet& registry, ComputedValues const&, ComputedValues&) const override {
//         return {
//             makeRc<BorderTopWidthProperty>(registry.resolveRegistration(Properties::BORDER_TOP_WIDTH, {}).expect(), _value.top),
//             makeRc<BorderRightWidthProperty>(registry.resolveRegistration(Properties::BORDER_RIGHT_WIDTH, {}).expect(), _value.end),
//             makeRc<BorderBottomWidthProperty>(registry.resolveRegistration(Properties::BORDER_BOTTOM_WIDTH, {}).expect(), _value.bottom),
//             makeRc<BorderLeftWidthProperty>(registry.resolveRegistration(Properties::BORDER_LEFT_WIDTH, {}).expect(), _value.start),
//         };
//     }
//
//     void repr(Io::Emit& e) const override {
//         e("{}", _value);
//     }
// };

// MARK: Borders - Table -------------------------------------------------------

// https://www.w3.org/TR/CSS22/tables.html#propdef-border-collapse
export struct BorderCollapseProperty : Property {
    static constexpr auto ID = LonghandId::BORDER_COLLAPSE;
    static constexpr Flags<Options> FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<BorderCollapseProperty>(BorderCollapse::SEPARATE);
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderCollapseProperty>(c.table->collapse);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BorderCollapseProperty>(try$(parseValue<BorderCollapse>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::BORDER_COLLAPSE, FLAGS) {}
    };

    BorderCollapse _value;

    BorderCollapseProperty(BorderCollapse value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.table.cow().collapse = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

// https://www.w3.org/TR/CSS22/tables.html#propdef-border-spacing
export struct BorderSpacingProperty : Property {
    static constexpr auto ID = LonghandId::BORDER_SPACING;
    static constexpr Flags<Options> FLAGS = {INHERITED, BULK_INHERITED};

    static Rc<Property> initial() {
        return makeRc<BorderSpacingProperty>(BorderSpacing{0_au, 0_au});
    }

    static Rc<Property> load(ComputedValues const& c) {
        return makeRc<BorderSpacingProperty>(c.table->spacing);
    }

    static Res<Rc<Property>> parse(Cursor<Css::Sst>& c, ParsingMode) {
        return Ok(makeRc<BorderSpacingProperty>(try$(parseValue<BorderSpacing>(c))));
    }

    struct Registration : LonghandRegistration {
        Registration()
            : LonghandRegistration(Properties::BORDER_SPACING, FLAGS) {}
    };

    BorderSpacing _value;

    BorderSpacingProperty(BorderSpacing value)
        : Property(ID), _value(value) {}

    void apply([[maybe_unused]] ComputedValues const& parent, ComputedValues& c, [[maybe_unused]] ComputationContext const& cx) const override {
        c.table.cow().spacing = _value;
    }

    Res<> serialize(Io::TextWriter& w) const override {
        return serializeValue(w, _value);
    }

    void repr(Io::Emit& e) const override {
        e("{}", _value);
    }
};

} // namespace Vaev::Style
