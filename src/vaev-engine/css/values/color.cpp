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
import :css.values.primitives;
import :css.values.numeric;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Css {

// https://drafts.csswg.org/css-color-4/#typedef-color-space
export struct ColorSpace {
    enum struct _Type {
        POLAR,
        RECTANGULAR,

        _LEN0,
    };

    using enum _Type;

    // https://drafts.csswg.org/css-color-4/#typedef-rectangular-color-space
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

    // https://drafts.csswg.org/css-color-4/#typedef-hue-interpolation-method
    enum struct _Interpolation {
        SHORTER,
        LONGER,
        INCREASING,
        DECREASING,

        _LEN2,
    };

    using enum _Interpolation;

    // https://drafts.csswg.org/css-color-4/#typedef-polar-color-space
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
};

// https://drafts.csswg.org/css-color-4/#typedef-system-color
export enum struct SystemColor : u8 {
#define COLOR(NAME, ...) NAME,
#include "defs/system-colors.inc"

#undef COLOR

    _LEN
};

// https://drafts.csswg.org/css-color-4/#specifying-oklab-oklch
export struct Oklch {
    // https://drafts.csswg.org/css-color-4/#serializing-oklab-oklch
    // (half float or float, is recommended for internal storage).
    Opt<f32> lightness;
    Opt<f32> chroma;
    Opt<f32> hue;
    Opt<f32> alpha;

    bool operator==(Oklch const& other) const = default;

    void repr(Io::Emit& e) const {
        e("(oklch {} {} {} {})", lightness, chroma, hue, alpha);
    }
};

export struct ColorMix;

// https://drafts.csswg.org/css-color-4/#typedef-color
export using Color = Union<
    Gfx::Color,
    Oklch,
    Keywords::CurrentColor,
    SystemColor,
    Box<ColorMix>>;

// https://drafts.csswg.org/css-color-5/#funcdef-color-mix
export struct ColorMix {
    struct Side {
        Color color;
        // FIXME: Allow calc.
        Opt<LegacyPercentage> perc;

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

// https://drafts.csswg.org/css-color-4/#typedef-named-color
#define COLOR(NAME, _, VALUE) \
    export constexpr Gfx::Color NAME = Gfx::Color::fromHex(VALUE);
#include "defs/colors.inc"
#undef COLOR

Opt<Color> parseNamedColor(Str name) {

#define COLOR(ID, NAME, ...)   \
    if (eqCi(name, #NAME ""s)) \
        return Some(ID);
#include "defs/colors.inc"

#undef COLOR

    return NONE;
}

Opt<SystemColor> parseSystemColor(Str name) {
#define COLOR(ID, NAME, ...)   \
    if (eqCi(name, #NAME ""s)) \
        return Some(SystemColor::ID);
#include "defs/system-colors.inc"

#undef COLOR

// https://drafts.csswg.org/css-color-4/#deprecated-system-colors
#define ALIAS(NAME, ID)        \
    if (eqCi(name, #NAME ""s)) \
        return Some(SystemColor::ID);
#include "defs/deprecated-system-colors.inc"

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

        // FIXME: Allow calc.
        if (scan.peek() == Token::Type::PERCENTAGE) {
            auto perc = try$(parseValue<LegacyPercentage>(scan)).value();
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

        // FIXME: Allow calc.
        if (scan.skip(Token::ident("none"))) {
            return Ok(noneValue);
        } else if (scan.peek() == Token::Type::PERCENTAGE) {
            auto perc = try$(parseValue<LegacyPercentage>(scan));
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

            // FIXME: Allow calc.
            if (usingPercentages) {
                auto perc = try$(parseValue<LegacyPercentage>(scan));
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

    // https://drafts.csswg.org/css-color-4/#typedef-modern-hsl-syntax
    static Res<Gfx::Color> _parseHSLModernFuncColor(Sst const& s) {
        Cursor<Sst> scan = s.content;
        eatWhitespace(scan);

        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        // FIXME: Allow calc.
        f64 hue;
        if (scan.skip(Token::ident("none")))
            hue = 0;
        else if (scan.peek() == Token::DIMENSION)
            hue = try$(parseValue<LegacyAngle>(scan)).unitValue().to(UnitValue::Unit::DEG);
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

    // https://drafts.csswg.org/css-color-4/#typedef-legacy-hsl-syntax
    static Res<Gfx::Color> _parseHSLLegacyFuncColor(Sst const& s) {
        Cursor<Sst> scan = s.content;
        eatWhitespace(scan);

        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        // FIXME: Allow calc.
        f64 hue;
        if (scan.skip(Token::ident("none")))
            hue = 0;
        else if (scan.peek() == Token::DIMENSION)
            hue = try$(parseValue<LegacyAngle>(scan)).unitValue().to(UnitValue::Unit::DEG);
        else if (scan.peek() == Token::Type::NUMBER)
            hue = try$(parseValue<f64>(scan));
        else
            return Error::invalidData("expected hue");

        eatWhitespace(scan);

        if (not scan.skip(Token::COMMA))
            return Error::invalidData("comma is expected separating hsl components");
        eatWhitespace(scan);

        // FIXME: Allow calc.
        auto sat = try$(parseValue<LegacyPercentage>(scan)).value() / 100.0f;
        eatWhitespace(scan);

        if (not scan.skip(Token::COMMA))
            return Error::invalidData("comma is expected separating hsl components");
        eatWhitespace(scan);

        // FIXME: Allow calc.
        auto l = try$(parseValue<LegacyPercentage>(scan)).value() / 100.0f;
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

    // https://drafts.csswg.org/css-color-4/#funcdef-color
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
        // FIXME: Allow calc.
        Opt<LegacyPercentage> percent;

        if (s.ended())
            return Error::invalidData("unexpected end of input");

        if (s.peek() == Token::Type::PERCENTAGE) {
            percent = Some(try$(parseValue<LegacyPercentage>(s)));
            eatWhitespace(s);
            return Ok(ColorMix::Side{
                try$(parseValue<Color>(s)),
                percent,
            });
        }

        Color color = try$(parseValue<Color>(s));
        eatWhitespace(s);
        if (not s.ended() and s.peek() == Token::Type::PERCENTAGE)
            percent = Some(try$(parseValue<LegacyPercentage>(s)));

        return Ok(ColorMix::Side{
            std::move(color),
            percent,
        });
    }

    // [ <percentage> | <number> | none]
    static Res<Opt<f64>> _parseNumberOrPercentage(Cursor<Sst>& scan, f64 percentReference) {
        eatWhitespace(scan);
        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        // FIXME: Allow calc.
        if (scan.skip(Token::ident("none")))
            return Ok(NONE);

        if (scan.peek() == Token::Type::PERCENTAGE)
            return Ok(Some(try$(parseValue<LegacyPercentage>(scan)).value() * percentReference / 100));

        if (scan.peek() == Token::Type::NUMBER)
            return Ok(Some(try$(parseValue<f64>(scan))));

        return Error::invalidData("expected number or percentage");
    }

    // https://drafts.csswg.org/css-color-4/#hue-syntax
    // <hue> = <number> | <angle>
    static Res<Opt<f64>> _parseHue(Cursor<Sst>& scan) {
        eatWhitespace(scan);
        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        // FIXME: Allow calc.
        if (scan.skip(Token::ident("none")))
            return Ok(NONE);

        f64 degrees;
        if (scan.peek() == Token::DIMENSION)
            degrees = try$(parseValue<LegacyAngle>(scan)).unitValue().to(UnitValue::Unit::DEG);
        else if (scan.peek() == Token::Type::NUMBER)
            degrees = try$(parseValue<f64>(scan));
        else
            return Error::invalidData("expected hue");

        // This number is normalized to the range [0,360).
        // In hsl(calc(-infinity) 0 0) or hsl(calc(infinity) 0 0), the <hue> component is again normalized to 0
        // degrees.
        if (Math::isInf(degrees))
            return Ok(Some(0.0));

        f32 hue = Math::fmod(degrees, 360.0);
        if (hue < 0)
            hue += 360;

        // NOTE: The hue is stored in a f32, a hue just below 360 is rounded up to 360.
        if (hue >= 360)
            hue = 0;

        return Ok(Some(hue));
    }

    // https://drafts.csswg.org/css-color-4/#typedef-color-alpha-value
    // <alpha-value> = <number> | <percentage>
    static Res<Opt<f64>> _parseAlpha(Cursor<Sst>& scan) {
        eatWhitespace(scan);

        // Unless otherwise specified, an <alpha-value> component of a color defaults to 100% when omitted.
        if (not scan.skip(Token::delim("/")))
            return Ok(Some(1.0));

        auto alpha = try$(_parseNumberOrPercentage(scan, 1));

        // Values outside the range [0,1] are not invalid, but are clamped to that range at parsed-value time.
        return Ok(alpha.map([](f64 value) {
            return clamp01(value);
        }));
    }

    // https://drafts.csswg.org/css-color-4/#specifying-oklab-oklch
    // oklch() = oklch( [ <percentage> | <number> | none]
    //       [ <percentage> | <number> | none]
    //       [ <hue> | none]
    //       [ / [<alpha-value> | none] ]? )
    static Res<Oklch> _parseOklchFunc(Sst const& s) {
        Cursor<Sst> scan = s.content;

        auto lightness = try$(_parseNumberOrPercentage(scan, 1)).map([](f64 value) {
            return clamp01(value);
        });

        // for C: 0% = 0.0 100% = 0.4
        // If the provided value is negative, it is clamped to 0 at parsed-value time.
        // https://drafts.csswg.org/css-values-4/#numeric-ranges
        // CSS theoretically supports infinite precision and infinite ranges for all value types; however in reality
        // implementations have finite capacity. UAs should support reasonably useful ranges and precisions.
        // NOTE: The chroma is stored in a f32, it is clamped to the largest f32.
        auto chroma = try$(_parseNumberOrPercentage(scan, 0.4)).map([](f64 value) {
            return clamp(value, 0.0, static_cast<f64>(Limits<f32>::MAX));
        });

        auto hue = try$(_parseHue(scan));
        auto alpha = try$(_parseAlpha(scan));

        eatWhitespace(scan);
        if (not scan.ended())
            return Error::invalidData("unexpected tokens at the end of oklch()");

        return Ok(Oklch{lightness, chroma, hue, alpha});
    }

    // https://drafts.csswg.org/css-color-5/#funcdef-color-mix
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

            if (c->prefix == Token::function("oklch(")) {
                auto color = try$(_parseOklchFunc(*c));
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
            // https://drafts.csswg.org/css-color-4/#serializing-oklab-oklch
            [&](Oklch const& oklch) -> Res<> {
                try$(w.writeStr("oklch("s));

                try$(oklch.lightness ? serializeValue(w, static_cast<f64>(*oklch.lightness)) : w.writeStr("none"s));
                try$(w.writeStr(" "s));
                try$(oklch.chroma ? serializeValue(w, static_cast<f64>(*oklch.chroma)) : w.writeStr("none"s));
                try$(w.writeStr(" "s));
                try$(oklch.hue ? serializeValue(w, static_cast<f64>(*oklch.hue)) : w.writeStr("none"s));

                // As noted earlier, unitary alpha values are not explicitly serialized. Non-unitary alpha values must
                // be explicitly serialized, and the string " / " (an ASCII space, then forward slash, then another
                // space) must be used to separate the final color component (b, or C) value from the alpha value.
                if (oklch.alpha != 1.0f) {
                    try$(w.writeStr(" / "s));
                    try$(oklch.alpha ? serializeValue(w, static_cast<f64>(*oklch.alpha)) : w.writeStr("none"s));
                }

                return w.writeStr(")"s);
            },
            [&](auto const& other) -> Res<> {
                return Io::format(w, "{}", other);
            }
        );
    }
};

// https://drafts.csswg.org/css-color-4/#typedef-opacity-opacity-value
// <opacity-value> = <number> | <percentage>
export using OpacityValue = Union<Number, Percentage>;

export template <>
struct ValueParser<OpacityValue> {
    static Res<OpacityValue> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        // NOTE: A literal <percentage> is equivalent to the <number> it maps to,
        //       only math functions keep their percentages.
        if (c.peek() == Token::PERCENTAGE) {
            auto percentage = try$(parseValue<LegacyPercentage>(c));
            return Ok(number(percentage.value() / 100.));
        }

        if (auto n = parseValue<Number>(c))
            return Ok(n.take());
        return Ok(try$(parseValue<Percentage>(c)));
    }
};

} // namespace Vaev::Css
