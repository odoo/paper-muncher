export module Vaev.Engine:values.counter;

import Karm.Core;

import :values.keywords;

namespace Vaev {

export using CounterSymbolsType = Union<
    Keywords::Cyclic,
    Keywords::Numeric,
    Keywords::Alphabetic,
    Keywords::Fixed>;

} // namespace Vaev
