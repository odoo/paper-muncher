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

// https://drafts.csswg.org/css-backgrounds-3/#border-color
export struct BorderColorShorthand {
    static constexpr auto ID = ShorthandId::BORDER_COLOR;

    static constexpr auto LONGHANDS = Array{
        LonghandId::BORDER_TOP_COLOR,
        LonghandId::BORDER_RIGHT_COLOR,
        LonghandId::BORDER_BOTTOM_COLOR,
        LonghandId::BORDER_LEFT_COLOR,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        auto value = try$(parseValue<Math::Insets<Color>>(c));
        stylesheet.pushBack(makeRc<BorderTopColorProperty>(value.top));
        stylesheet.pushBack(makeRc<BorderRightColorProperty>(value.end));
        stylesheet.pushBack(makeRc<BorderBottomColorProperty>(value.bottom));
        stylesheet.pushBack(makeRc<BorderLeftColorProperty>(value.start));
        return Ok();
    }
};

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
export struct BorderStyleShorthand {
    static constexpr auto ID = ShorthandId::BORDER_STYLE;

    static constexpr auto LONGHANDS = Array{
        LonghandId::BORDER_TOP_STYLE,
        LonghandId::BORDER_RIGHT_STYLE,
        LonghandId::BORDER_BOTTOM_STYLE,
        LonghandId::BORDER_LEFT_STYLE,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        auto value = try$(parseValue<Math::Insets<Gfx::BorderStyle>>(c));
        stylesheet.pushBack(makeRc<BorderTopStyleProperty>(value.top));
        stylesheet.pushBack(makeRc<BorderRightStyleProperty>(value.end));
        stylesheet.pushBack(makeRc<BorderBottomStyleProperty>(value.bottom));
        stylesheet.pushBack(makeRc<BorderLeftStyleProperty>(value.start));
        return Ok();
    }
};

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

// https://drafts.csswg.org/css-backgrounds/#the-border-radius
export struct BorderRadiusShorthand {
    static constexpr auto ID = ShorthandId::BORDER_RADIUS;

    static constexpr auto LONGHANDS = Array{
        LonghandId::BORDER_TOP_LEFT_RADIUS,
        LonghandId::BORDER_TOP_RIGHT_RADIUS,
        LonghandId::BORDER_BOTTOM_RIGHT_RADIUS,
        LonghandId::BORDER_BOTTOM_LEFT_RADIUS,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        auto radii = try$(parseValue<Math::Radii<Calc<PercentOr<Length>>>>(c));
        stylesheet.pushBack(makeRc<BorderTopLeftRadiusProperty>(Array{radii.a, radii.b}));
        stylesheet.pushBack(makeRc<BorderTopRightRadiusProperty>(Array{radii.c, radii.d}));
        stylesheet.pushBack(makeRc<BorderBottomRightRadiusProperty>(Array{radii.e, radii.f}));
        stylesheet.pushBack(makeRc<BorderBottomLeftRadiusProperty>(Array{radii.g, radii.h}));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderTopShorthand {
    static constexpr auto ID = ShorthandId::BORDER_TOP;

    static constexpr auto LONGHANDS = Array{
        LonghandId::BORDER_TOP_WIDTH,
        LonghandId::BORDER_TOP_STYLE,
        LonghandId::BORDER_TOP_COLOR,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        // NOTE: Omitted components are reset to their initial value,
        //       which is the default of SpecifiedBorder.
        auto border = try$(parseValue<SpecifiedBorder>(c));
        stylesheet.pushBack(makeRc<BorderTopWidthProperty>(border.width));
        stylesheet.pushBack(makeRc<BorderTopStyleProperty>(border.style));
        stylesheet.pushBack(makeRc<BorderTopColorProperty>(border.color));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderRightShorthand {
    static constexpr auto ID = ShorthandId::BORDER_RIGHT;

    static constexpr auto LONGHANDS = Array{
        LonghandId::BORDER_RIGHT_WIDTH,
        LonghandId::BORDER_RIGHT_STYLE,
        LonghandId::BORDER_RIGHT_COLOR,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        // NOTE: Omitted components are reset to their initial value,
        //       which is the default of SpecifiedBorder.
        auto border = try$(parseValue<SpecifiedBorder>(c));
        stylesheet.pushBack(makeRc<BorderRightWidthProperty>(border.width));
        stylesheet.pushBack(makeRc<BorderRightStyleProperty>(border.style));
        stylesheet.pushBack(makeRc<BorderRightColorProperty>(border.color));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderBottomShorthand {
    static constexpr auto ID = ShorthandId::BORDER_BOTTOM;

    static constexpr auto LONGHANDS = Array{
        LonghandId::BORDER_BOTTOM_WIDTH,
        LonghandId::BORDER_BOTTOM_STYLE,
        LonghandId::BORDER_BOTTOM_COLOR,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        // NOTE: Omitted components are reset to their initial value,
        //       which is the default of SpecifiedBorder.
        auto border = try$(parseValue<SpecifiedBorder>(c));
        stylesheet.pushBack(makeRc<BorderBottomWidthProperty>(border.width));
        stylesheet.pushBack(makeRc<BorderBottomStyleProperty>(border.style));
        stylesheet.pushBack(makeRc<BorderBottomColorProperty>(border.color));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderLeftShorthand {
    static constexpr auto ID = ShorthandId::BORDER_LEFT;

    static constexpr auto LONGHANDS = Array{
        LonghandId::BORDER_LEFT_WIDTH,
        LonghandId::BORDER_LEFT_STYLE,
        LonghandId::BORDER_LEFT_COLOR,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        // NOTE: Omitted components are reset to their initial value,
        //       which is the default of SpecifiedBorder.
        auto border = try$(parseValue<SpecifiedBorder>(c));
        stylesheet.pushBack(makeRc<BorderLeftWidthProperty>(border.width));
        stylesheet.pushBack(makeRc<BorderLeftStyleProperty>(border.style));
        stylesheet.pushBack(makeRc<BorderLeftColorProperty>(border.color));
        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-shorthands
export struct BorderShorthand {
    static constexpr auto ID = ShorthandId::BORDER;

    // FIXME: The border shorthand also resets border-image to its initial value.
    static constexpr auto LONGHANDS = Array{
        LonghandId::BORDER_TOP_WIDTH,
        LonghandId::BORDER_TOP_STYLE,
        LonghandId::BORDER_TOP_COLOR,
        LonghandId::BORDER_RIGHT_WIDTH,
        LonghandId::BORDER_RIGHT_STYLE,
        LonghandId::BORDER_RIGHT_COLOR,
        LonghandId::BORDER_BOTTOM_WIDTH,
        LonghandId::BORDER_BOTTOM_STYLE,
        LonghandId::BORDER_BOTTOM_COLOR,
        LonghandId::BORDER_LEFT_WIDTH,
        LonghandId::BORDER_LEFT_STYLE,
        LonghandId::BORDER_LEFT_COLOR,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        auto border = try$(parseValue<SpecifiedBorder>(c));

        stylesheet.pushBack(makeRc<BorderTopWidthProperty>(border.width));
        stylesheet.pushBack(makeRc<BorderTopStyleProperty>(border.style));
        stylesheet.pushBack(makeRc<BorderTopColorProperty>(border.color));

        stylesheet.pushBack(makeRc<BorderRightWidthProperty>(border.width));
        stylesheet.pushBack(makeRc<BorderRightStyleProperty>(border.style));
        stylesheet.pushBack(makeRc<BorderRightColorProperty>(border.color));

        stylesheet.pushBack(makeRc<BorderBottomWidthProperty>(border.width));
        stylesheet.pushBack(makeRc<BorderBottomStyleProperty>(border.style));
        stylesheet.pushBack(makeRc<BorderBottomColorProperty>(border.color));

        stylesheet.pushBack(makeRc<BorderLeftWidthProperty>(border.width));
        stylesheet.pushBack(makeRc<BorderLeftStyleProperty>(border.style));
        stylesheet.pushBack(makeRc<BorderLeftColorProperty>(border.color));

        return Ok();
    }
};

// https://www.w3.org/TR/css-backgrounds-3/#border-width
export struct BorderWidthShorthand {
    static constexpr auto ID = ShorthandId::BORDER_WIDTH;

    static constexpr auto LONGHANDS = Array{
        LonghandId::BORDER_TOP_WIDTH,
        LonghandId::BORDER_RIGHT_WIDTH,
        LonghandId::BORDER_BOTTOM_WIDTH,
        LonghandId::BORDER_LEFT_WIDTH,
    };

    static Res<> parse(Cursor<Css::Sst>& c, Vec<Rc<Property>>& stylesheet, ParsingMode) {
        auto value = try$(parseValue<Math::Insets<LineWidth>>(c));
        stylesheet.pushBack(makeRc<BorderTopWidthProperty>(value.top));
        stylesheet.pushBack(makeRc<BorderRightWidthProperty>(value.end));
        stylesheet.pushBack(makeRc<BorderBottomWidthProperty>(value.bottom));
        stylesheet.pushBack(makeRc<BorderLeftWidthProperty>(value.start));
        return Ok();
    }
};

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
