export module Vaev.Engine:css.values.lineHeight;

import Karm.Core;

import :css.values.calc;
import :css.values.keywords;
import :values.keywords;

using namespace Karm;

namespace Vaev::Css {

// https://www.w3.org/TR/css-inline-3/#line-height-property
export using LineHeight = Union<Keywords::Normal, Number, LengthPercentage>;

} // namespace Vaev::Css
