module;

#include <karm/macros>

export module Vaev.Engine:values.table;

import Karm.Core;
import Karm.Math;

import :css;
import :values.base;
import :values.calc2;
import :values.length;
import :values.percent;
import :values.unitValue;
import :values.keywords;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev::Css {

// MARK: Table Layout ----------------------------------------------------------
// https://www.w3.org/TR/CSS21/tables.html#propdef-table-layout

export using TableLayout = Union<
    Keywords::Auto,
    Keywords::Fixed>;

// MARK: Caption Side ----------------------------------------------------------
// https://www.w3.org/TR/CSS21/tables.html#caption-position

export using CaptionSide = Union<
    Keywords::Top,
    Keywords::Bottom>;

// MARK: Border Collapse -------------------------------------------------------
// https://www.w3.org/TR/CSS22/tables.html#propdef-border-collapse

export using BorderCollapse = Union<
    Keywords::Separate,
    Keywords::Collapse>;

} // namespace Vaev::Css

namespace Vaev::Style {

export struct BorderSpacing {
    Pixel horizontal = Pixel{0};
    Pixel vertical = Pixel{0};
};

} // namespace Vaev::Style
