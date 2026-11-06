module;

#include <karm/macros>

export module Vaev.Engine:style.values.unitValue;

import Karm.Core;

import :values.writing;

using namespace Karm;

namespace Vaev::Style {

export struct RelativeLengthContextData {
    // current font metrics
    f64 fontSize;
    f64 xHeight;
    f64 capHeight;
    f64 zeroAdvance;
    f64 lineHeight;

    // relative font metrics
    f64 rootFontSize;
    f64 rootXHeight;
    f64 rootCapHeight;
    f64 rootZeroAdvance;
    f64 rootLineHeight;

    f64 viewportSmallWidth;
    f64 viewportSmallHeight;

    f64 viewportLargeWidth;
    f64 viewportLargeHeight;

    f64 viewportDynamicWidth;
    f64 viewportDynamicHeight;

    Axis boxAxis = Axis::HORIZONTAL;
};

} // namespace Vaev::Style
