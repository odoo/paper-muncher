module;

#include <karm/macros>

export module Vaev.Engine:values.sizing;

import Karm.Core;

import :css;
import :values.base;
import :values.calc;
import :values.calc2;
import :values.keywords;
import :values.length;
import :values.percent;
import :values.writing;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev {

// https://www.w3.org/TR/css-sizing-3/#box-sizing
export using BoxSizing = Union<Keywords::ContentBox, Keywords::BorderBox>;

// https://www.w3.org/TR/css-sizing-3/#propdef-width
// https://www.w3.org/TR/css-sizing-3/#propdef-height
export using Size = Union<Keywords::Auto, Ng::LengthPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;
export using MaxSize = Union<Keywords::None, Ng::LengthPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

export using ComputedSize = Union<Keywords::Auto, PixelPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;
export using ComputedMaxSize = Union<Keywords::None, PixelPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

export ComputedSize resolve(Size const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> ComputedSize { return resolve(v, ctx); });
}

export ComputedMaxSize resolve(MaxSize const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> ComputedMaxSize { return resolve(v, ctx); });
}

export Size toSpecified(ComputedSize const& value) {
    return value.visit([](auto const& v) -> Size { return toSpecified(v); });
}

export MaxSize toSpecified(ComputedMaxSize const& value) {
    return value.visit([](auto const& v) -> MaxSize { return toSpecified(v); });
}

export struct SizingProps {
    ComputedSize width = Keywords::AUTO, height = Keywords::AUTO;
    ComputedSize minWidth = Keywords::AUTO, minHeight = Keywords::AUTO;
    ComputedMaxSize maxWidth = Keywords::NONE, maxHeight = Keywords::NONE;

    ComputedSize& size(Axis axis) {
        return axis == Axis::HORIZONTAL ? width : height;
    }

    ComputedSize const& size(Axis axis) const {
        return axis == Axis::HORIZONTAL ? width : height;
    }

    ComputedSize& minSize(Axis axis) {
        return axis == Axis::HORIZONTAL ? minWidth : minHeight;
    }

    ComputedSize const& minSize(Axis axis) const {
        return axis == Axis::HORIZONTAL ? minWidth : minHeight;
    }

    ComputedMaxSize& maxSize(Axis axis) {
        return axis == Axis::HORIZONTAL ? maxWidth : maxHeight;
    }

    ComputedMaxSize const& maxSize(Axis axis) const {
        return axis == Axis::HORIZONTAL ? maxWidth : maxHeight;
    }

    void repr(Io::Emit& e) const {
        e("(sizing");
        e(" width={}", width);
        e(" height={}", height);
        e(" minWidth={}", minWidth);
        e(" minHeight={}", minHeight);
        e(" maxWidth={}", maxWidth);
        e(" maxHeight={}", maxHeight);
        e(")");
    }
};

} // namespace Vaev
