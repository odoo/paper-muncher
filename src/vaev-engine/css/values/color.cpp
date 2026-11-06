module;

#include <karm/macros>

export module Vaev.Engine:css.values.color;

import Karm.Core;
import Karm.Gfx;
import Karm.Logger;

import :css.lexer;
import :css.parser;
import :css.serializer;
import :css.values.base;
import :css.values.keywords;
import :css.values.length;
import :css.values.primitives;
import :css.values.unitValue;
import :values.keywords;
import :values.primitives;

using namespace Karm;
using namespace Karm::Literals;

// https://www.w3.org/TR/css-color-4

namespace Vaev::Css {

// MARK: Color Interpolation ---------------------------------------------------

// https://www.w3.org/TR/css-color-4/#typedef-color-space
export struct ColorSpace {
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
    void _preparePolar(Polar& a, Polar& b) const {
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
    Polar _interpolatePolar(Polar a, Polar b, f64 t) const {
        _preparePolar(a, b);
        return a.lerpWith(b, t);
    }

    // https://drafts.csswg.org/css-color-4/#interpolation
    // Not compliant regarding missing or carried values
    Gfx::Color interpolate(Gfx::Color a, Gfx::Color b, f64 t) const {
        a = a.premultiply();
        b = b.premultiply();

        if (type == _Type::RECTANGULAR) {
            switch (rectangular) {
            case _Rectangular::SRGB:
                return a.lerpWith(b, t).unpremultiply();

            default:
                logWarn("interpolation method is not supported: {}", rectangular);
                return Gfx::FUCHSIA;
            }
        } else if (type == _Type::POLAR) {
            switch (polar) {
            case _Polar::HSL:
                return Gfx::hslToRgb(
                           _interpolatePolar<Gfx::Hsl>(
                               Gfx::rgbToHsl(a),
                               Gfx::rgbToHsl(b),
                               t
                           )
                )
                    .unpremultiply();

            default:
                logWarn("interpolation method is not supported: {}", polar);
                return Gfx::FUCHSIA;
            }
        }

        return Gfx::WHITE;
    }
};

// MARK: Color -----------------------------------------------------------------
// https://drafts.csswg.org/css-color

export enum struct SystemColor : u8 {
#define COLOR(NAME, ...) NAME,
#include "../../values/defs/system-colors.inc"

#undef COLOR

    _LEN
};

export struct ColorMix;

export using Color = Union<
    Gfx::Color,
    Keywords::CurrentColor,
    SystemColor,
    Box<ColorMix>>;

export struct ColorMix {
    struct Side {
        Color color;
        Opt<PurePercentage> perc;

        void repr(Io::Emit& e) const {
            e("{}", color);
            if (perc)
                e(" {}", *perc);
        }
    };

    ColorSpace colorSpace;
    Side lhs;
    Side rhs;

    void repr(Io::Emit& e) const {
        e("(color-mix {} {} {})", colorSpace, lhs, rhs);
    }
};

#define COLOR(NAME, _, VALUE) \
    export constexpr Gfx::Color NAME = Gfx::Color::fromHex(VALUE);
#include "../../values/defs/colors.inc"
#undef COLOR

export Opt<Color> parseNamedColor(Str name) {

#define COLOR(ID, NAME, ...)   \
    if (eqCi(name, #NAME ""s)) \
        return Some(ID);
#include "../../values/defs/colors.inc"

#undef COLOR

    return NONE;
}

export Opt<SystemColor> parseSystemColor(Str name) {
#define COLOR(ID, NAME, ...)   \
    if (eqCi(name, #NAME ""s)) \
        return Some(SystemColor::ID);
#include "../../values/defs/system-colors.inc"

#undef COLOR

// https://drafts.csswg.org/css-color-4/#deprecated-system-colors
#define ALIAS(NAME, ID)        \
    if (eqCi(name, #NAME ""s)) \
        return Some(SystemColor::ID);
#include "../../values/defs/deprecated-system-colors.inc"

#undef ALIAS

    return NONE;
}

export template <>
struct ValueParser<Color> {

    static Res<Gfx::Color> _parseHexColor(Io::SScan& s) {
        if (s.next() != '#')
            panic("expected '#'");

        auto nextHex = [&](usize len) {
            return Io::atou(s.slice(len), {.base = 16}).unwrapOr(0);
        };

        if (s.rem() == 3) {
            // #RGB
            auto r = nextHex(1);
            auto g = nextHex(1);
            auto b = nextHex(1);
            return Ok(Gfx::Color::fromRgb(r | (r << 4), g | (g << 4), b | (b << 4)));
        } else if (s.rem() == 4) {
            // #RGBA
            auto r = nextHex(1);
            auto g = nextHex(1);
            auto b = nextHex(1);
            auto a = nextHex(1);
            return Ok(Gfx::Color::fromRgba(r | (r << 4), g | (g << 4), b | (b << 4), a | (a << 4)));
        } else if (s.rem() == 6) {
            // #RRGGBB
            auto r = nextHex(2);
            auto g = nextHex(2);
            auto b = nextHex(2);
            return Ok(Gfx::Color::fromRgb(r, g, b));
        } else if (s.rem() == 8) {
            // #RRGGBBAA
            auto r = nextHex(2);
            auto g = nextHex(2);
            auto b = nextHex(2);
            auto a = nextHex(2);
            return Ok(Gfx::Color::fromRgba(r, g, b, a));
        } else {
            return Error::invalidData("invalid color format");
        }
    }

    static Res<u8> _parseAlphaValue(Cursor<Sst>& scan) {
        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        if (scan.peek() == Token::Type::PERCENTAGE) {
            auto perc = try$(parseValue<PurePercentage>(scan)).value();
            return Ok(Math::round((perc * 255) / 100));
        } else if (scan.peek() == Token::Type::NUMBER)
            return Ok(Math::round(try$(parseValue<f64>(scan)) * 255));
        return Error::invalidData("expected alpha channel");
    }

    static Res<Opt<u8>> _parseAlphaComponentModernSyntax(Cursor<Sst>& scan) {
        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        if (not scan.skip(Token::delim("/")))
            return Error::invalidData("expected '/' before alpha channel");

        eatWhitespace(scan);
        if (scan.skip(Token::ident("none")))
            return Ok(NONE);

        return Ok(Some(try$(_parseAlphaValue(scan))));
    }

    static Res<f64> _parseNumPercNone(Cursor<Sst>& scan, f64 percOf, f64 noneValue = 0) {
        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        if (scan.skip(Token::ident("none"))) {
            return Ok(noneValue);
        } else if (scan.peek() == Token::Type::PERCENTAGE) {
            auto perc = try$(parseValue<PurePercentage>(scan));
            return Ok((perc.value() * percOf) / 100.0f);
        } else
            return Ok(try$(parseValue<f64>(scan)));
    }

    static Res<Array<f64, 3>> _parsePredefinedRGBParams(Cursor<Sst>& scan, f64 percOf = 255) {
        Array<f64, 3> channels{};
        for (usize i = 0; i < 3; ++i) {

            if (scan.ended())
                return Error::invalidData("unexpected end of input");

            channels[i] = Math::round(try$(_parseNumPercNone(scan, percOf)));

            eatWhitespace(scan);
        }
        return Ok(channels);
    }

    // https://drafts.csswg.org/css-color/#rgb-functions
    static Res<Gfx::Color> _parseSRGBModernFuncColor(Sst const& s) {
        Cursor<Sst> scan = s.content;
        eatWhitespace(scan);

        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        auto channels = try$(_parsePredefinedRGBParams(scan));
        u8 r = Math::round(channels[0]);
        u8 g = Math::round(channels[1]);
        u8 b = Math::round(channels[2]);

        eatWhitespace(scan);

        if (scan.ended())
            return Ok(Gfx::Color::fromRgb(r, g, b));

        auto alphaComponent = try$(_parseAlphaComponentModernSyntax(scan));
        return Ok(Gfx::Color::fromRgba(r, g, b, alphaComponent.unwrapOr(0)));
    }

    static Res<Gfx::Color> _parseSRGBLegacyFuncColor(Sst const& s) {
        Cursor<Sst> scan = s.content;
        eatWhitespace(scan);

        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        bool usingPercentages = scan.peek() == Token::Type::PERCENTAGE;

        Array<u8, 3> channels{};
        for (usize i = 0; i < 3; ++i) {
            if (scan.ended())
                return Error::invalidData("unexpected end of input");

            if (usingPercentages) {
                auto perc = try$(parseValue<PurePercentage>(scan));
                channels[i] = Math::round((perc.value() * 255) / 100.0f);
            } else {
                channels[i] = try$(parseValue<f64>(scan));
            }
            eatWhitespace(scan);
            if (i < 2) {
                if (not scan.skip(Token::COMMA))
                    return Error::invalidData("expected comma between rgb color channels");
                eatWhitespace(scan);
            }
        }

        u8 r = channels[0];
        u8 g = channels[1];
        u8 b = channels[2];

        eatWhitespace(scan);
        if (scan.ended())
            return Ok(Gfx::Color::fromRgb(r, g, b));

        if (not scan.skip(Token::COMMA))
            return Error::invalidData("expected comma before alpha channel");

        eatWhitespace(scan);
        return Ok(Gfx::Color::fromRgba(r, g, b, try$(_parseAlphaValue(scan))));
    }

    // https://drafts.csswg.org/css-color/#funcdef-hsl
    static Res<Gfx::Color> _parseHSLModernFuncColor(Sst const& s) {
        Cursor<Sst> scan = s.content;
        eatWhitespace(scan);

        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        f64 hue;
        if (scan.skip(Token::ident("none")))
            hue = 0;
        else if (scan.peek() == Token::DIMENSION)
            hue = try$(parseValue<PureAngle>(scan)).toDegree();
        else if (scan.peek() == Token::Type::NUMBER)
            hue = try$(parseValue<f64>(scan));
        else
            return Error::invalidData("expected hue");

        eatWhitespace(scan);

        auto sat = try$(_parseNumPercNone(scan, 1));
        eatWhitespace(scan);

        auto l = try$(_parseNumPercNone(scan, 1));
        eatWhitespace(scan);

        if (scan.ended())
            return Ok(Gfx::hslToRgb(Gfx::Hsl{hue, sat, l}));

        auto alpha = try$(_parseAlphaComponentModernSyntax(scan))
                         .unwrapOr(255);
        auto color = Gfx::hslToRgb(Gfx::Hsl{hue, sat, l});
        return Ok(color.withAlpha(alpha));
    }

    static Res<Gfx::Color> _parseHSLLegacyFuncColor(Sst const& s) {
        Cursor<Sst> scan = s.content;
        eatWhitespace(scan);

        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        f64 hue;
        if (scan.skip(Token::ident("none")))
            hue = 0;
        else if (scan.peek() == Token::DIMENSION)
            hue = try$(parseValue<PureAngle>(scan)).toDegree();
        else if (scan.peek() == Token::Type::NUMBER)
            hue = try$(parseValue<f64>(scan));
        else
            return Error::invalidData("expected hue");

        eatWhitespace(scan);

        if (not scan.skip(Token::COMMA))
            return Error::invalidData("comma is expected separating hsl components");
        eatWhitespace(scan);

        auto sat = try$(parseValue<PurePercentage>(scan)).value() / 100.0f;
        eatWhitespace(scan);

        if (not scan.skip(Token::COMMA))
            return Error::invalidData("comma is expected separating hsl components");
        eatWhitespace(scan);

        auto l = try$(parseValue<PurePercentage>(scan)).value() / 100.0f;
        eatWhitespace(scan);

        if (scan.ended()) {
            auto color = Gfx::hslToRgb(Gfx::Hsl{hue, sat, l});
            return Ok(color);
        }

        if (not scan.skip(Token::COMMA))
            return Error::invalidData("expected comma before alpha channel");

        eatWhitespace(scan);
        auto alpha = try$(_parseAlphaValue(scan));
        auto color = Gfx::hslToRgb(Gfx::Hsl{hue, sat, l});
        return Ok(color.withAlpha(alpha));
    }

    // https://drafts.csswg.org/css-color-4/#predefined
    static Res<Gfx::Color> _parseColorFuncColor(Sst const& s) {
        Cursor<Sst> scan = s.content;
        eatWhitespace(scan);

        if (scan.ended() or not(scan.peek() == Token::IDENT))
            return Error::invalidData("expected color-space identifier");

        auto colorSpace = scan.next().token.data;
        eatWhitespace(scan);

        // FIXME: predefined-rgb
        if (
            colorSpace == "srgb" or colorSpace == "srgb-linear" or
            colorSpace == "display-p3" or colorSpace == "a98-rgb" or
            colorSpace == "prophoto-rgb" or colorSpace == "rec2020"
        ) {
            auto channels = try$(_parsePredefinedRGBParams(scan, 1));

            f64 r = channels[0];
            f64 g = channels[1];
            f64 b = channels[2];

            eatWhitespace(scan);

            if (scan.ended()) {
                // FIXME: dispatch to constructor of colorSpace without alpha
                return Ok(
                    Gfx::Color::fromRgb(
                        Math::round(r * 255),
                        Math::round(g * 255),
                        Math::round(b * 255)
                    )
                );
            }

            if (auto alphaComponent = try$(_parseAlphaComponentModernSyntax(scan))) {
                // FIXME: dispatch to constructor of colorSpace with alpha
                return Ok(
                    Gfx::Color::fromRgba(
                        Math::round(r * 255),
                        Math::round(g * 255),
                        Math::round(b * 255),
                        alphaComponent.expect()
                    )
                );
            } else {
                // FIXME: correctly deal with missing alpha values
                // FIXME: dispatch to constructor of colorSpace with missing alpha
                return Ok(
                    Gfx::Color::fromRgba(
                        Math::round(r * 255),
                        Math::round(g * 255),
                        Math::round(b * 255),
                        0
                    )
                );
            }
        } else {
            logWarn("predefined color space not implemented: {}", colorSpace);
            return Ok(Gfx::WHITE);
        }
    }

    static Res<Gfx::Color> _parseFuncColor(Sst const& s) {
        if (s.prefix == Token::function("color(")) {
            return _parseColorFuncColor(s);
        } else if (s.prefix == Token::function("rgb(") or s.prefix == Token::function("rgba(")) {
            auto modernColor = _parseSRGBModernFuncColor(s);
            if (modernColor)
                return modernColor;

            return _parseSRGBLegacyFuncColor(s);
        } else if (s.prefix == Token::function("hsl(") or s.prefix == Token::function("hsla(")) {
            auto modernColor = _parseHSLModernFuncColor(s);
            if (modernColor)
                return modernColor;

            return _parseHSLLegacyFuncColor(s);
        } else {
            return Error::invalidData("unknown color function");
        }
    }

    static Res<ColorMix::Side> _parseColorMixSide(Cursor<Sst>& s) {
        Opt<PurePercentage> percent;

        if (s.ended())
            return Error::invalidData("unexpected end of input");

        if (s.peek() == Token::Type::PERCENTAGE) {
            percent = Some(try$(parseValue<PurePercentage>(s)));
            eatWhitespace(s);
            return Ok(ColorMix::Side{
                try$(parseValue<Color>(s)),
                percent,
            });
        }

        Color color = try$(parseValue<Color>(s));
        eatWhitespace(s);
        if (not s.ended() and s.peek() == Token::Type::PERCENTAGE)
            percent = Some(try$(parseValue<PurePercentage>(s)).value());

        return Ok(ColorMix::Side{
            std::move(color),
            percent,
        });
    }

    static Res<Color> _parseColorMixFunc(Sst const& s) {
        Cursor<Sst> scan = s.content;

        eatWhitespace(scan);
        if (not scan.skip(Token::ident("in"))) // FIXME: correct token class?
            return Error::invalidData("expected 'in' keyword");

        eatWhitespace(scan);
        if (scan.ended() or not(scan.peek() == Token::IDENT))
            return Error::invalidData("expected color-space identifier");

        // FIXME: not parsing interpolation method for polar spaces
        auto colorSpace = scan.next().token.data;
        if (not skipOmmitableComma(scan))
            return Error::invalidData("expected comma separting color mix arguments");

        auto lhs = try$(_parseColorMixSide(scan));
        if (not skipOmmitableComma(scan))
            return Error::invalidData("expected comma");

        auto rhs = try$(_parseColorMixSide(scan));
        return Ok(makeBox<ColorMix>(ColorSpace::fromStr(colorSpace), std::move(lhs), std::move(rhs)));
    }

    static Res<Color> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Token::HASH) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(try$(_parseHexColor(scan)));
        } else if (c.peek() == Token::IDENT) {
            Str data = c->token.data.str();

            if (eqCi(data, "currentcolor"s)) {
                c.next();
                return Ok(Keywords::CURRENT_COLOR);
            }

            if (eqCi(data, "transparent"s)) {
                c.next();
                return Ok(Gfx::ALPHA);
            }

            auto maybeColor = parseNamedColor(data);
            if (maybeColor) {
                c.next();
                return Ok(maybeColor.expect());
            }

            auto maybeSystemColor = parseSystemColor(data);
            if (maybeSystemColor) {
                c.next();
                return Ok(maybeSystemColor.expect());
            }

        } else if (c.peek() == Sst::FUNC) {

            if (c->prefix == Token::function("color-mix(")) {
                auto color = try$(_parseColorMixFunc(*c));
                c.next();
                return Ok(color);
            }

            auto color = try$(_parseFuncColor(*c));
            c.next();
            return Ok(color);
        }

        return Error::invalidData("expected color");
    }
};

export template <>
struct ValueSerializer<Color> {
    // https://drafts.csswg.org/css-color-4/#serializing-color-values
    static Res<> serialize(Io::TextWriter& w, Color const& value) {
        // FIXME: This is a rough approximate of the spec and only supports srgb.

        return value.visit(
            [&](Gfx::Color const& srgb) -> Res<> {
                if (srgb.alpha == Limits<u8>::MAX) {
                    try$(w.writeStr("rgb("s));
                } else {
                    try$(w.writeStr("rgba("s));
                }

                try$(serializeValue(w, static_cast<Integer>(srgb.red)));
                try$(w.writeStr(", "s));
                try$(serializeValue(w, static_cast<Integer>(srgb.green)));
                try$(w.writeStr(", "s));
                try$(serializeValue(w, static_cast<Integer>(srgb.blue)));

                if (srgb.alpha < Limits<u8>::MAX) {
                    try$(w.writeStr(", "s));
                    try$(serializeValue(w, static_cast<f64>(srgb.alpha) / 255.0));
                }

                try$(w.writeStr(")"s));

                return Ok();
            },
            [&](auto const& other) -> Res<> {
                return Io::format(w, "{}", other);
            }
        );
    }
};

export constexpr Array<Gfx::Color, static_cast<usize>(SystemColor::_LEN)> SYSTEM_COLOR = {
#define COLOR(NAME, _, VALUE) Gfx::Color::fromHex(VALUE),
#include "../../values/defs/system-colors.inc"

#undef COLOR
};

} // namespace Vaev::Css
