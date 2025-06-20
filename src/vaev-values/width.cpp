module;

#include <karm-base/union.h>

export module Vaev.Values:width;

import :calc;
import :keywords;
import :length;
import :percent;

namespace Vaev {

export using Width = FlatUnion<
    Keywords::Auto,
    CalcValue<PercentOr<Length>>>;

} // namespace Vaev
