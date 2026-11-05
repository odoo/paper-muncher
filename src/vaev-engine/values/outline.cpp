export module Vaev.Engine:values.outline;

import Karm.Gfx;
import Karm.Math;

import :values.calc2;
import :values.color;
import :values.length;
import :values.lineWidth;

using namespace Vaev::Literals;

namespace Vaev::Style {

// https://drafts.csswg.org/css-ui/#outline

export struct Outline {
    Pixel width = 3_px;
    Pixel offset = Pixel{0};
    Union<Keywords::Auto, Gfx::BorderStyle> style = Gfx::BorderStyle::NONE;
    Gfx::Color color = Gfx::BLUE500;
};

} // namespace Vaev::Style

