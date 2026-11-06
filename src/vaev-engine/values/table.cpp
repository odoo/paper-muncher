export module Vaev.Engine:values.table;

import Karm.Core;

import :values.keywords;

using namespace Karm;

namespace Vaev {

export using TableLayout = Union<
    Keywords::Auto,
    Keywords::Fixed>;

export using CaptionSide = Union<
    Keywords::Top,
    Keywords::Bottom>;

export using BorderCollapse = Union<
    Keywords::Separate,
    Keywords::Collapse>;

} // namespace Vaev
