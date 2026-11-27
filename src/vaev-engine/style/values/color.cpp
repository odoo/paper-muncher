export module Vaev.Engine:style.values.color;

import Karm.Core;
import Karm.Gfx;
import Karm.Logger;

import :css.values.color;
import :css.values.numeric;
import :style.values.keywords;
import :style.values.numeric;
import :style.values.base;

using namespace Karm;
using namespace Karm::Literals;

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
// NOSPEC: Not compliant regarding missing or carried values
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
constexpr Array _SYSTEM_COLORS = {
#define COLOR(NAME, _, VALUE) Gfx::Color::fromHex(VALUE),
#include "../../css/values/defs/system-colors.inc"

#undef COLOR
};

// MARK: Gamut Mapping ---------------------------------------------------------

static Gfx::Srgb _oklchToSrgb(Gfx::Oklch oklch) {
    return Gfx::linearToSrgb(Gfx::xyzToLinearSrgb(Gfx::oklabToXyz(Gfx::oklchToOklab(oklch))));
}

// https://drafts.csswg.org/css-color-4/#pseudo-binsearch
// To Binary Search Gamut Map with Local MINDE a color origin in color space origin color space to be in gamut of a
// destination color space destination:
static Gfx::Srgb _gamutMapToSrgb(Gfx::Oklch originOklch) {
    // NOTE: The destination is sRGB, which has gamut limits, so step 1 does not apply. The origin is already in OkLCh,
    //       so step 2 does not apply either.

    // 3. if the Lightness of origin_OkLCh is greater than or equal to 100%, convert `oklab(1 0 0 / origin.alpha)`
    //    to destination and return it as the gamut mapped color
    if (originOklch.l >= 1)
        return _oklchToSrgb({1, 0, NONE});

    // 4. if the Lightness of origin_OkLCh is less than or equal to 0%, convert `oklab(0 0 0 / origin.alpha)` to
    //    destination and return it as the gamut mapped color
    if (originOklch.l <= 0)
        return _oklchToSrgb({0, 0, NONE});

    // 5. let inGamut(color) be a function which returns true if, when passed a color, that color is inside the
    //    gamut of destination. For HSL and HWB, it returns true if the color is inside the gamut of sRGB.
    auto inGamut = [](Gfx::Oklch color) {
        auto srgb = _oklchToSrgb(color);
        return srgb.r >= 0 and srgb.r <= 1 and
               srgb.g >= 0 and srgb.g <= 1 and
               srgb.b >= 0 and srgb.b <= 1;
    };

    // 6. if inGamut(origin_OkLCh) is true, convert origin_OkLCh to destination and return it as the gamut mapped
    //    color
    if (inGamut(originOklch))
        return _oklchToSrgb(originOklch);

    // 7. otherwise, let delta(one, two) be a function which returns the deltaEOK of color one compared to color two
    auto delta = [](Gfx::Srgb one, Gfx::Oklch two) {
        auto oneOklab = Gfx::xyzToOklab(Gfx::linearSrgbToXyz(Gfx::srgbToLinear(one)));
        return Gfx::deltaEOK(oneOklab, Gfx::oklchToOklab(two));
    };

    // 8. let JND be 0.02
    f64 jnd = 0.02;

    // 9. let epsilon be 0.0001
    f64 epsilon = 0.0001;

    // 10. let clip(color) be a function which converts color to destination, clamps each component to the bounds of
    //     the reference range for that component and returns the result
    auto clip = [](Gfx::Oklch color) -> Gfx::Srgb {
        auto srgb = _oklchToSrgb(color);
        return {clamp01(srgb.r), clamp01(srgb.g), clamp01(srgb.b)};
    };

    // 11. set current to origin_OkLCh
    auto current = originOklch;

    // 12. set clipped to clip(current)
    auto clipped = clip(current);

    // 13. set E to delta(clipped, current)
    f64 e = delta(clipped, current);

    // 14. if E < JND
    //     14.1. return clipped as the gamut mapped color
    if (e < jnd)
        return clipped;

    // 15. set min to zero
    f64 min = 0;

    // 16. set max to the OkLCh chroma of origin_OkLCh
    f64 max = originOklch.c;

    // 17. let min_inGamut be a boolean that represents when min is still in gamut, and set it to true
    bool minInGamut = true;

    // 18. while (max - min is greater than epsilon) repeat the following steps
    while (max - min > epsilon) {
        // 18.1. set chroma to (min + max) /2
        f64 chroma = (min + max) / 2;

        // 18.2. set the chroma component of current to chroma
        current.c = chroma;

        // 18.3. if min_inGamut is true and also if inGamut(current) is true, set min to chroma and continue to
        //       repeat these steps
        if (minInGamut and inGamut(current)) {
            min = chroma;
            continue;
        }

        // 18.4. otherwise, carry out these steps:
        //       18.4.1. set clipped to clip(current)
        clipped = clip(current);

        //       18.4.2. set E to delta(clipped, current)
        e = delta(clipped, current);

        //       18.4.3. if E < JND
        if (e < jnd) {
            // 18.4.3.1. if (JND - E < epsilon) return clipped as the gamut mapped color
            if (jnd - e < epsilon)
                return clipped;

            // 18.4.3.2. otherwise,
            //           18.4.3.2.1. set min_inGamut to false
            minInGamut = false;
            //           18.4.3.2.2. set min to chroma
            min = chroma;
        } else {
            // 18.4.4. otherwise, set max to chroma and continue to repeat these steps
            max = chroma;
        }
    }

    // 19. return clipped as the gamut mapped color
    return clipped;
}

// MARK: Color -----------------------------------------------------------------
// https://drafts.csswg.org/css-color-4/#resolving-color-values

// https://drafts.csswg.org/css-color-4/#typedef-color
export using Color = Union<Gfx::Color, Css::Oklch, Keywords::CurrentColor>;

Gfx::Color _mix(Css::ColorMix const& cm, Gfx::Color currentColor);

export template <>
struct ValueComputer<Css::Color> {
    static Color compute(Css::Color const& value, Gfx::Color currentColor) {
        return value.visit(
            [](Gfx::Color const& srgb) -> Color {
                return srgb;
            },
            // https://drafts.csswg.org/css-color-4/#resolving-oklab-oklch-values
            // The declared, computed and used value is the corresponding Oklab or OkLCh color (after clamping of L, C
            // and H) paired with the specified alpha component
            [](Css::Oklch const& oklch) -> Color {
                return oklch;
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

    static Css::Color specify(Color const& value) {
        return value.visit([](auto const& v) -> Css::Color {
            return v;
        });
    }
};

export Gfx::Color resolve(Color const& value, Gfx::Color currentColor) {
    return value.visit(
        [](Gfx::Color const& srgb) {
            return srgb;
        },
        // https://drafts.csswg.org/css-color-4/#css-gamut-mapping
        [](Css::Oklch const& oklch) {
            // https://drafts.csswg.org/css-color-4/#missing
            // For all other purposes, a missing component behaves as a zero value
            auto srgb = _gamutMapToSrgb({oklch.lightness.unwrapOr(0), oklch.chroma.unwrapOr(0), oklch.hue});

            // https://drafts.csswg.org/css-color-4/#rgb-functions
            return Gfx::Color::fromRgba(
                Math::round(srgb.r * 255),
                Math::round(srgb.g * 255),
                Math::round(srgb.b * 255),
                Math::round(oklch.alpha.unwrapOr(0) * 255)
            );
        },
        [&](Keywords::CurrentColor) {
            return currentColor;
        }
    );
}

export Gfx::Color toUsed(Css::Color const& value, Gfx::Color currentColor) {
    return resolve(computeValue(value, currentColor), currentColor);
}

Gfx::Color _mix(Css::ColorMix const& cm, Gfx::Color currentColor) {
    Gfx::Color lhsColor = toUsed(cm.lhs.color, currentColor);
    f64 lhsPerc = cm.lhs.perc
                      ? cm.lhs.perc->value()
                      : 100 - (cm.rhs.perc ? cm.rhs.perc->value() : 50);

    Gfx::Color rhsColor = toUsed(cm.rhs.color, currentColor);
    f64 rhsPerc = cm.rhs.perc
                      ? cm.rhs.perc->value()
                      : 100 - (cm.lhs.perc ? cm.lhs.perc->value() : 50);

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

// MARK: Opacity ---------------------------------------------------------------
// https://drafts.csswg.org/css-color-4/#typedef-opacity-opacity-value

export template <>
struct ValueComputer<Css::OpacityValue> {
    static f64 compute(Css::OpacityValue const& value) {
        auto number = value.visit(
            [](Css::Number const& n) {
                return computeValue(n);
            },
            [](Css::Percentage const& percent) {
                return computeValue(percent, 1);
            }
        );

        return clamp(number, 0., 1.);
    }

    static Css::OpacityValue specify(f64 value) {
        return specifyValue<Css::Number>(value);
    }
};

} // namespace Vaev::Style
