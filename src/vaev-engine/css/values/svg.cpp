module;

#include <karm/macros>

export module Vaev.Engine:css.values.svg;

import :css.values.base;
import :css.values.color;
import :css.values.keywords;
import :css.values.primitives;

using namespace Karm::Literals;

namespace Vaev::Css {

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

// https://svgwg.org/svg2-draft/coords.html#PreserveAspectRatioAttribute
export using SvgAlign = Union<None, SvgAlignAxis>;

// https://svgwg.org/svg2-draft/coords.html#PreserveAspectRatioAttribute
export enum struct SvgMeetOrSlice {
    MEET,
    SLICE
};

// https://svgwg.org/svg2-draft/painting.html#SpecifyingPaint
export using SvgPaint = Opt<Color>;

// https://svgwg.org/svg2-draft/painting.html#SpecifyingPaint
export template <>
struct ValueParser<SvgPaint> {
    static Res<SvgPaint> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("none")))
            return Ok(NONE);

        return Ok(Some(try$(parseValue<Color>(c))));
    }
};

export template <>
struct ValueSerializer<SvgPaint> {
    // https://svgwg.org/svg2-draft/painting.html#SpecifyingPaint
    static Res<> serialize(Io::TextWriter& w, SvgPaint const& value) {
        if (auto const& [color] = value)
            return serializeValue(w, color);
        return w.writeStr("none"s);
    }
};

// https://svgwg.org/svg2-draft/coords.html#ViewBoxAttribute
export struct SvgViewBox {
    f64 minX;
    f64 minY;
    f64 width;
    f64 height;

    void repr(Io::Emit& e) const {
        e("(ViewBox minX={} minY={} width={} height={})", minX, minY, width, height);
    }
};

export template <>
struct ValueSerializer<SvgViewBox> {
    static Res<> serialize(Io::TextWriter& w, SvgViewBox const& value) {
        try$(serializeValue(w, value.minX));
        try$(w.writeRune(' '));
        try$(serializeValue(w, value.minY));
        try$(w.writeRune(' '));
        try$(serializeValue(w, value.width));
        try$(w.writeRune(' '));
        return serializeValue(w, value.height);
    }
};

} // namespace Vaev::Css
