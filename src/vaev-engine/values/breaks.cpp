export module Vaev.Engine:values.breaks;

import Karm.Core;

import :values.keywords;

using namespace Karm;

namespace Vaev {

export using BreakBetween = Union<
    Keywords::Auto,
    Keywords::Avoid,
    Keywords::AvoidPage,
    Keywords::Page,
    Keywords::Left,
    Keywords::Right,
    Keywords::Recto,
    Keywords::Verso,
    Keywords::AvoidColumn,
    Keywords::Column,
    Keywords::AvoidRegion,
    Keywords::Region>;

export using BreakInside = Union<
    Keywords::Auto,
    Keywords::Avoid,
    Keywords::AvoidPage,
    Keywords::AvoidColumn,
    Keywords::AvoidRegion>;

} // namespace Vaev
