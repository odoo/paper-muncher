#pragma once

#include <karm-gfx/color.h>

#include "base.h"
#include "color.h"
#include "length.h"
#include "percent.h"

namespace Vaev {

// MARK: PreserveAspectRatio
// https://svgwg.org/svg2-draft/coords.html#PreserveAspectRatioAttribute
struct AlignAxisSVG {
    enum struct _AlignAxis {
        MIN,
        MID,
        MAX
    };

    using enum _AlignAxis;

    _AlignAxis x;
    _AlignAxis y;
};

using AlignSVG = Union<None, AlignAxisSVG>;

// https://svgwg.org/svg2-draft/coords.html#PreserveAspectRatioAttribute
enum struct MeetOrSlice {
    MEET,
    SLICE
};

// MARK: Paint
// TODO: still not complete type
// https://svgwg.org/svg2-draft/painting.html#SpecifyingPaint
using Paint = Union<Color, None>;

// https://svgwg.org/svg2-draft/coords.html#ViewBoxAttribute
struct ViewBox {
    Number minX;
    Number minY;
    Number width;
    Number height;

    void repr(Io::Emit& e) const {
        e("(ViewBox minX={} minY={} width={} height={})", minX, minY, width, height);
    }
};

struct SVGProps {
    PercentOr<Length> x = Length{0_au};
    PercentOr<Length> y = Length{0_au};
    PercentOr<Length> cx = Length{0_au};
    PercentOr<Length> cy = Length{0_au};
    PercentOr<Length> r = Length{0_au};

    Number fillOpacity = 1;
    PercentOr<Length> strokeWidth = Length{1_au};

    Union<String, None> d = NONE;

    Paint fill = Color{Gfx::BLACK};
    Paint stroke = NONE;

    Opt<ViewBox> viewBox = NONE;

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
template <>
struct ValueParser<Paint> {
    static Res<Paint> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok(NONE);

        return Ok(try$(parseValue<Color>(c)));
    }
};

} // namespace Vaev
