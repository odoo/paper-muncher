module;

#include <karm/macros>

export module Vaev.Engine:css.values.sizing;

import Karm.Core;

import :css.values.calc;
import :css.values.keywords;
import :values.keywords;

using namespace Karm;

namespace Vaev::Css {

// https://www.w3.org/TR/css-sizing-3/#propdef-width
// https://www.w3.org/TR/css-sizing-3/#propdef-height
export using Size = Union<Keywords::Auto, LengthPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;
export using MaxSize = Union<Keywords::None, LengthPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

} // namespace Vaev::Css
