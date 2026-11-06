export module Vaev.Engine:css.values.baseline;

import Karm.Core;

import :css.values.calc;
import :css.values.keywords;
import :values.keywords;

using namespace Karm;

namespace Vaev::Css {

// https://drafts.csswg.org/css-inline/#propdef-baseline-shift
export using BaselineShift = Union<
    LengthPercentage,
    Keywords::Sub,
    Keywords::Super,
    Keywords::Top,
    Keywords::Center,
    Keywords::Bottom>;

} // namespace Vaev::Css
