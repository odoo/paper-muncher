module;

#include <karm/macros>

export module Vaev.Engine:style.values.svg;

import Karm.Core;
import Karm.Gfx;

import :css.values.color;
import :css.values.svg;
import :style.values.color;

using namespace Karm::Literals;
using namespace Karm::Math::Literals;

namespace Vaev::Style {

export using SvgPaint = Opt<Color>;

export SvgPaint toComputed(Css::SvgPaint const& value, Gfx::Color currentColor) {
    return value.map([&](Css::Color const& color) {
        return toComputed(color, currentColor);
    });
}

export Css::SvgPaint toSpecified(SvgPaint const& value) {
    return value.map([](Color const& color) {
        return toSpecified(color);
    });
}

export Opt<Gfx::Color> resolve(SvgPaint const& value, Gfx::Color currentColor) {
    return value.map([&](Color const& color) {
        return resolve(color, currentColor);
    });
}

} // namespace Vaev::Style
