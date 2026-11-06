export module Vaev.Engine:style.viewport;

import Karm.Math;

import :css.values.resolution;
import :style.values.length;

using namespace Karm;

namespace Vaev::Style {

export struct Viewport {
    Css::PureResolution dpi = Css::PureResolution::fromDpi(96);
    // https://drafts.csswg.org/css-values/#small-viewport-size
    Math::Vec2<Pixel> small;
    // https://drafts.csswg.org/css-values/#large-viewport-size
    Math::Vec2<Pixel> large = small;
    // https://drafts.csswg.org/css-values/#dynamic-viewport-size
    Math::Vec2<Pixel> dynamic = small;
};

} // namespace Vaev::Style
