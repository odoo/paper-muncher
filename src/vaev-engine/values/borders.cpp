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
struct ValueParser<Gfx::BorderStyle> {
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

template <typename T>
struct _Border {
    T width;
    Gfx::BorderStyle style = Gfx::BorderStyle::NONE;
    Color color = Keywords::CURRENT_COLOR;

    _Border();
    _Border(T width, Gfx::BorderStyle style, Color color)
        : width(width), style(style), color(color) {};

    void repr(Io::Emit& e) const {
        e("(border {} {} {})", width, style, color);
    }
};

export using Border = _Border<LineWidth>;

template <>
_Border<LineWidth>::_Border() : width(Keywords::MEDIUM) {}

export using UsedBorder = _Border<Au>;
export using UsedBorders = Math::Insets<UsedBorder>;

template <>
_Border<Au>::_Border() : width(0_au) {}

export template <>
struct ValueParser<Border> {
    static Res<Border> parse(Cursor<Css::Sst>& c) {
        Border border;
        while (not c.ended()) {
            eatWhitespace(c);

            auto width = parseValue<LineWidth>(c);
            if (width) {
                border.width = width.unwrap();
                continue;
            }

            auto color = parseValue<Color>(c);
            if (color) {
                border.color = color.unwrap();
                continue;
            }

            auto style = parseValue<Gfx::BorderStyle>(c);
            if (style) {
                border.style = style.unwrap();
                continue;
            }

            break;
        }

        return Ok(border);
    }
};

// FIXME: cant we have this for karm/insets?
export enum struct BorderEdge {
    TOP,
    START,
    BOTTOM,
    END,
};

struct BorderRadius {
    LengthPercentage h = Length{0_au};
    LengthPercentage v = Length{0_au};

    BorderRadius() = default;
    BorderRadius(LengthPercentage val) : h(val), v(val) {}
    BorderRadius(LengthPercentage h, LengthPercentage v) : h(h), v(v) {}

    void repr(Io::Emit& e) const {
        e("(border-radius {} {})", h, v);
    }
};

struct BorderRadii {
    BorderRadius topLeft{};
    BorderRadius topRight{};
    BorderRadius bottomRight{};
    BorderRadius bottomLeft{};

    BorderRadii(BorderRadius a, BorderRadius b, BorderRadius c, BorderRadius d)
        : topLeft(a), topRight(b), bottomRight(c), bottomLeft(d) {}

    BorderRadii(BorderRadius a, BorderRadius b, BorderRadius c)
        : BorderRadii(a, b, c, b) {}

    BorderRadii(BorderRadius a, BorderRadius b)
        : BorderRadii(a, b, a, b) {}

    BorderRadii(BorderRadius a)
        : BorderRadii(a, a, a, a) {}

    BorderRadii() = default;

    void repr(Io::Emit& e) const {
        e(
            "(border-radius {} {} {} {} / {} {} {} {})",
            topLeft.h, topRight.h, bottomRight.h, bottomLeft.h,
            topLeft.v, topRight.v, bottomRight.v, bottomLeft.v
        );
    }
};

export struct BorderProps {
    Border top, start, bottom, end;
    BorderRadii radii;

    void all(Border b) {
        top = start = bottom = end = b;
    }

    Border const& get(BorderEdge edge) const {
        switch (edge) {
        case BorderEdge::TOP:
            return top;
        case BorderEdge::START:
            return start;
        case BorderEdge::BOTTOM:
            return bottom;
        case BorderEdge::END:
            return end;
        }
    }

    void repr(Io::Emit& e) const {
        e("(borders");
        e(" top={}", top);
        e(" start={}", start);
        e(" bottom={}", bottom);
        e(" end={}", end);
        e(" radiis={}", radii);
        e(")");
    }
};

export template <>
struct ValueParser<BorderRadii> {
    static Res<BorderRadii> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto value1 = parseValue<LengthPercentage>(c);
        if (not value1)
            return Ok(parsePostSlash(c, BorderRadii{BorderRadius{}}));

        auto value2 = parseValue<LengthPercentage>(c);
        if (not value2)
            return Ok(parsePostSlash(c, BorderRadii{{value1.unwrap()}}));

        auto value3 = parseValue<LengthPercentage>(c);
        if (not value3)
            return Ok(parsePostSlash(c, BorderRadii{value1.unwrap(), value2.unwrap()}));

        auto value4 = parseValue<LengthPercentage>(c);
        if (not value4)
            return Ok(parsePostSlash(c, BorderRadii{value1.unwrap(), value2.unwrap(), value3.unwrap(), value2.unwrap()}));

        return Ok(parsePostSlash(c, BorderRadii{value1.unwrap(), value2.unwrap(), value3.unwrap(), value4.unwrap()}));
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
            auto value1 = parseValue<LengthPercentage>(c);
            if (not value1) {
                return radii;
            }

            auto value2 = parseValue<LengthPercentage>(c);
            if (not value2) {
                radii.topLeft.v = value1.unwrap();
                radii.topRight.v = value1.unwrap();
                radii.bottomRight.v = value1.unwrap();
                radii.bottomLeft.v = value1.unwrap();
                return radii;
            }

            eatWhitespace(c);
            auto value3 = parseValue<LengthPercentage>(c);
            if (not value3) {
                radii.topLeft.v = value1.unwrap();
                radii.topRight.v = value2.unwrap();
                radii.bottomRight.v = value1.unwrap();
                radii.bottomLeft.v = value2.unwrap();
                return radii;
            }

            eatWhitespace(c);
            auto value4 = parseValue<LengthPercentage>(c);
            if (not value4) {
                radii.topLeft.v = value1.unwrap();
                radii.topRight.v = value2.unwrap();
                radii.bottomRight.v = value3.unwrap();
                radii.bottomLeft.v = value2.unwrap();

                return radii;
            }

            radii.topLeft.v = value1.unwrap();
            radii.topRight.v = value2.unwrap();
            radii.bottomRight.v = value3.unwrap();
            radii.bottomLeft.v = value4.unwrap();

            return radii;
        }

        return radii;
    }
};

} // namespace Vaev
