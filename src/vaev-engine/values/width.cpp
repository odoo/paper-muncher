module;

#include <karm-base/union.h>

export module Vaev.Engine:values.width;

import :values.calc;
import :values.keywords;
import :values.length;
import :values.percent;

namespace Vaev {

export using Width = FlatUnion<
    Keywords::Auto,
    CalcValue<PercentOr<Length>>>;

} // namespace Vaev
