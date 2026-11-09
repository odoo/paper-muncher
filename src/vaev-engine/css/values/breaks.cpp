export module Vaev.Engine:css.values.breaks;

import Karm.Core;

import :css.values.keywords;

using namespace Karm;

namespace Vaev::Css {

// https://drafts.csswg.org/css-break/#break-between
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

// https://drafts.csswg.org/css-break/#break-within
export using BreakInside = Union<
    Keywords::Auto,
    Keywords::Avoid,
    Keywords::AvoidPage,
    Keywords::AvoidColumn,
    Keywords::AvoidRegion>;

} // namespace Vaev::Css
