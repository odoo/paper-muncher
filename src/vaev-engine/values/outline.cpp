export module Vaev.Engine:values.outline;

import Karm.Gfx;
import Karm.Math;

import :values.calc2;
import :values.color;
import :values.length;
import :values.lineWidth;

using namespace Vaev::Literals;

namespace Vaev {

export struct SpecifiedOutline {
    LineWidth width = Keywords::MEDIUM;
    Length offset = Length::fromCanonical(0);
    Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
    Union<Keywords::Auto, Color> color = Keywords::AUTO;

    void repr(Io::Emit& e) const {
        e("(outline {} {} {} {})", width, offset, style, color);
    }
};

// https://drafts.csswg.org/css-ui/#outline
export struct OutlineProps {
    Pixel width = 3_px;
    Pixel offset = Pixel{0};
    Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
    Color color = Gfx::BLUE500;

    operator SpecifiedOutline() const {
        return {
            toSpecified(width),
            toSpecified(offset),
            style,
            Color{color}
        };
    }
};

} // namespace Vaev
