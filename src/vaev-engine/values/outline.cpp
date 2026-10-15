export module Vaev.Engine:values.outline;

import Karm.Gfx;
import Karm.Math;

import :values.color;
import :values.lineWidth;
import :values.resolved;

namespace Vaev {

export struct SpecifiedOutline {
    LineWidth width = Keywords::MEDIUM;
    Calc<Length> offset = 0_au;
    Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
    Union<Keywords::Auto, Color> color = Keywords::AUTO;

    void repr(Io::Emit& e) const {
        e("(outline {} {} {} {})", width, offset, style, color);
    }
};

// https://drafts.csswg.org/css-ui/#outline
export struct OutlineProps {
    Computed<LineWidth> width = MEDIUM_VALUE;
    Computed<Calc<Length>> offset = 0_au;
    Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
    Color color = Gfx::BLUE500;

    operator SpecifiedOutline() const {
        return {
            specified<LineWidth>(width),
            specified<Calc<Length>>(offset),
            style,
            Color{color}
        };
    }
};

} // namespace Vaev
