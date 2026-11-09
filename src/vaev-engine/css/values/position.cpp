export module Vaev.Engine:css.values.position;

import Karm.Core;

import :css.values.gcpm;
import :css.values.keywords;
import :css.values.primitives;

using namespace Karm;

// https://drafts.csswg.org/css-position-3/

namespace Vaev::Css {

export using ZIndex = Union<Keywords::Auto, Integer>;

// https://www.w3.org/TR/CSS22/visuren.html#propdef-position
export using PositionScheme = Union<Keywords::Static, Keywords::Relative, Keywords::Absolute, Keywords::Fixed, Keywords::Sticky, RunningPosition>;

} // namespace Vaev::Css
