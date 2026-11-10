export module Vaev.Engine:style.values.color;

import Karm.Core;
import Karm.Gfx;
import Karm.Logger;

import :css.values.color;
import :css.values.numeric;
import :style.values.keywords;

using namespace Karm;
using namespace Karm::Literals;

// https://www.w3.org/TR/css-color-4

namespace Vaev::Style {

// MARK: Color Interpolation ---------------------------------------------------

template <typename Polar>
static void _preparePolar(Css::ColorSpace const& space, Polar& a, Polar& b) {
    auto delta = b.hue - a.hue;
    if (space.interpolation == Css::ColorSpace::SHORTER) {
        if (delta > 180.f)
            a.hue += 360.0f;
        else if (delta < -180.f)
            b.hue += 360.0f;
    } else if (space.interpolation == Css::ColorSpace::LONGER) {
        if (0.0f < delta and delta < 180.0f)
            a.hue += 360.0f;
        else if (-180.0f < delta and delta <= 0.0f)
            b.hue += 360.0f;
    } else if (space.interpolation == Css::ColorSpace::INCREASING) {
        if (delta < 0.0f)
            b.hue += 360.0f;
    } else {
        if (delta > 0.0f)
            a.hue += 360.0f;
    }
}

template <typename Polar>
static Polar _interpolatePolar(Css::ColorSpace const& space, Polar a, Polar b, f64 t) {
    _preparePolar(space, a, b);
    return a.lerpWith(b, t);
}

// https://drafts.csswg.org/css-color-4/#interpolation
// Not compliant regarding missing or carried values
export Gfx::Color interpolate(Css::ColorSpace const& space, Gfx::Color a, Gfx::Color b, f64 t) {
    a = a.premultiply();
    b = b.premultiply();

    if (space.type == Css::ColorSpace::RECTANGULAR) {
        switch (space.rectangular) {
        case Css::ColorSpace::SRGB:
            return a.lerpWith(b, t).unpremultiply();

        default:
            logWarn("interpolation method is not supported: {}", space.rectangular);
            return Gfx::FUCHSIA;
        }
    } else if (space.type == Css::ColorSpace::POLAR) {
        switch (space.polar) {
        case Css::ColorSpace::HSL:
            return Gfx::hslToRgb(
                       _interpolatePolar<Gfx::Hsl>(
                           space,
                           Gfx::rgbToHsl(a),
                           Gfx::rgbToHsl(b),
                           t
                       )
            )
                .unpremultiply();

        default:
            logWarn("interpolation method is not supported: {}", space.polar);
            return Gfx::FUCHSIA;
        }
    }

    return Gfx::WHITE;
}

// https://drafts.csswg.org/css-color-4/#css-system-colors
constexpr Array<Gfx::Color, static_cast<usize>(Css::SystemColor::_LEN)> _SYSTEM_COLORS = {
#define COLOR(NAME, _, VALUE) Gfx::Color::fromHex(VALUE),
#include "../../css/values/defs/system-colors.inc"

#undef COLOR
};

// MARK: Color -----------------------------------------------------------------
// https://drafts.csswg.org/css-color-4/#resolving-color-values

// currentcolor computes to itself, so it inherits as a keyword,
// every other color computes to a concrete color.
export using Color = Union<Gfx::Color, Keywords::CurrentColor>;

Gfx::Color _mix(Css::ColorMix const& cm, Gfx::Color currentColor);

// NOTE: currentColor is only used to resolve color-mix() functions referencing currentcolor.
export Color toComputed(Css::Color const& value, Gfx::Color currentColor) {
    return value.visit(
        [](Gfx::Color const& srgb) -> Color {
            return srgb;
        },
        // https://drafts.csswg.org/css-color-4/#resolving-other-colors
        // The currentcolor keyword computes to itself.
        [](Keywords::CurrentColor) -> Color {
            return Keywords::CURRENT_COLOR;
        },
        // For system colors, the computed and used value is the corresponding color in its color space.
        [](Css::SystemColor const& system) -> Color {
            return _SYSTEM_COLORS[static_cast<usize>(system)];
        },
        // https://drafts.csswg.org/css-color-5/#resolving-mix
        // FIXME: A color-mix() referencing currentcolor should compute to the function itself,
        //        to preserve inheritance; we mix eagerly with the current color instead.
        [&](Box<Css::ColorMix> const& mix) -> Color {
            return _mix(*mix, currentColor);
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

// Specified → used, through the computed value.
export Gfx::Color toUsed(Css::Color const& value, Gfx::Color currentColor) {
    return resolve(toComputed(value, currentColor), currentColor);
}

Gfx::Color _mix(Css::ColorMix const& cm, Gfx::Color currentColor) {
    Gfx::Color lhsColor = toUsed(cm.lhs.color, currentColor);
    f64 lhsPerc = cm.lhs.perc
                      ? cm.lhs.perc->value()
                      : 100 - cm.rhs.perc.unwrapOr(Css::percent(50)).value();

    Gfx::Color rhsColor = toUsed(cm.rhs.color, currentColor);
    f64 rhsPerc = cm.rhs.perc
                      ? cm.rhs.perc->value()
                      : 100 - cm.lhs.perc.unwrapOr(Css::percent(50)).value();

    if (lhsPerc == rhsPerc and lhsPerc == 0) {
        logWarn("cannot mix colors when both have zero percentages");
        return Gfx::WHITE;
    }

    f64 rhsPercNorm = rhsPerc;

    if (lhsPerc + rhsPerc != 100)
        rhsPercNorm = rhsPercNorm / (lhsPerc + rhsPerc);
    else
        rhsPercNorm /= 100;

    Gfx::Color resColor = interpolate(cm.colorSpace, lhsColor, rhsColor, rhsPercNorm);
    if (lhsPerc + rhsPerc < 100)
        resColor = resColor.withOpacity((lhsPerc + rhsPerc) / 100.0);

    return resColor;
}

} // namespace Vaev::Style
