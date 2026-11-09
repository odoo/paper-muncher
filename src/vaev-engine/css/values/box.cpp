export module Vaev.Engine:css.values.box;

import Karm.Core;

import :css.values.keywords;
import :css.values.numeric;

using namespace Karm;

namespace Vaev::Css {

// https://drafts.csswg.org/css-box-4/#propdef-margin-top
export using Margin = Union<Keywords::Auto, LengthPercentage>;

} // namespace Vaev::Css
