module;

#include <karm/macros>

export module Vaev.Engine:style.values.sizing;

import Karm.Core;

import :css.values.sizing;
import :style.values.numeric;
import :values.unitValue;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev::Style {

export using Size = Union<Keywords::Auto, PixelPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

export using MaxSize = Union<Keywords::None, PixelPercentage, Keywords::Stretch, Keywords::MinContent, Keywords::MaxContent, Keywords::FitContent>;

export Size toComputed(Css::Size const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> Size { return toComputed(v, ctx); });
}

export MaxSize toComputed(Css::MaxSize const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> MaxSize { return toComputed(v, ctx); });
}

export Css::Size toSpecified(Size const& value) {
    return value.visit([](auto const& v) -> Css::Size { return toSpecified(v); });
}

export Css::MaxSize toSpecified(MaxSize const& value) {
    return value.visit([](auto const& v) -> Css::MaxSize { return toSpecified(v); });
}

} // namespace Vaev::Style
