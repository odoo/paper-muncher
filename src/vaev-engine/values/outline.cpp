export module Vaev.Engine:values.outline;

import Karm.Gfx;
import Karm.Math;

import :values.calc2;
import :values.color;
import :values.length;
import :values.lineWidth;

using namespace Vaev::Literals;

namespace Vaev::Css {

export struct SpecifiedOutline {
    LineWidth width = Keywords::MEDIUM;
    Length offset = Length::fromCanonical(0);
    Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
    Union<Keywords::Auto, Color> color = Keywords::AUTO;

    void repr(Io::Emit& e) const {
        e("(outline {} {} {} {})", width, offset, style, color);
    }
};

} // namespace Vaev::Css

namespace Vaev::Style {

// https://drafts.csswg.org/css-ui/#outline

export struct Outline {
    Pixel width = 3_px;
    Pixel offset = Pixel{0};
    Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
    Gfx::Color color = Gfx::BLUE500;

    operator Css::SpecifiedOutline() const {
        return {
            toSpecified(width),
            toSpecified(offset),
            style,
            Css::Color{color}
        };
    }
};

} // namespace Vaev::Style

