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
export using SvgPaint = Union<Color, None>;

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

export struct SvgProps {
    PercentOr<Length> x = Length{0_au};
    PercentOr<Length> y = Length{0_au};
    PercentOr<Length> cx = Length{0_au};
    PercentOr<Length> cy = Length{0_au};
    PercentOr<Length> r = Length{0_au};

    Number fillOpacity = 1;
    PercentOr<Length> strokeWidth = Length{1_au};
    Number strokeOpacity = 1;
    Union<String, None> d = NONE;
    SvgPaint fill = Color{Gfx::BLACK};
    SvgPaint stroke = NONE;
    Opt<SvgViewBox> viewBox = NONE;

    void repr(Io::Emit& e) const {
        e("(svg");
        e(" x={}", x);
        e(" y={}", y);
        e(" cx={}", cx);
        e(" cy={}", cy);
        e(" r={}", r);
        e(" fillOpacity={}", fillOpacity);
        e(" strokeWidth={}", strokeWidth);
        e(" d={}", d);
        e(" fill={}", fill);
        e(" stroke={}", stroke);
        e(" viewBox={}", viewBox);
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

        return Ok(try$(parseValue<Color>(c)));
    }
};

} // namespace Vaev
