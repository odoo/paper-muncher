export module Vaev.Engine:css.values.tables;

import Karm.Core;

import :css.values.keywords;

using namespace Karm;

// https://drafts.csswg.org/css-tables-3/

namespace Vaev::Css {

export using TableLayout = Union<
    Keywords::Auto,
    Keywords::Fixed>;

export using CaptionSide = Union<
    Keywords::Top,
    Keywords::Bottom>;

export using BorderCollapse = Union<
    Keywords::Separate,
    Keywords::Collapse>;

} // namespace Vaev::Css
