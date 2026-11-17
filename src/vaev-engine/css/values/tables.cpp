export module Vaev.Engine:css.values.tables;

import Karm.Core;

import :css.values.keywords;

using namespace Karm;

namespace Vaev::Css {

// https://drafts.csswg.org/css-tables-3/#table-layout-property
export using TableLayout = Union<
    Keywords::Auto,
    Keywords::Fixed>;

// https://drafts.csswg.org/css-tables-3/#caption-side-property
export using CaptionSide = Union<
    Keywords::Top,
    Keywords::Bottom>;

// https://drafts.csswg.org/css-tables-3/#border-collapse-property
export using BorderCollapse = Union<
    Keywords::Separate,
    Keywords::Collapse>;

} // namespace Vaev::Css
