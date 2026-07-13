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

export struct SpecifiedBorder {
    LineWidth width = Keywords::MEDIUM;
    Gfx::BorderStyle style = Gfx::BorderStyle::NONE;
    Color color = Keywords::CURRENT_COLOR;

    void repr(Io::Emit& e) const {
        e("(specified-border {} {} {})", width, style, color);
    }
};

export struct ComputedBorder {
    LineWidth width = Keywords::MEDIUM;
    Gfx::BorderStyle style = Gfx::BorderStyle::NONE;
    Color color = Gfx::BLACK;

    operator SpecifiedBorder() const {
        return {width, style, color};
    }

    void repr(Io::Emit& e) const {
        e("(computed-border {} {} {})", width, style, color);
    }
};

export struct UsedBorder {
    Au width;
    Gfx::BorderStyle style = Gfx::BorderStyle::NONE;
    Gfx::Color color = Gfx::BLACK;

    void repr(Io::Emit& e) const {
        e("(used-border {} {} {})", width, style, color);
    }
};

export using UsedBorders = Math::Insets<UsedBorder>;

export template <>
struct ValueParser<SpecifiedBorder> {
    static Res<SpecifiedBorder> parse(Cursor<Css::Sst>& c) {
        SpecifiedBorder border;
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

export struct BorderProps {
    ComputedBorder top, start, bottom, end;
    Math::Radii<CalcValue<PercentOr<Length>>> radii = {Length{0_au}};

    ComputedBorder const& get(BorderEdge edge) const {
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
        e(" radii={}", radii);
        e(")");
    }
};

export template <typename T>
struct ValueParser<Math::Radii<T>> {
    static Res<Math::Radii<T>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto value1 = parseValue<PercentOr<Length>>(c);
        if (not value1)
            return parsePostSlash(c, NONE);

        auto value2 = parseValue<PercentOr<Length>>(c);
        if (not value2)
            return parsePostSlash(c, Math::Radii<T>{value1.take()});

        auto value3 = parseValue<PercentOr<Length>>(c);
        if (not value3)
            return parsePostSlash(c, Math::Radii<T>{value1.take(), value2.take()});

        auto value4 = parseValue<PercentOr<Length>>(c);
        if (not value4)
            return parsePostSlash(c, Math::Radii<T>{value1.take(), value2.take(), value3.take(), value2.take()});

        return parsePostSlash(c, Math::Radii<T>{value1.take(), value2.take(), value3.take(), value4.take()});
    }

    static Res<Math::Radii<T>> parsePostSlash(Cursor<Css::Sst>& c, Opt<Math::Radii<T>> maybeRadii) {
        // if parse a /
        // 1 value-- > border all(a, d, e, h)
        // 2 values-- > 1 = top - start + bottom - end 2 = the others
        // 3 values-- > 1 = top - start, 2 = top - end + bottom - start, 3 = bottom - end
        // 4 values-- > 1 = top - start, 2 = top - end 3 = bottom - end, 4 = bottom - start
        eatWhitespace(c);

        if (c.ended() or c.peek().token.data != "/"s)
            return maybeRadii.okOr(Error::invalidData("expected border radius"));

        auto radii = maybeRadii.unwrapOr(Math::Radii<T>{Length{0_au}});
        c.next();
        eatWhitespace(c);
        auto value1 = parseValue<PercentOr<Length>>(c);
        if (not value1) {
            return Ok(std::move(radii));
        }

        auto value2 = parseValue<PercentOr<Length>>(c);
        if (not value2) {
            radii.a = value1.unwrap();
            radii.d = value1.unwrap();
            radii.e = value1.unwrap();
            radii.h = value1.unwrap();
            return Ok(std::move(radii));
        }

        eatWhitespace(c);
        auto value3 = parseValue<PercentOr<Length>>(c);
        if (not value3) {
            radii.a = value1.unwrap();
            radii.d = value2.unwrap();
            radii.e = value1.unwrap();
            radii.h = value2.unwrap();
            return Ok(std::move(radii));
        }

        eatWhitespace(c);
        auto value4 = parseValue<PercentOr<Length>>(c);
        if (not value4) {
            radii.a = value1.unwrap();
            radii.d = value2.unwrap();
            radii.e = value3.unwrap();
            radii.h = value2.unwrap();

            return Ok(std::move(radii));
        }

        radii.a = value1.unwrap();
        radii.d = value2.unwrap();
        radii.e = value3.unwrap();
        radii.h = value4.unwrap();

        return Ok(std::move(radii));
    }
};

} // namespace Vaev
