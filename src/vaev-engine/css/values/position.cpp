export module Vaev.Engine:css.values.position;

import Karm.Core;

import :css.values.gcpm;
import :css.values.keywords;
import :css.values.numeric;
import :css.values.primitives;

using namespace Karm;

namespace Vaev::Css {

// https://drafts.csswg.org/css2/#propdef-z-index
export using ZIndex = Union<Keywords::Auto, Integer>;

// https://drafts.csswg.org/css-position-3/#propdef-position
export using PositionScheme = Union<Keywords::Static, Keywords::Relative, Keywords::Absolute, Keywords::Fixed, Keywords::Sticky, RunningPosition>;

// https://drafts.csswg.org/css-position-3/#propdef-top
export using BoxInset = Union<Keywords::Auto, LengthPercentage>;

} // namespace Vaev::Css
