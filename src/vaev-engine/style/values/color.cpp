module;

#include <karm/macros>

export module Vaev.Engine:style.values.color;

import Karm.Core;
import Karm.Gfx;
import Karm.Logger;

import :css.values.color;
import :css.values.unitValue;

using namespace Karm;
using namespace Karm::Literals;

// https://www.w3.org/TR/css-color-4

namespace Vaev::Style {

export Gfx::Color resolve(Css::ColorMix const& cm, Gfx::Color currentColor);

export Gfx::Color resolve(Css::Color const& c, Gfx::Color currentColor) {
    return c.visit(
        [&](Gfx::Color const& srgb) {
            return srgb;
        },
        [&](Keywords::CurrentColor) {
            return currentColor;
        },
        [&](Css::SystemColor const& system) {
            return Css::SYSTEM_COLOR[static_cast<usize>(system)];
        },
        [&](Box<Css::ColorMix> const& mix) {
            return resolve(*mix, currentColor);
        }
    );
}

export Gfx::Color resolve(Css::ColorMix const& cm, Gfx::Color currentColor) {
    Gfx::Color lhsColor = resolve(cm.lhs.color, currentColor);
    f64 lhsPerc = cm.lhs.perc
                      ? cm.lhs.perc->value()
                      : 100 - cm.rhs.perc.unwrapOr(Css::PurePercentage{50}).value();

    Gfx::Color rhsColor = resolve(cm.rhs.color, currentColor);
    f64 rhsPerc = cm.rhs.perc
                      ? cm.rhs.perc->value()
                      : 100 - cm.lhs.perc.unwrapOr(Css::PurePercentage{50}).value();

    if (lhsPerc == rhsPerc and lhsPerc == 0) {
        logWarn("cannot mix colors when both have zero percentages");
        return Gfx::WHITE;
    }

    f64 rhsPercNorm = rhsPerc;

    if (lhsPerc + rhsPerc != 100)
        rhsPercNorm = rhsPercNorm / (lhsPerc + rhsPerc);
    else
        rhsPercNorm /= 100;

    Gfx::Color resColor = cm.colorSpace.interpolate(lhsColor, rhsColor, rhsPercNorm);
    if (lhsPerc + rhsPerc < 100)
        resColor = resColor.withOpacity((lhsPerc + rhsPerc) / 100.0);

    return resColor;
}

// https://drafts.csswg.org/css-color-4/#resolving-color-values
// currentcolor computes to itself, so it inherits as a keyword,
// every other color computes to an absolute color.
export using Color = Union<Gfx::Color, Keywords::CurrentColor>;

// NOTE: currentColor is only used to resolve color-mix() functions referencing currentcolor.
export Color toComputed(Css::Color const& value, Gfx::Color currentColor) {
    return value.visit(
        [](Gfx::Color const& srgb) -> Color {
            return srgb;
        },
        [](Keywords::CurrentColor) -> Color {
            return Keywords::CURRENT_COLOR;
        },
        [&](auto const&) -> Color {
            return resolve(value, currentColor);
        }
    );
}

export Css::Color toSpecified(Color const& value) {
    return value.visit([](auto const& v) -> Css::Color { return v; });
}

export Gfx::Color resolve(Color const& value, Gfx::Color currentColor) {
    return value.visit(
        [](Gfx::Color const& srgb) {
            return srgb;
        },
        [&](Keywords::CurrentColor) {
            return currentColor;
        }
    );
}

} // namespace Vaev::Style
