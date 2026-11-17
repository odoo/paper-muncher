export module Vaev.Engine:style.values.tables;

import Karm.Core;

import :css.values.tables;
import :style.values.numeric;

using namespace Karm;
using namespace Vaev::Literals;

// https://drafts.csswg.org/css-tables-3/

namespace Vaev::Style {

// https://drafts.csswg.org/css-tables-3/#table-layout-property
export using TableLayout = Css::TableLayout;

// https://drafts.csswg.org/css-tables-3/#caption-side-property
export using CaptionSide = Css::CaptionSide;

// https://drafts.csswg.org/css-tables-3/#border-collapse-property
export using BorderCollapse = Css::BorderCollapse;

// https://drafts.csswg.org/css-tables-3/#propdef-border-spacing
export struct BorderSpacing {
    Px horizontal = 0_px;
    Px vertical = 0_px;
};

} // namespace Vaev::Style
