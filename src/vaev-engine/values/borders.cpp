export module Vaev.Engine:values.borders;

import Karm.Gfx;
import Karm.Math;

import :css;
import :values.base;
import :values.calc2;
import :values.color;
import :values.length;
import :values.lineWidth;
import :values.percent;
import :values.unitValue;

using namespace Vaev::Literals;

namespace Vaev::Css {

// MARK: Border Style ----------------------------------------------------------
// https://www.w3.org/TR/CSS22/box.html#border-style-properties

export template <>
struct ValueParser<Gfx::BorderStyle> {
    static Res<Gfx::BorderStyle> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of property");

        if (c.skip(Token::ident("none"))) {
            return Ok(Gfx::BorderStyle::NONE);
        } else if (c.skip(Token::ident("solid"))) {
            return Ok(Gfx::BorderStyle::SOLID);
        } else if (c.skip(Token::ident("dashed"))) {
            return Ok(Gfx::BorderStyle::DASHED);
        } else if (c.skip(Token::ident("dotted"))) {
            return Ok(Gfx::BorderStyle::DOTTED);
        } else if (c.skip(Token::ident("hidden"))) {
            return Ok(Gfx::BorderStyle::HIDDEN);
        } else if (c.skip(Token::ident("double"))) {
            return Ok(Gfx::BorderStyle::DOUBLE);
        } else if (c.skip(Token::ident("groove"))) {
            return Ok(Gfx::BorderStyle::GROOVE);
        } else if (c.skip(Token::ident("ridge"))) {
            return Ok(Gfx::BorderStyle::RIDGE);
        } else if (c.skip(Token::ident("outset"))) {
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

} // namespace Vaev::Css

namespace Vaev::Style {

export struct Border {
    Pixel width = 3_px;
    Gfx::BorderStyle style = Gfx::BorderStyle::NONE;
    Style::Color color = Gfx::BLACK;

    operator Css::SpecifiedBorder() const {
        return {toSpecified(width), style, toSpecified(color)};
    }
};

} // namespace Vaev::Style

namespace Vaev::Css {

export struct UsedBorder {
    Au width = 0_au;
    Gfx::BorderStyle style = Gfx::BorderStyle::NONE;
    Gfx::Color color = Gfx::BLACK;

    void repr(Io::Emit& e) const {
        e("(used-border {} {})", style, color);
    }
};

export using UsedBorders = Math::Insets<UsedBorder>;

export template <>
struct ValueParser<SpecifiedBorder> {
    static Res<SpecifiedBorder> parse(Cursor<Sst>& c) {
        // https://drafts.csswg.org/css-backgrounds/#border-shorthands
        // <line-width> || <line-style> || <color>, each at most once.
        SpecifiedBorder border;
        bool widthSet = false;
        bool styleSet = false;
        bool colorSet = false;
        while (not c.ended()) {
            eatWhitespace(c);

            auto width = parseValue<LineWidth>(c);
            if (width) {
                if (widthSet)
                    return Error::invalidData("duplicated border width");
                border.width = width.expect();
                widthSet = true;
                continue;
            }

            auto color = parseValue<Color>(c);
            if (color) {
                if (colorSet)
                    return Error::invalidData("duplicated border color");
                border.color = color.expect();
                colorSet = true;
                continue;
            }

            auto style = parseValue<Gfx::BorderStyle>(c);
            if (style) {
                if (styleSet)
                    return Error::invalidData("duplicated border style");
                border.style = style.expect();
                styleSet = true;
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

// NOTE: Math::Radii::repr() needs T to be constructible from 0, which PixelPercentage isn't.
export template <typename T>
void reprRadii(Io::Emit& e, Math::Radii<T> const& radii) {
    e("(radii {} {} {} {} {} {} {} {})", radii.a, radii.b, radii.c, radii.d, radii.e, radii.f, radii.g, radii.h);
}

} // namespace Vaev::Css

namespace Vaev::Style {

export struct Borders {
    Style::Border top, start, bottom, end;
    Math::Radii<PixelPercentage> radii = {Pixel{0}};

    Style::Border const& get(Css::BorderEdge edge) const {
        switch (edge) {
        case Css::BorderEdge::TOP:
            return top;
        case Css::BorderEdge::START:
            return start;
        case Css::BorderEdge::BOTTOM:
            return bottom;
        case Css::BorderEdge::END:
            return end;
        }
    }
};

} // namespace Vaev::Style

namespace Vaev::Css {

export template <typename T>
struct ValueParser<Math::Radii<T>> {
    static Res<Math::Radii<T>> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        eatWhitespace(c);
        auto value1 = parseValue<T>(c);
        if (not value1)
            return parsePostSlash(c, NONE);

        eatWhitespace(c);
        auto value2 = parseValue<T>(c);
        if (not value2)
            return parsePostSlash(c, Some(Math::Radii<T>{value1.take()}));

        eatWhitespace(c);
        auto value3 = parseValue<T>(c);
        if (not value3)
            return parsePostSlash(c, Some(Math::Radii<T>{value1.take(), value2.take()}));

        eatWhitespace(c);
        auto value4 = parseValue<T>(c);
        if (not value4) {
            auto topEndAndBottomStart = value2.take();
            return parsePostSlash(c, Some(Math::Radii<T>{value1.take(), topEndAndBottomStart, value3.take(), topEndAndBottomStart}));
        }

        return parsePostSlash(c, Some(Math::Radii<T>{value1.take(), value2.take(), value3.take(), value4.take()}));
    }

    static Res<Math::Radii<T>> parsePostSlash(Cursor<Sst>& c, Opt<Math::Radii<T>> maybeRadii) {
        // if parse a /
        // 1 value-- > border all(a, d, e, h)
        // 2 values-- > 1 = top - start + bottom - end 2 = the others
        // 3 values-- > 1 = top - start, 2 = top - end + bottom - start, 3 = bottom - end
        // 4 values-- > 1 = top - start, 2 = top - end 3 = bottom - end, 4 = bottom - start
        eatWhitespace(c);

        if (c.ended() or c.peek().token.data != "/"s)
            return maybeRadii.okOr(Error::invalidData("expected border radius"));

        auto radii = maybeRadii.unwrapOr(Math::Radii<T>{T::fromCanonical(0)});
        c.next();
        eatWhitespace(c);
        auto value1 = parseValue<T>(c);
        if (not value1) {
            return Ok(std::move(radii));
        }

        eatWhitespace(c);
        auto value2 = parseValue<T>(c);
        if (not value2) {
            radii.a = value1.expect();
            radii.d = value1.expect();
            radii.e = value1.expect();
            radii.h = value1.expect();
            return Ok(std::move(radii));
        }

        eatWhitespace(c);
        auto value3 = parseValue<T>(c);
        if (not value3) {
            radii.a = value1.expect();
            radii.d = value2.expect();
            radii.e = value1.expect();
            radii.h = value2.expect();
            return Ok(std::move(radii));
        }

        eatWhitespace(c);
        auto value4 = parseValue<T>(c);
        if (not value4) {
            radii.a = value1.expect();
            radii.d = value2.expect();
            radii.e = value3.expect();
            radii.h = value2.expect();

            return Ok(std::move(radii));
        }

        radii.a = value1.expect();
        radii.d = value2.expect();
        radii.e = value3.expect();
        radii.h = value4.expect();

        return Ok(std::move(radii));
    }
};

} // namespace Vaev::Css

namespace Vaev::Style {

export Math::Radii<PixelPercentage> resolve(Math::Radii<Css::LengthPercentage> const& value, Css::LengthContext auto const& ctx) {
    return {
        resolve(value.a, ctx),
        resolve(value.b, ctx),
        resolve(value.c, ctx),
        resolve(value.d, ctx),
        resolve(value.e, ctx),
        resolve(value.f, ctx),
        resolve(value.g, ctx),
        resolve(value.h, ctx),
    };
}

export Math::Radii<Css::LengthPercentage> toSpecified(Math::Radii<PixelPercentage> const& value) {
    return {
        toSpecified(value.a),
        toSpecified(value.b),
        toSpecified(value.c),
        toSpecified(value.d),
        toSpecified(value.e),
        toSpecified(value.f),
        toSpecified(value.g),
        toSpecified(value.h),
    };
}

} // namespace Vaev::Style

