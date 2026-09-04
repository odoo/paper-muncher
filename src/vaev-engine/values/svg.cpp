module;

#include <karm/macros>

export module Vaev.Engine:values.svg;

import Karm.Gfx;
import Karm.Math;

import :css;
import :values.base;
import :values.color;
import :values.length;
import :values.percent;

using namespace Karm::Literals;
using namespace Karm::Math::Literals;

namespace Vaev {

// MARK: PreserveAspectRatio
// https://svgwg.org/svg2-draft/coords.html#PreserveAspectRatioAttribute
export struct SvgAlignAxis {
    enum struct _AlignAxis {
        MIN,
        MID,
        MAX
    };

    using enum _AlignAxis;

    _AlignAxis x;
    _AlignAxis y;
};

export using SvgAlign = Union<None, SvgAlignAxis>;

// https://svgwg.org/svg2-draft/coords.html#PreserveAspectRatioAttribute
export enum struct SvgMeetOrSlice {
    MEET,
    SLICE
};

// MARK: Paint
// TODO: still not complete type
// https://svgwg.org/svg2-draft/painting.html#SpecifyingPaint
export using SvgPaint = Opt<Color>;

Opt<Gfx::Color> resolve(SvgPaint color, Gfx::Color currentColor) {
    if (color == NONE)
        return NONE;
    return Some(Vaev::resolve(color.unwrap(), currentColor));
}

// https://svgwg.org/svg2-draft/coords.html#ViewBoxAttribute
export struct SvgViewBox {
    Number minX;
    Number minY;
    Number width;
    Number height;

    void repr(Io::Emit& e) const {
        e("(ViewBox minX={} minY={} width={} height={})", minX, minY, width, height);
    }
};

// https://svgwg.org/svg2-draft/shapes.html#TermShapeElement
enum struct SvgShapeElement {
    RECT,
    CIRCLE,
    PATH,

    _LEN
};

// The geometry of an SVG element. None of it is inherited.
export struct SvgProps {
    PercentOr<Length> x = Length{0_au};
    PercentOr<Length> y = Length{0_au};
    PercentOr<Length> cx = Length{0_au};
    PercentOr<Length> cy = Length{0_au};
    PercentOr<Length> r = Length{0_au};

    Union<String, None> d = NONE;
    Opt<SvgViewBox> viewBox = NONE;

    void repr(Io::Emit& e) const {
        e("(svg");
        e(" x={}", x);
        e(" y={}", y);
        e(" cx={}", cx);
        e(" cy={}", cy);
        e(" r={}", r);
        e(" d={}", d);
        e(" viewBox={}", viewBox);
        e(")");
    }
};

// How an SVG element is painted. All of it is inherited, which is what keeps it
// apart from the geometry above: a group holding both would be copied for every
// element that merely inherits a fill, since the copy-on-write is per group.
// https://svgwg.org/svg2-draft/painting.html
export struct SvgPaintProps {
    Number fillOpacity = 1;
    PercentOr<Length> strokeWidth = Length{1_au};
    Number strokeOpacity = 1;
    SvgPaint fill = Some(Gfx::BLACK);
    SvgPaint stroke = NONE;

    void repr(Io::Emit& e) const {
        e("(svg-paint");
        e(" fillOpacity={}", fillOpacity);
        e(" strokeWidth={}", strokeWidth);
        e(" strokeOpacity={}", strokeOpacity);
        e(" fill={}", fill);
        e(" stroke={}", stroke);
        e(")");
    }
};

// MARK: Paint
// https://svgwg.org/svg2-draft/painting.html#SpecifyingPaint
export template <>
struct ValueParser<SvgPaint> {
    static Res<SvgPaint> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok(NONE);

        return Ok(Some(try$(parseValue<Color>(c))));
    }
};

} // namespace Vaev
