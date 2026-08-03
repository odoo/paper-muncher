export module Vaev.Engine:values.lineHeight;

import Karm.Core;

import :values.calc;
import :values.keywords;
import :values.length;
import :values.percent;
import :values.primitives;

using namespace Karm;

namespace Vaev {

// https://www.w3.org/TR/css-inline-3/#line-height-property
export using LineHeight = Union<Keywords::Normal, Number, Calc<PercentOr<Length>>>;

} // namespace Vaev
