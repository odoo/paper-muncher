export module Vaev.Engine:style.values.breaks;

import Karm.Core;

import :css.values.breaks;

using namespace Karm;

// https://drafts.csswg.org/css-break-3/

namespace Vaev::Style {

// https://drafts.csswg.org/css-break/#break-between
export using BreakBetween = Css::BreakBetween;

// https://drafts.csswg.org/css-break/#break-within
export using BreakInside = Css::BreakInside;

} // namespace Vaev::Style
