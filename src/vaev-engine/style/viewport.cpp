export module Vaev.Engine:style.viewport;

import :values.length;
import :values.resolution;

namespace Vaev::Style {

export struct Viewport {
    Css::PureResolution dpi = Css::PureResolution::fromDpi(96);
    // https://drafts.csswg.org/css-values/#small-viewport-size
    RectAu small;
    // https://drafts.csswg.org/css-values/#large-viewport-size
    RectAu large = small;
    // https://drafts.csswg.org/css-values/#dynamic-viewport-size
    RectAu dynamic = small;
};

} // namespace Vaev::Style