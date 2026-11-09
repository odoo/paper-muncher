export module Vaev.Engine:style.values.align;

import Karm.Core;

import :css.values.align;
import :style.values.keywords;
import :style.values.numeric;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-align-3/
export using Align = Css::Align;

// https://drafts.csswg.org/css-gaps-1/#propdef-row-gap
export using Gap = Union<Keywords::Normal, PxPercent>;

} // namespace Vaev::Style
