export module Vaev.Engine:style.values.align;

import Karm.Core;

import :css.values.align;
import :style.values.keywords;
import :style.values.numeric;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-align-3/
// Computed value: specified keyword(s)
export using Align = Css::Align;

export using Gap = Union<
    Keywords::Normal,
    PxPercent>;

} // namespace Vaev::Style
