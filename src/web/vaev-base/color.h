#pragma once

#include <karm-gfx/colors.h>
#include <karm-io/emit.h>

#include "karm-logger/logger.h"

// https://www.w3.org/TR/css-color-4

namespace Vaev {

// MARK: Color Interpolation ---------------------------------------------------

// https://www.w3.org/TR/css-color-4/#typedef-color-space
struct ColorSpace {
    enum struct _Type {
        POLAR,
        RECTANGULAR,

        _LEN0,
    };

    using enum _Type;

    // https://www.w3.org/TR/css-color-4/#typedef-rectangular-color-space
    enum struct _Rectangular {
        SRGB,
        SRGB_LINEAR,
        DISPLAY_P3,
        A98_RGB,
        PROPHOTO_RGB,
        REC2020,
        LAB,
        OKLAB,
        XYZ,
        XYZ_D50,
        XYZ_D65,

        _LEN1,

    };

    using enum _Rectangular;

    // https://www.w3.org/TR/css-color-4/#typedef-hue-interpolation-method
    enum struct _Interpolation {
        SHORTER,
        LONGER,
        INCREASING,
        DECREASING,

        _LEN2,
    };

    using enum _Interpolation;

    // https://www.w3.org/TR/css-color-4/#typedef-polar-color-space
    enum struct _Polar {
        HSL,
        HWB,
        LCH,
        OKLCH,

        _LEN3,
    };

    using enum _Polar;

    _Type type;

    union {
        _Rectangular rectangular;

        struct {
            _Polar polar;
            _Interpolation interpolation;
        };
    };

    constexpr ColorSpace(_Rectangular rectangular)
        : type(_Type::RECTANGULAR), rectangular(rectangular) {
    }

    constexpr ColorSpace(_Polar polar, _Interpolation interpolation)
        : type(_Type::POLAR), polar(polar), interpolation(interpolation) {
    }

    constexpr ColorSpace()
        : type(_Type::RECTANGULAR), rectangular(_Rectangular::SRGB) {
    }

    void repr(Io::Emit& e) const {
        if (type == ColorSpace::RECTANGULAR) {
            e("{}", rectangular);
        } else {
            e("{} {}", polar, interpolation);
        }
    }

    static ColorSpace fromStr(Str name) {
        if (name == "lch")
            return ColorSpace(HSL, SHORTER);

        return ColorSpace();
    }

    template <typename Polar>
    void _preparePolar(Polar& a, Polar& b) {
        auto delta = b.hue - a.hue;
        if (interpolation == SHORTER) {
            if (delta > 180.f)
                a.hue += 360.0f;
            else if (delta < -180.f)
                b.hue += 360.0f;
        } else if (interpolation == LONGER) {
            if (0.0f < delta and delta < 180.0f)
                a.hue += 360.0f;
            else if (-180.0f < delta and delta <= 0.0f)
                b.hue += 360.0f;
        } else if (interpolation == INCREASING) {
            if (delta < 0.0f)
                b.hue += 360.0f;
        } else {
            if (delta > 0.0f)
                a.hue += 360.0f;
        }
    }

    template <typename Polar>
    Polar _interpolatePolar(Polar a, Polar b, f64 t) {
        _preparePolar(a, b);
        return a.lerpWith(b, t);
    }

    // https://drafts.csswg.org/css-color-4/#interpolation
    // Not compliant regarding missing or carried values
    Gfx::Color interpolate(Gfx::Color a, Gfx::Color b, f64 t) {
        a = a.premultiply();
        b = b.premultiply();

        if (type == _Type::RECTANGULAR) {
            switch (rectangular) {
            case _Rectangular::SRGB:
                return a.lerpWith(b, t).unpremultiply();
            default:
                logWarn("interpolation method is not supported: {}", rectangular);
                return Gfx::WHITE;
            }
        } else if (type == _Type::POLAR) {
            switch (polar) {
            case _Polar::HSL:
                return Gfx::hslToRgb(
                           _interpolatePolar<Gfx::Hsl>(Gfx::rgbToHsl(a), Gfx::rgbToHsl(b), t)
                )
                    .unpremultiply();
            default:
                logWarn("interpolation method is not supported: {}", polar);
                return Gfx::WHITE;
            }
        }

        return Gfx::WHITE;
    }
};

// MARK: Color -----------------------------------------------------------------

enum struct SystemColor : u8 {
#define COLOR(NAME, ...) NAME,
#include "defs/system-colors.inc"
#undef COLOR

    _LEN
};

struct Color {
    enum struct Type {
        SRGB,
        SYSTEM,
        CURRENT, // currentColor
        MIX,
    };

    using enum Type;

    Type type;

    struct ColorMix {
        ColorSpace colorSpace;
        Box<Color> colorA, colorB;
        Opt<i8> percA, percB;

        void repr(Io::Emit& e) const {
            e("{} {}, {} {}", colorA, percA, colorB, percB);
        }
    };

    using Value = Union<None, Gfx::Color, SystemColor, ColorMix>;
    Value _color;

    Color() : type(Type::SRGB), _color(Gfx::Color{Gfx::ALPHA}) {}

    static Value defaultColorValue(Type type) {
        switch (type) {
        case Type::SRGB:
            return Gfx::Color{Gfx::ALPHA};
        case Type::SYSTEM:
            return SystemColor{};
        case Type::CURRENT:
            return NONE;
        case Type::MIX:
            panic("No default color value for color-mix type");
        }
    }

    Color(Type type) : type(type), _color(defaultColorValue(type)) {}

    Color(Gfx::Color srgb) : type(Type::SRGB), _color(srgb) {}

    Color(SystemColor system) : type(Type::SYSTEM), _color(system) {}

    Color(ColorSpace colorSpace, Color&& colorA, Color&& colorB, Opt<i8> percA, Opt<i8> percB)
        : type(Type::MIX),
          _color(ColorMix{
              .colorSpace = colorSpace,
              .colorA = std::move(colorA),
              .colorB = std::move(colorB),
              .percA = percA,
              .percB = percB
          }) {}

    void repr(Io::Emit& e) const {
        switch (type) {
        case Type::SRGB: {
            auto srgb = _color.unwrap<Gfx::Color>();
            e("#{02x}{02x}{02x}{02x}", srgb.red, srgb.green, srgb.blue, srgb.alpha);
            break;
        }

        case Type::SYSTEM: {
            auto system = _color.unwrap<SystemColor>();
            e("{}", system);
            break;
        }

        case Type::CURRENT:
            e("currentColor");
            break;

        case Type::MIX: {
            auto colorMix = _color.unwrap<ColorMix>();
            e("color-mix(");
            colorMix.repr(e);
            e(")");
        }
        }
    }
};

inline constexpr Gfx::Color TRANSPARENT = Gfx::Color::fromRgba(0, 0, 0, 0);

#define COLOR(NAME, _, VALUE) \
    inline constexpr Gfx::Color NAME = Gfx::Color::fromHex(VALUE);
#include "defs/colors.inc"
#undef COLOR

Opt<Color> parseNamedColor(Str name);

Opt<SystemColor> parseSystemColor(Str name);

Gfx::Color resolve(Color c, Gfx::Color currentColor);

} // namespace Vaev
