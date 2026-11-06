module;

#include <karm/macros>

export module Vaev.Engine:style.values.flex;

import Karm.Core;

import :css.values.flex;
import :style.values.calc2;
import :values.unitValue;

using namespace Karm;

namespace Vaev::Style {

export using FlexBasis = Union<
    Keywords::Auto,
    Keywords::Content,
    Keywords::MinContent,
    Keywords::MaxContent,
    Keywords::FitContent,
    PixelPercentage>;

export FlexBasis toComputed(Css::FlexBasis const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> FlexBasis { return toComputed(v, ctx); });
}

export Css::FlexBasis toSpecified(FlexBasis const& value) {
    return value.visit([](auto const& v) -> Css::FlexBasis { return toSpecified(v); });
}

} // namespace Vaev::Style
