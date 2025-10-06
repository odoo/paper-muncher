export module Vaev.Engine:values.outline;

import Karm.Gfx;
import Karm.Math;

import :values.color;
import :values.lineWidth;

namespace Vaev {

// https://drafts.csswg.org/css-ui/#outline
export struct Outline {
    LineWidth width = Keywords::MEDIUM;
    CalcValue<Length> offset = 0_au;
    Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
    Union<Keywords::Auto, Color> color = Keywords::AUTO;

    void repr(Io::Emit& e) const {
        e("(outline {} {} {} {})", width, offset, style, color);
    }
};

} // namespace Vaev
