export module Vaev.Engine:style.values.position;

import Karm.Core;
import Karm.Math;

import :css.values.position;
import :style.values.numeric;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css2/#propdef-z-index
export using ZIndex = Css::ZIndex;

// https://www.w3.org/TR/CSS22/visuren.html#propdef-position
export using PositionScheme = Css::PositionScheme;

// https://drafts.csswg.org/css-position-3/#propdef-top
export using BoxInset = Union<Keywords::Auto, PxPercent>;

// https://drafts.csswg.org/css-position-3/#insets
export using BoxInsets = Math::Insets<BoxInset>;

} // namespace Vaev::Style
