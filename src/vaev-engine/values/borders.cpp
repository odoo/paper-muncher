module;

#include <karm/macros>

export module Vaev.Engine:values.borders;

import Karm.Gfx;
import Karm.Math;

import :css;
import :values.base;
import :values.calc;
import :values.color;
import :values.length;
import :values.lineWidth;
import :values.percent;

namespace Vaev {

// MARK: Border Style ----------------------------------------------------------
// https://www.w3.org/TR/CSS22/box.html#border-style-properties

export template <>
struct ValueTraits<Gfx::BorderStyle> : DefaultValueTraits<Gfx::BorderStyle> {
    static Res<Gfx::BorderStyle> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of property");

        if (c.skip(Css::Token::ident("none"))) {
            return Ok(Gfx::BorderStyle::NONE);
        } else if (c.skip(Css::Token::ident("solid"))) {
            return Ok(Gfx::BorderStyle::SOLID);
        } else if (c.skip(Css::Token::ident("dashed"))) {
            return Ok(Gfx::BorderStyle::DASHED);
        } else if (c.skip(Css::Token::ident("dotted"))) {
            return Ok(Gfx::BorderStyle::DOTTED);
        } else if (c.skip(Css::Token::ident("hidden"))) {
            return Ok(Gfx::BorderStyle::HIDDEN);
        } else if (c.skip(Css::Token::ident("double"))) {
            return Ok(Gfx::BorderStyle::DOUBLE);
        } else if (c.skip(Css::Token::ident("groove"))) {
            return Ok(Gfx::BorderStyle::GROOVE);
        } else if (c.skip(Css::Token::ident("ridge"))) {
            return Ok(Gfx::BorderStyle::RIDGE);
        } else if (c.skip(Css::Token::ident("outset"))) {
            return Ok(Gfx::BorderStyle::OUTSET);
        } else {
            return Error::invalidData("unknown border-style");
        }
    }
};

// MARK: Border ----------------------------------------------------------------

export template <typename T>
struct Edges<T> {
    T top;
    T right;
    T bottom;
    T left;

    Edges(T a) : top(a), right(a), bottom(a), left(a) {}

    Edges(T a, T b) : top(a), right(b), bottom(a), left(b) {}

    Edges(T a, T b, T c) : top(a), right(b), bottom(c), left(b) {}

    Edges(T a, T b, T c, T d) : top(a), right(b), bottom(c), left(d) {}
};

export template <typename T>
struct ValueTraits<Edges<T>> {
    Res<Edges<T>> parse(Cursor<Css::Sst> c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto v0 = try$(parseValue<T>(c));

        if (c.ended())
            return Ok(Edges<T>{v0});

        auto v1 = try$(parseValue<T>(c));

        if (c.ended())
            return Ok(Edges<T>{v0, v1});

        auto v2 = try$(parseValue<T>(c));

        if (c.ended())
            return Ok(Edges<T>{v0, v1, v2});

        auto v3 = try$(parseValue<T>(c));

        if (c.ended())
            return Ok(Edges<T>{v0, v1, v2, v3});
    }
};

export template <typename T>
struct Corners<T> {
    T topLeft;
    T topRight;
    T bottomLeft;
    T bottomRight;

    Corners(T a) : topLeft(a), topRight(a), bottomLeft(a), bottomRight(a) {}

    Corners(T a, T b) : topLeft(a), topRight(b), bottomLeft(a), bottomRight(b) {}

    Corners(T a, T b, T c) : topLeft(a), topRight(b), bottomLeft(c), bottomRight(b) {}

    Corners(T a, T b, T c, T d) : topLeft(a), topRight(b), bottomLeft(c), bottomRight(d) {}
};

export template <typename T>
struct ValueTraits<Corners<T>> {
    Res<Edges<T>> parse(Cursor<Css::Sst> c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto v0 = try$(parseValue<T>(c));

        if (c.ended())
            return Ok(Corners<T>{v0});

        auto v1 = try$(parseValue<T>(c));

        if (c.ended())
            return Ok(Corners<T>{v0, v1});

        auto v2 = try$(parseValue<T>(c));

        if (c.ended())
            return Ok(Corners<T>{v0, v1, v2});

        auto v3 = try$(parseValue<T>(c));

        if (c.ended())
            return Ok(Corners<T>{v0, v1, v2, v3});
    }
};

struct Border {
    LineWidth width = Keywords::MEDIUM;
    Gfx::BorderStyle style = Gfx::BorderStyle::NONE;
    Color color = Keywords::CURRENT_COLOR;

    void repr(Io::Emit& e) const {
        e("(border {} {} {})", width, style, color);
    }
};

// FIXME: Remove
export struct UsedBorder {
    Au width{};
    Gfx::BorderStyle style{};
    Gfx::Color color{};
};

export using UsedBorders = Math::Insets<UsedBorder>;

export template <>
struct ValueTraits<Border> {
    struct ComputedType {
        Computed<LineWidth> width;
        Computed<Gfx::BorderStyle> style;
        Computed<Color> color;
    };

    static Res<Border> parse(Cursor<Css::Sst>& c) {
        return parseOneOrMoreUnordered<Border, &Border::width, &Border::style, &Border::color>(c);
    }

    static ComputedType compute(Border const& border, ComputationContext const& ctx) {
        return {
            .width = computeValue(border.width, ctx),
            .style = computeValue(border.style, ctx),
            .color = computeValue(border.color, ctx),
        };
    }

    static Border fromComputed(ComputedType const& computed) {
        return Border{
            .width = valueFromComputed<LineWidth>(computed.width),
            .style = valueFromComputed<Gfx::BorderStyle>(computed.style),
            .color = valueFromComputed<Color>(computed.color),
        };
    }
};

// FIXME: cant we have this for karm/insets?
export enum struct BorderEdge {
    TOP,
    START,
    BOTTOM,
    END,
};

using BorderRadius = Pair<CalcValue<PercentOr<Length>>>;

export struct BorderRadii {
    BorderRadius topLeft, topRight, bottomRight, bottomLeft;

    constexpr BorderRadii(BorderRadius topLeft, BorderRadius topRight, BorderRadius bottomRight, BorderRadius bottomLeft)
        : topLeft(topLeft), topRight(topRight), bottomRight(bottomRight), bottomLeft(bottomLeft) {}

    void repr(Io::Emit& e) const {
        e("(border-radii");
        e(" topLeft={}", topLeft);
        e(" topRight={}", topRight);
        e(" bottomRight={}", bottomRight);
        e(" bottomLeft={}", bottomLeft);
        e(")");
    }
};

export template <>
struct ValueTraits<BorderRadii> {
    using ComputedType = struct {
        Computed<BorderRadius> topLeft, topRight, bottomRight, bottomLeft;
    };

    static Res<BorderRadii> parse(Cursor<Css::Sst>& c) {
        auto horizonal = try$(parseValue<Corners<CalcValue<PercentOr<Length>>>>(c));

        eatWhitespace(c);
        if (not c.ended() and c.peek().token.data == "/"s) {
            c.next();
            eatWhitespace(c);

            auto vertical = try$(parseValue<Corners<CalcValue<PercentOr<Length>>>>(c));

            return Ok(
                BorderRadii(
                    {horizonal.topLeft, vertical.topLeft},
                    {horizonal.topRight, vertical.topRight},
                    {horizonal.bottomLeft, vertical.bottomLeft},
                    {horizonal.bottomRight, vertical.bottomRight}
                )
            );
        }

        return Ok(
            BorderRadii(
                {horizonal.topLeft},
                {horizonal.topRight},
                {horizonal.bottomLeft},
                {horizonal.bottomRight}
            )
        );
    }

    static ComputedType compute(BorderRadii const& radii, ComputationContext const& ctx) {
        return {
            .topLeft = computeValue(radii.topLeft, ctx),
            .topRight = computeValue(radii.topRight, ctx),
            .bottomRight = computeValue(radii.bottomRight, ctx),
            .bottomLeft = computeValue(radii.bottomLeft, ctx),
        };
    }

    static BorderRadii fromComputed(ComputedType const& computed) {
        return {
            .topLeft = valueFromComputed<BorderRadius>(computed.topLeft),
            .topRight = valueFromComputed<BorderRadius>(computed.topRight),
            .bottomRight = valueFromComputed<BorderRadius>(computed.bottomRight),
            .bottomLeft = valueFromComputed<BorderRadius>(computed.bottomLeft),
        };
    }
};

} // namespace Vaev
