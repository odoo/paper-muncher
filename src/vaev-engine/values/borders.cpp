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
    Color color{};
};
export using UsedBorders = Math::Insets<UsedBorder>;

export template <>
struct ValueTraits<Border> {
    using ComputedType = struct {
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
        return Border {
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

    constexpr BorderRadii(BorderRadius all = {})
        : topLeft(all), topRight(all), bottomRight(all), bottomLeft(all) {}

    constexpr BorderRadii(BorderRadius startEnd, BorderRadius endStart)
        : BorderRadii(startEnd, endStart, startEnd, endStart) {}

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
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto value1 = parseValue<CalcValue<PercentOr<Length>>>(c);
        if (not value1)
            return Ok(parsePostSlash(c, BorderRadii{}));

        auto value2 = parseValue<CalcValue<PercentOr<Length>>>(c);
        if (not value2)
            return Ok(parsePostSlash(c, BorderRadii{{value1.take()}}));

        auto value3 = parseValue<PercentOr<Length>>(c);
        if (not value3)
            return Ok(parsePostSlash(c, BorderRadii{{value1.take()}, {value2.take()}}));

        auto value4 = parseValue<PercentOr<Length>>(c);
        if (not value4)
            return Ok(parsePostSlash(c, BorderRadii{{value1.take()}, {value2.take()}, {value3.take()}, {value2.take()}}));

        return Ok(parsePostSlash(c, BorderRadii{{value1.take()}, {value2.take()}, {value3.take()}, {value4.take()}}));
    }

    static BorderRadii parsePostSlash(Cursor<Css::Sst>& c, BorderRadii radii) {
        // if parse a /
        // 1 value-- > border all(a, d, e, h)
        // 2 values-- > 1 = top - start + bottom - end 2 = the others
        // 3 values-- > 1 = top - start, 2 = top - end + bottom - start, 3 = bottom - end
        // 4 values-- > 1 = top - start, 2 = top - end 3 = bottom - end, 4 = bottom - start
        eatWhitespace(c);
        if (not c.ended() and c.peek().token.data == "/"s) {
            c.next();
            eatWhitespace(c);
            auto value1 = parseValue<PercentOr<Length>>(c);
            if (not value1) {
                return radii;
            }

            auto value2 = parseValue<PercentOr<Length>>(c);
            if (not value2) {
                radii.topLeft.v1 = value1.unwrap();
                radii.topRight.v1 = value1.unwrap();
                radii.bottomRight.v1 = value1.unwrap();
                radii.bottomLeft.v1 = value1.unwrap();
                return radii;
            }

            eatWhitespace(c);
            auto value3 = parseValue<PercentOr<Length>>(c);
            if (not value3) {
                radii.topLeft.v1 = value1.unwrap();
                radii.topRight.v1 = value2.unwrap();
                radii.bottomRight.v1 = value1.unwrap();
                radii.bottomLeft.v1 = value2.unwrap();
                return radii;
            }

            eatWhitespace(c);
            auto value4 = parseValue<PercentOr<Length>>(c);
            if (not value4) {
                radii.topLeft.v1 = value1.take();
                radii.topRight.v1 = value2.take();
                radii.bottomRight.v1 = value3.take();
                radii.bottomLeft.v1 = value2.take();

                return radii;
            }

            radii.topLeft.v1 = value1.take();
            radii.topRight.v1 = value2.take();
            radii.bottomRight.v1 = value3.take();
            radii.bottomLeft.v1 = value4.take();
            return radii;
        }

        return radii;
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
