module;

#include <karm/macros>

export module Vaev.Engine:css.values.svg;

import :css.lexer;
import :css.parser;
import :css.serializer;
import :css.values.base;
import :css.values.color;
import :css.values.keywords;
import :css.values.primitives;

using namespace Karm::Literals;

namespace Vaev::Css {

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

// MARK: Paint
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

} // namespace Vaev::Css
