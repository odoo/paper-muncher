module;

#include <karm/macros>

export module Vaev.Engine:values.sizing;

import Karm.Core;

import :css;
import :values.base;
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
export using Size = Union<Keywords::Auto, LengthPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;
export using MaxSize = Union<Keywords::None, LengthPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

namespace Style {

export using Size = Union<Keywords::Auto, PixelPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

export using MaxSize = Union<Keywords::None, PixelPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

} // namespace Style

export Style::Size resolve(Size const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> Style::Size { return resolve(v, ctx); });
}

export Style::MaxSize resolve(MaxSize const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> Style::MaxSize { return resolve(v, ctx); });
}

export Size toSpecified(Style::Size const& value) {
    return value.visit([](auto const& v) -> Size { return toSpecified(v); });
}

export MaxSize toSpecified(Style::MaxSize const& value) {
    return value.visit([](auto const& v) -> MaxSize { return toSpecified(v); });
}

namespace Style {

export struct Sizing {
    Style::Size width = Keywords::AUTO, height = Keywords::AUTO;
    Style::Size minWidth = Keywords::AUTO, minHeight = Keywords::AUTO;
    Style::MaxSize maxWidth = Keywords::NONE, maxHeight = Keywords::NONE;

    Style::Size& size(Axis axis) {
        return axis == Axis::HORIZONTAL ? width : height;
    }

    Style::Size const& size(Axis axis) const {
        return axis == Axis::HORIZONTAL ? width : height;
    }

    Style::Size& minSize(Axis axis) {
        return axis == Axis::HORIZONTAL ? minWidth : minHeight;
    }

    Style::Size const& minSize(Axis axis) const {
        return axis == Axis::HORIZONTAL ? minWidth : minHeight;
    }

    Style::MaxSize& maxSize(Axis axis) {
        return axis == Axis::HORIZONTAL ? maxWidth : maxHeight;
    }

    Style::MaxSize const& maxSize(Axis axis) const {
        return axis == Axis::HORIZONTAL ? maxWidth : maxHeight;
    }
};

} // namespace Style
} // namespace Vaev
