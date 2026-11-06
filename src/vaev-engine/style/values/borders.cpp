export module Vaev.Engine:style.values.borders;

import Karm.Core;
import Karm.Math;

import :css.values.calc2;
import :style.values.calc2;
import :style.values.length;
import :values.unitValue;

using namespace Vaev::Literals;

namespace Vaev::Style {

export Math::Radii<PixelPercentage> toComputed(Math::Radii<Css::LengthPercentage> const& value, LengthContext auto const& ctx) {
    return {
        toComputed(value.a, ctx),
        toComputed(value.b, ctx),
        toComputed(value.c, ctx),
        toComputed(value.d, ctx),
        toComputed(value.e, ctx),
        toComputed(value.f, ctx),
        toComputed(value.g, ctx),
        toComputed(value.h, ctx),
    };
}

export Math::Radii<Css::LengthPercentage> toSpecified(Math::Radii<PixelPercentage> const& value) {
    return {
        toSpecified(value.a),
        toSpecified(value.b),
        toSpecified(value.c),
        toSpecified(value.d),
        toSpecified(value.e),
        toSpecified(value.f),
        toSpecified(value.g),
        toSpecified(value.h),
    };
}

} // namespace Vaev::Style
