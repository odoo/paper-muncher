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

namespace Vaev::Css {

// https://www.w3.org/TR/css-sizing-3/#box-sizing
export using BoxSizing = Union<Keywords::ContentBox, Keywords::BorderBox>;

// https://www.w3.org/TR/css-sizing-3/#propdef-width
// https://www.w3.org/TR/css-sizing-3/#propdef-height
export using Size = Union<Keywords::Auto, LengthPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;
export using MaxSize = Union<Keywords::None, LengthPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

} // namespace Vaev::Css

namespace Vaev::Style {

export using Size = Union<Keywords::Auto, PixelPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

export using MaxSize = Union<Keywords::None, PixelPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

export Style::Size toComputed(Css::Size const& value, Css::LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> Style::Size { return toComputed(v, ctx); });
}

export Style::MaxSize toComputed(Css::MaxSize const& value, Css::LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> Style::MaxSize { return toComputed(v, ctx); });
}

export Css::Size toSpecified(Style::Size const& value) {
    return value.visit([](auto const& v) -> Css::Size { return toSpecified(v); });
}

export Css::MaxSize toSpecified(Style::MaxSize const& value) {
    return value.visit([](auto const& v) -> Css::MaxSize { return toSpecified(v); });
}

export struct Sizing {
    Style::Size width = Keywords::AUTO, height = Keywords::AUTO;
    Style::Size minWidth = Keywords::AUTO, minHeight = Keywords::AUTO;
    Style::MaxSize maxWidth = Keywords::NONE, maxHeight = Keywords::NONE;

    Style::Size& size(Css::Axis axis) {
        return axis == Css::Axis::HORIZONTAL ? width : height;
    }

    Style::Size const& size(Css::Axis axis) const {
        return axis == Css::Axis::HORIZONTAL ? width : height;
    }

    Style::Size& minSize(Css::Axis axis) {
        return axis == Css::Axis::HORIZONTAL ? minWidth : minHeight;
    }

    Style::Size const& minSize(Css::Axis axis) const {
        return axis == Css::Axis::HORIZONTAL ? minWidth : minHeight;
    }

    Style::MaxSize& maxSize(Css::Axis axis) {
        return axis == Css::Axis::HORIZONTAL ? maxWidth : maxHeight;
    }

    Style::MaxSize const& maxSize(Css::Axis axis) const {
        return axis == Css::Axis::HORIZONTAL ? maxWidth : maxHeight;
    }
};

} // namespace Vaev::Style

