export module Vaev.Engine:style.values.tables;

import Karm.Core;

import :css.values.tables;
import :style.values.numeric;

using namespace Karm;
using namespace Vaev::Literals;

// https://drafts.csswg.org/css-tables-3/

namespace Vaev::Style {

export using TableLayout = Css::TableLayout;

export using CaptionSide = Css::CaptionSide;

export using BorderCollapse = Css::BorderCollapse;

// https://drafts.csswg.org/css-tables-3/#propdef-border-spacing
export struct BorderSpacing {
    Px horizontal = 0_px;
    Px vertical = 0_px;
};

} // namespace Vaev::Style
