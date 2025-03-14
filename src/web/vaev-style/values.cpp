module;

#include <karm-io/aton.h>
#include <karm-print/paper.h>
#include <vaev-base/align.h>
#include <vaev-base/borders.h>
#include <vaev-base/break.h>
#include <vaev-base/calc.h>
#include <vaev-base/color.h>
#include <vaev-base/display.h>
#include <vaev-base/flex.h>
#include <vaev-base/float.h>
#include <vaev-base/font.h>
#include <vaev-base/insets.h>
#include <vaev-base/keywords.h>
#include <vaev-base/length.h>
#include <vaev-base/line.h>
#include <vaev-base/media.h>
#include <vaev-base/numbers.h>
#include <vaev-base/overflow.h>
#include <vaev-base/resolution.h>
#include <vaev-base/sizing.h>
#include <vaev-base/table.h>
#include <vaev-base/z-index.h>

export module Vaev.Style:values;

import Vaev.Style.Css;
import :base;

namespace Vaev::Style {

// MARK: Parser ----------------------------------------------------------------

// NOTE: Please keep this alphabetically sorted.

export template <typename T>
struct ValueParser;

export template <typename T>
always_inline Res<T> parseValue(Cursor<Css::Sst>& c) {
    return ValueParser<T>::parse(c);
}

export template <typename T>
always_inline Res<T> parseValue(Str str) {
    Css::Lexer lex{str};
    auto content = Css::consumeDeclarationValue(lex);
    Cursor<Css::Sst> c{content};
    return ValueParser<T>::parse(c);
}

// MARK: Align
// https://drafts.csswg.org/css-align-3/#propdef-align-content
// https://drafts.csswg.org/css-align-3/#propdef-justify-content
// https://drafts.csswg.org/css-align-3/#propdef-justify-self
// https://drafts.csswg.org/css-align-3/#propdef-align-self
// https://drafts.csswg.org/css-align-3/#propdef-justify-items
// https://drafts.csswg.org/css-align-3/#propdef-align-items

template <>
struct ValueParser<Align> {
    static Res<Align> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        Align align;

        if (c.skip(Css::Token::ident("legacy"))) {
            align.prefix = Align::Prefix::LEGACY;
        } else if (c.skip(Css::Token::ident("safe"))) {
            align.prefix = Align::Prefix::SAFE;
        } else if (c.skip(Css::Token::ident("unsafe"))) {
            align.prefix = Align::Prefix::UNSAFE;
        }

        if (c.skip(Css::Token::ident("auto"))) {
            align.keyword = Align::Keywords::AUTO;
        } else if (c.skip(Css::Token::ident("normal"))) {
            align.keyword = Align::Keywords::NORMAL;
        } else if (c.skip(Css::Token::ident("center"))) {
            align.keyword = Align::Keywords::CENTER;
        } else if (c.skip(Css::Token::ident("start"))) {
            align.keyword = Align::Keywords::START;
        } else if (c.skip(Css::Token::ident("end"))) {
            align.keyword = Align::Keywords::END;
        } else if (c.skip(Css::Token::ident("self-start"))) {
            align.keyword = Align::Keywords::SELF_START;
        } else if (c.skip(Css::Token::ident("self-end"))) {
            align.keyword = Align::Keywords::SELF_END;
        } else if (c.skip(Css::Token::ident("flex-start"))) {
            align.keyword = Align::Keywords::FLEX_START;
        } else if (c.skip(Css::Token::ident("flex-end"))) {
            align.keyword = Align::Keywords::FLEX_END;
        } else if (c.skip(Css::Token::ident("left"))) {
            align.keyword = Align::Keywords::LEFT;
        } else if (c.skip(Css::Token::ident("right"))) {
            align.keyword = Align::Keywords::RIGHT;
        } else if (c.skip(Css::Token::ident("first-baseline"))) {
            align.keyword = Align::Keywords::FIRST_BASELINE;
        } else if (c.skip(Css::Token::ident("last-baseline"))) {
            align.keyword = Align::Keywords::LAST_BASELINE;
        } else if (c.skip(Css::Token::ident("stretch"))) {
            align.keyword = Align::Keywords::STRETCH;
        } else if (c.skip(Css::Token::ident("space-between"))) {
            align.keyword = Align::Keywords::SPACE_BETWEEN;
        } else if (c.skip(Css::Token::ident("space-around"))) {
            align.keyword = Align::Keywords::SPACE_AROUND;
        } else if (c.skip(Css::Token::ident("space-evenly"))) {
            align.keyword = Align::Keywords::SPACE_EVENLY;
        }

        if (align == Align::NO_PREFIX and align == Align::NO_KEYWORD)
            return Error::invalidData("expected align value");

        return Ok(align);
    }
};

// MARK: Angle
// https://drafts.csswg.org/css-values/#angles

static Res<Angle::Unit> _parseAngleUnit(Str unit) {
    if (eqCi(unit, "deg"s))
        return Ok(Angle::Unit::DEGREE);
    else if (eqCi(unit, "grad"s))
        return Ok(Angle::Unit::GRAD);
    else if (eqCi(unit, "rad"s))
        return Ok(Angle::Unit::RADIAN);
    else if (eqCi(unit, "turn"s))
        return Ok(Angle::Unit::TURN);
    else
        return Error::invalidData("unknown length unit");
}

template <>
struct ValueParser<Angle> {
    static Res<Angle> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::DIMENSION) {
            Io::SScan scan = c->token.data.str();
            auto value = Io::atof(scan).unwrapOr(0.0);
            auto unit = try$(_parseAngleUnit(scan.remStr()));

            c.next();
            return Ok(Angle{value, unit});
        }

        return Error::invalidData("expected angle");
    }
};

// MARK: Boolean
// https://drafts.csswg.org/mediaqueries/#grid

template <>
struct ValueParser<bool> {
    static Res<bool> parse(Cursor<Css::Sst>& c) {
        auto val = try$(parseValue<Integer>(c));
        return Ok(val > 0);
    }
};

// MARK: Border-Style
// https://www.w3.org/TR/CSS22/box.html#border-style-properties

template <>
struct ValueParser<Gfx::BorderStyle> {
    static Res<Gfx::BorderStyle> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of property");

        if (c.skip(Css::Token::ident("none"))) {
            return Ok(Gfx::BorderStyle::NONE);
        } else if (c.skip(Css::Token::ident("solid"))) {
            return Ok(Gfx::BorderStyle::SOLID);
        } else if (c.skip(Css::Token::ident("dashed"))) {
            return Ok(Gfx::BorderStyle::DASHED);
        } else if (c.skip(Css::Token::ident("dotted"))) {
            return Ok(Gfx::BorderStyle::DOTTED);
        } else if (c.skip(Css::Token::ident("hidden"))) {
            return Ok(Gfx::BorderStyle::HIDDEN);
        } else if (c.skip(Css::Token::ident("double"))) {
            return Ok(Gfx::BorderStyle::DOUBLE);
        } else if (c.skip(Css::Token::ident("groove"))) {
            return Ok(Gfx::BorderStyle::GROOVE);
        } else if (c.skip(Css::Token::ident("ridge"))) {
            return Ok(Gfx::BorderStyle::RIDGE);
        } else if (c.skip(Css::Token::ident("outset"))) {
            return Ok(Gfx::BorderStyle::OUTSET);
        } else {
            return Error::invalidData("unknown border-style");
        }
    }
};

template <>
struct ValueParser<Border> {
    static Res<Border> parse(Cursor<Css::Sst>& c) {
        Border border;
        while (not c.ended()) {
            eatWhitespace(c);

            auto width = parseValue<LineWidth>(c);
            if (width) {
                border.width = width.unwrap();
                continue;
            }

            auto color = parseValue<Color>(c);
            if (color) {
                border.color = color.unwrap();
                continue;
            }

            auto style = parseValue<Gfx::BorderStyle>(c);
            if (style) {
                border.style = style.unwrap();
                continue;
            }

            break;
        }

        return Ok(border);
    }
};

// MARK: BorderCollapse
// https://www.w3.org/TR/CSS22/tables.html#propdef-border-collapse

template <>
struct ValueParser<BorderCollapse> {
    static Res<BorderCollapse> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("collapse"))) {
            return Ok(BorderCollapse::COLLAPSE);
        } else if (c.skip(Css::Token::ident("separate"))) {
            return Ok(BorderCollapse::SEPARATE);
        }

        return Error::invalidData("expected border collapse value");
    }
};

// MARK: BorderSpacing
// https://www.w3.org/TR/CSS22/tables.html#propdef-border-spacing

template <>
struct ValueParser<BorderSpacing> {
    static Res<BorderSpacing> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto firstLength = parseValue<Length>(c);

        if (not firstLength)
            return Error::invalidData("expected length parameter for border-spacing");

        auto secondLength = parseValue<Length>(c);

        if (secondLength) {
            return Ok(BorderSpacing{firstLength.unwrap(), secondLength.unwrap()});
        } else {
            return Ok(BorderSpacing{firstLength.unwrap(), firstLength.unwrap()});
        }

        return Error::invalidData("expected border spacing value");
    }
};

// MARK: line-width

template <>
struct ValueParser<LineWidth> {
    static Res<LineWidth> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::ident("thin")) {
            c.next();
            return Ok(Keywords::THIN);
        }
        if (c.peek() == Css::Token::ident("medium")) {
            c.next();
            return Ok(Keywords::MEDIUM);
        }
        if (c.peek() == Css::Token::ident("thick")) {
            c.next();
            return Ok(Keywords::THICK);
        }

        return Ok(try$(parseValue<CalcValue<Length>>(c)));
    }
};

// MARK: BreakAfter & BreakBefore
// https://www.w3.org/TR/css-break-3/#propdef-break-after
// https://www.w3.org/TR/css-break-3/#propdef-break-before

template <>
struct ValueParser<BreakBetween> {
    static Res<BreakBetween> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("auto"))) {
            return Ok(BreakBetween::AUTO);
        } else if (c.skip(Css::Token::ident("avoid"))) {
            return Ok(BreakBetween::AVOID);
        } else if (c.skip(Css::Token::ident("avoid-page"))) {
            return Ok(BreakBetween::AVOID_PAGE);
        } else if (c.skip(Css::Token::ident("page"))) {
            return Ok(BreakBetween::PAGE);
        } else if (c.skip(Css::Token::ident("left"))) {
            return Ok(BreakBetween::LEFT);
        } else if (c.skip(Css::Token::ident("right"))) {
            return Ok(BreakBetween::RIGHT);
        } else if (c.skip(Css::Token::ident("recto"))) {
            return Ok(BreakBetween::RECTO);
        } else if (c.skip(Css::Token::ident("verso"))) {
            return Ok(BreakBetween::VERSO);
        } else if (c.skip(Css::Token::ident("avoid-column"))) {
            return Ok(BreakBetween::AVOID_COLUMN);
        } else if (c.skip(Css::Token::ident("column"))) {
            return Ok(BreakBetween::COLUMN);
        } else if (c.skip(Css::Token::ident("avoid-region"))) {
            return Ok(BreakBetween::AVOID_REGION);
        } else if (c.skip(Css::Token::ident("region"))) {
            return Ok(BreakBetween::REGION);
        }

        return Error::invalidData("expected break between value");
    }
};

// MARK: BreakInside
// https://www.w3.org/TR/css-break-3/#break-within

template <>
struct ValueParser<BreakInside> {
    static Res<BreakInside> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("auto"))) {
            return Ok(BreakInside::AUTO);
        } else if (c.skip(Css::Token::ident("avoid"))) {
            return Ok(BreakInside::AVOID);
        } else if (c.skip(Css::Token::ident("avoid-page"))) {
            return Ok(BreakInside::AVOID_PAGE);
        } else if (c.skip(Css::Token::ident("avoid-column"))) {
            return Ok(BreakInside::AVOID_COLUMN);
        } else if (c.skip(Css::Token::ident("avoid-region"))) {
            return Ok(BreakInside::AVOID_REGION);
        }

        return Error::invalidData("expected break between value");
    }
};

template <typename T>
struct ValueParser<CalcValue<T>> {
    static Res<CalcValue<T>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Sst::FUNC) {
            auto const& prefix = c.peek().prefix;
            auto prefixToken = prefix.unwrap()->token;
            if (prefixToken.data == "calc(") {
                Cursor<Css::Sst> content = c.peek().content;
                auto lhs = try$(parseVal(content));
                c.next();

                auto op = parseOp(content);
                if (not op)
                    return Ok(CalcValue<T>{lhs});

                if (content.peek() == Css::Token::WHITESPACE) {
                    content.next();
                }
                auto rhs = try$(parseVal(content));
                return Ok(CalcValue<T>{op.unwrap(), lhs, rhs});
            }
        }

        return Ok(try$(parseValue<T>(c)));
    }

    static Res<CalcOp> parseOp(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::WHITESPACE) {
            c.next();
            return parseOp(c);
        }

        if (c.peek().token.data == "+") {
            c.next();
            return Ok(CalcOp::ADD);
        } else if (c.peek().token.data == "-") {
            c.next();
            return Ok(CalcOp::SUBSTRACT);
        } else if (c.peek().token.data == "*") {
            c.next();
            return Ok(CalcOp::MULTIPLY);
        } else if (c.peek().token.data == "/") {
            c.next();
            return Ok(CalcOp::DIVIDE);
        }
        return Error::invalidData("unexpected operator");
    }

    static Res<typename CalcValue<T>::Value> parseVal(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek().token == Css::Token::NUMBER) {
            return Ok(try$(parseValue<Number>(c)));
        }

        return Ok(try$(parseValue<T>(c)));
    }
};

// MARK: Colors
// https://drafts.csswg.org/mediaqueries/#forced-colors

template <>
struct ValueParser<Colors> {
    static Res<Colors> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok(Colors::NONE);
        else if (c.skip(Css::Token::ident("active")))
            return Ok(Colors::ACTIVE);
        else
            return Error::invalidData("expected colors value");
    }
};

// MARK: ColorScheme
// https://drafts.csswg.org/mediaqueries/#color-scheme

template <>
struct ValueParser<ColorScheme> {
    static Res<ColorScheme> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("light")))
            return Ok(ColorScheme::LIGHT);
        else if (c.skip(Css::Token::ident("dark")))
            return Ok(ColorScheme::DARK);
        else
            return Error::invalidData("expected color scheme value");
    }
};

// MARK: Color
// https://drafts.csswg.org/css-color

static Res<Number> _parseNumPercNone(Cursor<Css::Sst>& scan, Number percOf, Number noneValue = 0) {
    if (scan.ended())
        return Error::invalidData("unexpected end of input");

    if (scan.skip(Css::Token::ident("none"))) {
        return Ok(noneValue);
    } else if (scan.peek() == Css::Token::Type::PERCENTAGE) {
        auto perc = try$(parseValue<Percent>(scan));
        return Ok((perc.value() * percOf) / 100.0f);
    } else
        return Ok(try$(parseValue<Number>(scan)));
}

static Res<Array<Number, 3>> _parsePredefinedRGBParams(Cursor<Css::Sst>& scan, Number percOf = 255) {
    Array<Number, 3> channels{};
    for (usize i = 0; i < 3; ++i) {

        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        channels[i] = round(try$(_parseNumPercNone(scan, percOf)));

        eatWhitespace(scan);
    }
    return Ok(channels);
}

static Res<u8> _parseAlphaValue(Cursor<Css::Sst>& scan) {
    if (scan.ended())
        return Error::invalidData("unexpected end of input");

    if (scan.peek() == Css::Token::Type::PERCENTAGE) {
        auto perc = try$(parseValue<Percent>(scan)).value();
        return Ok(round((perc * 255) / 100));
    } else if (scan.peek() == Css::Token::Type::NUMBER)
        return Ok(round(try$(parseValue<Number>(scan)) * 255));
    return Error::invalidData("expected alpha channel");
}

static Res<Opt<u8>> _parseAlphaComponentModernSyntax(Cursor<Css::Sst>& scan) {
    if (scan.ended())
        return Error::invalidData("unexpected end of input");

    if (scan.skip(Css::Token::delim("/"))) {
        eatWhitespace(scan);
        if (scan.skip(Css::Token::ident("none")))
            return Ok(NONE);
        return Ok(try$(_parseAlphaValue(scan)));
    } else
        return Error::invalidData("expected '/' before alpha channel");
}

static Res<Gfx::Color> _parseColorFuncColor(Css::Sst const& s) {
    Cursor<Css::Sst> scan = s.content;
    eatWhitespace(scan);

    if (scan.ended() or not(scan.peek() == Css::Token::IDENT))
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

        Number r = channels[0];
        Number g = channels[1];
        Number b = channels[2];

        eatWhitespace(scan);

        if (scan.ended()) {
            // FIXME: dispatch to constructor of colorSpace without alpha
            return Ok(Gfx::Color::fromRgb(round(r * 255), round(g * 255), round(b * 255)));
        }

        if (auto alphaComponent = try$(_parseAlphaComponentModernSyntax(scan))) {
            // FIXME: dispatch to constructor of colorSpace with alpha
            return Ok(Gfx::Color::fromRgba(round(r * 255), round(g * 255), round(b * 255), alphaComponent.unwrap()));
        } else {
            // FIXME: correctly deal with missing alpha values
            // FIXME: dispatch to constructor of colorSpace with missing alpha
            return Ok(Gfx::Color::fromRgba(round(r * 255), round(g * 255), round(b * 255), 0));
        }
    } else {
        logWarn("predefined color space not implemented: {}", colorSpace);
        return Ok(Gfx::WHITE);
    }
}

static Res<Gfx::Color> _parseSRGBModernFuncColor(Css::Sst const& s) {
    Cursor<Css::Sst> scan = s.content;
    eatWhitespace(scan);

    if (scan.ended())
        return Error::invalidData("unexpected end of input");

    auto channels = try$(_parsePredefinedRGBParams(scan));
    u8 r = round(channels[0]);
    u8 g = round(channels[1]);
    u8 b = round(channels[2]);

    eatWhitespace(scan);

    if (scan.ended())
        return Ok(Gfx::Color::fromRgb(r, g, b));

    if (auto alphaComponent = try$(_parseAlphaComponentModernSyntax(scan)))
        return Ok(Gfx::Color::fromRgba(r, g, b, alphaComponent.unwrap()));
    else
        return Ok(Gfx::Color::fromRgba(r, g, b, 0));
}

static Res<Gfx::Color> _parseSRGBLegacyFuncColor(Css::Sst const& s) {
    Cursor<Css::Sst> scan = s.content;
    eatWhitespace(scan);

    if (scan.ended())
        return Error::invalidData("unexpected end of input");

    bool usingPercentages = scan.peek() == Css::Token::Type::PERCENTAGE;

    Array<u8, 3> channels{};
    for (usize i = 0; i < 3; ++i) {

        if (scan.ended())
            return Error::invalidData("unexpected end of input");

        if (usingPercentages) {
            auto perc = try$(parseValue<Percent>(scan));
            channels[i] = round((perc.value() * 255) / 100.0f);
        } else {
            channels[i] = try$(parseValue<Number>(scan));
        }
        eatWhitespace(scan);
        if (i < 2) {
            if (not scan.skip(Css::Token::COMMA))
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

    if (scan.skip(Css::Token::COMMA)) {
        eatWhitespace(scan);
        return Ok(Gfx::Color::fromRgba(r, g, b, try$(_parseAlphaValue(scan))));
    } else
        return Error::invalidData("expected comma before alpha channel");
}

// https://drafts.csswg.org/css-color/#funcdef-hsl
static Res<Gfx::Color> _parseHSLModernFuncColor(Css::Sst const& s) {
    Cursor<Css::Sst> scan = s.content;
    eatWhitespace(scan);

    if (scan.ended())
        return Error::invalidData("unexpected end of input");

    Number hue;
    if (scan.skip(Css::Token::ident("none")))
        hue = 0;
    else if (scan.peek() == Css::Token::DIMENSION)
        hue = try$(parseValue<Angle>(scan)).toDegree();
    else if (scan.peek() == Css::Token::Type::NUMBER)
        hue = try$(parseValue<Number>(scan));
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

static Res<Gfx::Color> _parseHSLLegacyFuncColor(Css::Sst const& s) {
    Cursor<Css::Sst> scan = s.content;
    eatWhitespace(scan);

    if (scan.ended())
        return Error::invalidData("unexpected end of input");

    Number hue;
    if (scan.skip(Css::Token::ident("none")))
        hue = 0;
    else if (scan.peek() == Css::Token::DIMENSION)
        hue = try$(parseValue<Angle>(scan)).toDegree();
    else if (scan.peek() == Css::Token::Type::NUMBER)
        hue = try$(parseValue<Number>(scan));
    else
        return Error::invalidData("expected hue");

    eatWhitespace(scan);

    if (not scan.skip(Css::Token::COMMA))
        return Error::invalidData("comma is expected separating hsl components");
    eatWhitespace(scan);

    auto sat = try$(parseValue<Percent>(scan)).value() / 100.0f;
    eatWhitespace(scan);

    if (not scan.skip(Css::Token::COMMA))
        return Error::invalidData("comma is expected separating hsl components");
    eatWhitespace(scan);

    auto l = try$(parseValue<Percent>(scan)).value() / 100.0f;
    eatWhitespace(scan);

    if (scan.ended()) {
        auto color = Gfx::hslToRgb(Gfx::Hsl{hue, sat, l});
        return Ok(color);
    }

    if (scan.skip(Css::Token::COMMA)) {
        eatWhitespace(scan);
        auto alpha = try$(_parseAlphaValue(scan));
        auto color = Gfx::hslToRgb(Gfx::Hsl{hue, sat, l});
        return Ok(color.withAlpha(alpha));
    }

    return Error::invalidData("expected comma before alpha channel");
}

static Res<Gfx::Color> _parseFuncColor(Css::Sst const& s) {
    if (s.prefix == Css::Token::function("color(")) {
        return _parseColorFuncColor(s);
    } else if (s.prefix == Css::Token::function("rgb(") or s.prefix == Css::Token::function("rgba(")) {
        auto modernColor = _parseSRGBModernFuncColor(s);
        if (modernColor)
            return modernColor;

        return _parseSRGBLegacyFuncColor(s);
    } else if (s.prefix == Css::Token::function("hsl(") or s.prefix == Css::Token::function("hsla(")) {
        auto modernColor = _parseHSLModernFuncColor(s);
        if (modernColor)
            return modernColor;

        return _parseHSLLegacyFuncColor(s);
    } else {
        return Error::invalidData("unknown color function");
    }
}

static Res<ColorMix::Side> _parseColorMixSide(Cursor<Css::Sst>& s) {
    Opt<Percent> percent;

    if (s.ended())
        return Error::invalidData("unexpected end of input");

    if (s.peek() == Css::Token::Type::PERCENTAGE) {
        percent = try$(parseValue<Percent>(s));
        eatWhitespace(s);
        return Ok(ColorMix::Side{
            try$(parseValue<Color>(s)),
            percent,
        });
    }

    Color color = try$(parseValue<Color>(s));
    eatWhitespace(s);
    if (not s.ended() and s.peek() == Css::Token::Type::PERCENTAGE)
        percent = try$(parseValue<Percent>(s)).value();

    return Ok(ColorMix::Side{
        std::move(color),
        percent,
    });
}

static Res<Color> _parseColorMixFunc(Css::Sst const& s) {
    Cursor<Css::Sst> scan = s.content;

    eatWhitespace(scan);
    if (not scan.skip(Css::Token::ident("in"))) // FIXME: correct token class?
        return Error::invalidData("expected 'in' keyword");

    eatWhitespace(scan);

    if (scan.ended() or not(scan.peek() == Css::Token::IDENT))
        return Error::invalidData("expected color-space identifier");

    auto colorSpace = scan.next().token.data;
    // FIXME: not parsing interpolation method for polar spaces

    eatWhitespace(scan);

    if (not scan.skip(Css::Token::COMMA))
        return Error::invalidData("expected comma separting color mix arguments");

    eatWhitespace(scan);
    auto lhs = try$(_parseColorMixSide(scan));
    eatWhitespace(scan);
    if (not scan.skip(Css::Token::Type::COMMA))
        return Error::invalidData("expected comma");

    auto rhs = try$(_parseColorMixSide(scan));

    return Ok(makeBox<ColorMix>(ColorSpace::fromStr(colorSpace), std::move(lhs), std::move(rhs)));
}

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

template <>
struct ValueParser<Color> {
    static Res<Color> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::HASH) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(try$(_parseHexColor(scan)));
        } else if (c.peek() == Css::Token::IDENT) {
            Str data = c->token.data.str();

            if (eqCi(data, "currentcolor"s)) {
                c.next();
                return Ok(Keywords::CURRENT_COLOR);
            }

            if (eqCi(data, "transparent"s)) {
                c.next();
                return Ok(TRANSPARENT);
            }

            auto maybeColor = parseNamedColor(data);
            if (maybeColor) {
                c.next();
                return Ok(maybeColor.unwrap());
            }

            auto maybeSystemColor = parseSystemColor(data);
            if (maybeSystemColor) {
                c.next();
                return Ok(maybeSystemColor.unwrap());
            }

        } else if (c.peek() == Css::Sst::FUNC) {

            if (c->prefix == Css::Token::function("color-mix(")) {
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

// MARK: Color Gamut
// https://drafts.csswg.org/mediaqueries/#color-gamut

template <>
struct ValueParser<ColorGamut> {
    static Res<ColorGamut> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("srgb")))
            return Ok(ColorGamut::SRGB);
        else if (c.skip(Css::Token::ident("p3")))
            return Ok(ColorGamut::DISPLAY_P3);
        else if (c.skip(Css::Token::ident("rec2020")))
            return Ok(ColorGamut::REC2020);
        else
            return Error::invalidData("expected color gamut");
    }
};

// MARK: Contrast
// https://drafts.csswg.org/mediaqueries/#contrast

template <>
struct ValueParser<Contrast> {
    static Res<Contrast> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("less")))
            return Ok(Contrast::LESS);
        else if (c.skip(Css::Token::ident("more")))
            return Ok(Contrast::MORE);
        else
            return Error::invalidData("expected contrast value");
    }
};

// MARK: Display
// https://drafts.csswg.org/css-display-3/#propdef-display

static Res<Display> _parseLegacyDisplay(Cursor<Css::Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Css::Token::ident("inline-block"))) {
        return Ok(Display{Display::FLOW_ROOT, Display::INLINE});
    } else if (c.skip(Css::Token::ident("inline-table"))) {
        return Ok(Display{Display::TABLE, Display::INLINE});
    } else if (c.skip(Css::Token::ident("inline-flex"))) {
        return Ok(Display{Display::FLEX, Display::INLINE});
    } else if (c.skip(Css::Token::ident("inline-grid"))) {
        return Ok(Display{Display::GRID, Display::INLINE});
    }

    return Error::invalidData("expected legacy display value");
}

static Res<Display> _parseBoxDisplay(Cursor<Css::Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Css::Token::ident("contents"))) {
        return Ok(Display::CONTENTS);
    } else if (c.skip(Css::Token::ident("none"))) {
        return Ok(Display::NONE);
    }

    return Error::invalidData("expected box value");
}

static Res<Display> _parseInternalDisplay(Cursor<Css::Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Css::Token::ident("table-row-group"))) {
        return Ok(Display::TABLE_ROW_GROUP);
    } else if (c.skip(Css::Token::ident("table-header-group"))) {
        return Ok(Display::TABLE_HEADER_GROUP);
    } else if (c.skip(Css::Token::ident("table-footer-group"))) {
        return Ok(Display::TABLE_FOOTER_GROUP);
    } else if (c.skip(Css::Token::ident("table-row"))) {
        return Ok(Display::TABLE_ROW);
    } else if (c.skip(Css::Token::ident("table-cell"))) {
        return Ok(Display::TABLE_CELL);
    } else if (c.skip(Css::Token::ident("table-column-group"))) {
        return Ok(Display::TABLE_COLUMN_GROUP);
    } else if (c.skip(Css::Token::ident("table-column"))) {
        return Ok(Display::TABLE_COLUMN);
    } else if (c.skip(Css::Token::ident("table-caption"))) {
        return Ok(Display::TABLE_CAPTION);
    } else if (c.skip(Css::Token::ident("ruby-base"))) {
        return Ok(Display::RUBY_BASE);
    } else if (c.skip(Css::Token::ident("ruby-text"))) {
        return Ok(Display::RUBY_TEXT);
    } else if (c.skip(Css::Token::ident("ruby-base-container"))) {
        return Ok(Display::RUBY_BASE_CONTAINER);
    } else if (c.skip(Css::Token::ident("ruby-text-container"))) {
        return Ok(Display::RUBY_TEXT_CONTAINER);
    }

    return Error::invalidData("expected internal value");
}

static Res<Display::Inside> _parseInsideDisplay(Cursor<Css::Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Css::Token::ident("flow"))) {
        return Ok(Display::FLOW);
    } else if (c.skip(Css::Token::ident("flow-root"))) {
        return Ok(Display::FLOW_ROOT);
    } else if (c.skip(Css::Token::ident("flex"))) {
        return Ok(Display::FLEX);
    } else if (c.skip(Css::Token::ident("grid"))) {
        return Ok(Display::GRID);
    } else if (c.skip(Css::Token::ident("ruby"))) {
        return Ok(Display::RUBY);
    } else if (c.skip(Css::Token::ident("table"))) {
        return Ok(Display::TABLE);
    } else if (c.skip(Css::Token::ident("math"))) {
        return Ok(Display::MATH);
    }

    return Error::invalidData("expected inside value");
}

static Res<Display::Outside> _parseOutsideDisplay(Cursor<Css::Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.skip(Css::Token::ident("block"))) {
        return Ok(Display::BLOCK);
    } else if (c.skip(Css::Token::ident("inline"))) {
        return Ok(Display::INLINE);
    } else if (c.skip(Css::Token::ident("run-in"))) {
        return Ok(Display::RUN_IN);
    }

    return Error::invalidData("expected outside value");
}

template <>
struct ValueParser<Display> {
    static Res<Display> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (auto legacy = _parseLegacyDisplay(c))
            return legacy;

        if (auto box = _parseBoxDisplay(c))
            return box;

        if (auto internal = _parseInternalDisplay(c))
            return internal;

        auto inside = _parseInsideDisplay(c);
        auto outside = _parseOutsideDisplay(c);
        auto item = c.skip(Css::Token::ident("list-item"))
                        ? Display::Item::YES
                        : Display::Item::NO;

        if (not inside and not outside and not bool(item))
            return Error::invalidData("expected display value");

        return Ok(Display{
            inside.unwrapOr(Display::FLOW),
            outside.unwrapOr(Display::BLOCK),
            item,
        });
    }
};

// MARK: CaptionSide
// https://www.w3.org/TR/CSS21/tables.html#caption-position

template <>
struct ValueParser<CaptionSide> {
    static Res<CaptionSide> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("top"))) {
            return Ok(CaptionSide::TOP);
        } else if (c.skip(Css::Token::ident("bottom"))) {
            return Ok(CaptionSide::BOTTOM);
        }

        return Error::invalidData("expected caption side value");
    }
};

// MARK: FitContent
// https://drafts.csswg.org/css-sizing-3/#preferred-size-properties

template <>
struct ValueParser<FitContent> {
    static Res<FitContent> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("fit-content(")) {
            FitContent result;
            Cursor<Css::Sst> scan = c->content;
            result.value = try$(parseValue<PercentOr<Length>>(scan));
            return Ok(result);
        }
        return Error::invalidData("invalid fit-content");
    }
};

// MARK: FlexDirection
// https://drafts.csswg.org/css-flexbox-1/#flex-direction-property

template <>
struct ValueParser<FlexDirection> {
    static Res<FlexDirection> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("row")))
            return Ok(FlexDirection::ROW);
        else if (c.skip(Css::Token::ident("row-reverse")))
            return Ok(FlexDirection::ROW_REVERSE);
        else if (c.skip(Css::Token::ident("column")))
            return Ok(FlexDirection::COLUMN);
        else if (c.skip(Css::Token::ident("column-reverse")))
            return Ok(FlexDirection::COLUMN_REVERSE);
        else
            return Error::invalidData("expected flex direction");
    }
};

// MARK: FlexWrap
// https://drafts.csswg.org/css-flexbox-1/#flex-wrap-property

template <>
struct ValueParser<FlexWrap> {
    static Res<FlexWrap> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("nowrap")))
            return Ok(FlexWrap::NOWRAP);
        else if (c.skip(Css::Token::ident("wrap")))
            return Ok(FlexWrap::WRAP);
        else if (c.skip(Css::Token::ident("wrap-reverse")))
            return Ok(FlexWrap::WRAP_REVERSE);
        else
            return Error::invalidData("expected flex wrap");
    }
};

// MARK: FontSize
// https://www.w3.org/TR/css-fonts-4/#font-size-prop

template <>
struct ValueParser<FontSize> {
    static Res<FontSize> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("xx-small")))
            return Ok(FontSize::XX_SMALL);
        else if (c.skip(Css::Token::ident("x-small")))
            return Ok(FontSize::X_SMALL);
        else if (c.skip(Css::Token::ident("small")))
            return Ok(FontSize::SMALL);
        else if (c.skip(Css::Token::ident("medium")))
            return Ok(FontSize::MEDIUM);
        else if (c.skip(Css::Token::ident("large")))
            return Ok(FontSize::LARGE);
        else if (c.skip(Css::Token::ident("x-large")))
            return Ok(FontSize::X_LARGE);
        else if (c.skip(Css::Token::ident("xx-large")))
            return Ok(FontSize::XX_LARGE);
        else if (c.skip(Css::Token::ident("smaller")))
            return Ok(FontSize::SMALLER);
        else if (c.skip(Css::Token::ident("larger")))
            return Ok(FontSize::LARGER);
        else
            return Ok(try$(parseValue<PercentOr<Length>>(c)));
    }
};

// MARK: FontStyle
// https://drafts.csswg.org/css-fonts-4/#propdef-font-style

template <>
struct ValueParser<FontStyle> {
    static Res<FontStyle> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("normal"))) {
            return Ok(FontStyle::NORMAL);
        } else if (c.skip(Css::Token::ident("italic"))) {
            return Ok(FontStyle::ITALIC);
        } else if (c.skip(Css::Token::ident("oblique"))) {
            auto angle = parseValue<Angle>(c);
            if (angle)
                return Ok(angle.unwrap());

            return Ok(FontStyle::OBLIQUE);
        }

        return Error::invalidData("expected font style");
    }
};

// MARK: FontWeight
// https://www.w3.org/TR/css-fonts-4/#font-weight-absolute-values

template <>
struct ValueParser<FontWeight> {
    static Res<FontWeight> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("normal"))) {
            return Ok(Text::FontWeight::REGULAR);
        } else if (c.skip(Css::Token::ident("bold"))) {
            return Ok(Text::FontWeight::BOLD);
        } else if (c.skip(Css::Token::ident("bolder"))) {
            return Ok(RelativeFontWeight::BOLDER);
        } else if (c.skip(Css::Token::ident("lighter"))) {
            return Ok(RelativeFontWeight::LIGHTER);
        } else {
            return Ok(Text::FontWeight{static_cast<u16>(clamp(try$(parseValue<Integer>(c)), 0, 1000))});
        }
    }
};

// MARK: FontWidth
// https://www.w3.org/TR/css-fonts-4/#propdef-font-width

template <>
struct ValueParser<FontWidth> {
    static Res<FontWidth> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("normal"))) {
            return Ok(FontWidth::NORMAL);
        } else if (c.skip(Css::Token::ident("condensed"))) {
            return Ok(FontWidth::CONDENSED);
        } else if (c.skip(Css::Token::ident("expanded"))) {
            return Ok(FontWidth::EXPANDED);
        } else if (c.skip(Css::Token::ident("ultra-condensed"))) {
            return Ok(FontWidth::ULTRA_CONDENSED);
        } else if (c.skip(Css::Token::ident("extra-condensed"))) {
            return Ok(FontWidth::EXTRA_CONDENSED);
        } else if (c.skip(Css::Token::ident("semi-condensed"))) {
            return Ok(FontWidth::SEMI_CONDENSED);
        } else if (c.skip(Css::Token::ident("semi-expanded"))) {
            return Ok(FontWidth::SEMI_EXPANDED);
        } else if (c.skip(Css::Token::ident("extra-expanded"))) {
            return Ok(FontWidth::EXTRA_EXPANDED);
        } else if (c.skip(Css::Token::ident("ultra-expanded"))) {
            return Ok(FontWidth::ULTRA_EXPANDED);
        }

        return Ok(try$(parseValue<Percent>(c)));
    }
};

template <>
struct ValueParser<Text::Family> {
    static Res<Text::Family> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::STRING)
            return Ok(Text::Family::parse(try$(parseValue<String>(c))));

        StringBuilder familyStrBuilder;
        usize amountOfIdents = 0;

        if (c.peek() == Css::Token::IDENT) {
            while (not c.ended() and c.peek() == Css::Token::IDENT) {
                if (++amountOfIdents > 1)
                    familyStrBuilder.append(' ');
                familyStrBuilder.append(c.next().token.data);
                eatWhitespace(c);
            }
        } else {
            return Error::invalidData("expected font family name");
        }

        if (amountOfIdents > 1)
            return Ok(familyStrBuilder.take());

        return Ok(Text::Family::parse(familyStrBuilder.take()));
    }
};

template <>
struct ValueParser<Float> {
    static Res<Float> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none"))) {
            return Ok(Float::NONE);
        } else if (c.skip(Css::Token::ident("inline-start"))) {
            return Ok(Float::INLINE_START);
        } else if (c.skip(Css::Token::ident("inline-end"))) {
            return Ok(Float::INLINE_END);
        } else if (c.skip(Css::Token::ident("left"))) {
            return Ok(Float::LEFT);
        } else if (c.skip(Css::Token::ident("right"))) {
            return Ok(Float::RIGHT);
        }

        return Error::invalidData("expected float");
    }
};

// MARK: Clear & Float
//

template <>
struct ValueParser<Clear> {
    static Res<Clear> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none"))) {
            return Ok(Clear::NONE);
        } else if (c.skip(Css::Token::ident("left"))) {
            return Ok(Clear::LEFT);
        } else if (c.skip(Css::Token::ident("right"))) {
            return Ok(Clear::RIGHT);
        } else if (c.skip(Css::Token::ident("both"))) {
            return Ok(Clear::BOTH);
        } else if (c.skip(Css::Token::ident("inline-start"))) {
            return Ok(Clear::INLINE_START);
        } else if (c.skip(Css::Token::ident("inline-end"))) {
            return Ok(Clear::INLINE_END);
        }

        return Error::invalidData("expected clear");
    }
};

// MARK: Hover
// https://drafts.csswg.org/mediaqueries/#hover

template <>
struct ValueParser<Hover> {
    static Res<Hover> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok(Hover::NONE);
        else if (c.skip(Css::Token::ident("hover")))
            return Ok(Hover::HOVER);
        else
            return Error::invalidData("expected hover value");
    }
};

// MARK: Integer
// https://drafts.csswg.org/css-values/#integers

template <>
struct ValueParser<Integer> {
    static Res<Integer> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::NUMBER) {
            Io::SScan scan = c->token.data.str();
            c.next();
            Integer result = try$(Io::atoi(scan));
            if (scan.ended())
                return Ok(result);
        }

        return Error::invalidData("expected integer");
    }
};

template <typename T>
struct ValueParser<Math::Insets<T>> {
    static Res<Math::Insets<T>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto top = parseValue<T>(c);
        if (not top)
            return top.none();

        auto right = parseValue<T>(c);
        if (not right)
            return Ok(Math::Insets<T>{top.take()});

        auto bottom = parseValue<T>(c);
        if (not bottom)
            return Ok(Math::Insets<T>{top.take(), right.take()});

        auto left = parseValue<T>(c);
        if (not left)
            return Ok(Math::Insets<T>{top.take(), right.take(), bottom.take()});

        return Ok(Math::Insets<T>{top.take(), right.take(), bottom.take(), left.take()});
    }
};

template <StrLit K>
struct ValueParser<Keyword<K>> {
    static Res<Keyword<K>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->token == Css::Token::ident(K)) {
            c.next();
            return Ok(Keyword<K>{});
        }

        return Error::invalidData("expected keyword");
    }
};

template <typename T>
concept ValueParseable = requires(T a, Cursor<Css::Sst> c) {
    parseValue<T>(c);
};

template <ValueParseable... Ts>
struct ValueParser<Union<Ts...>> {
    static Res<Union<Ts...>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        return Meta::any<Ts...>([&c]<typename T>() -> Res<Union<Ts...>> {
            return Ok(try$(parseValue<T>(c)));
        });
    }
};

// MARK: Length
// https://drafts.csswg.org/css-values/#lengths

static Res<Length::Unit> _parseLengthUnit(Str unit) {
#define LENGTH(NAME, ...)      \
    if (eqCi(unit, #NAME ""s)) \
        return Ok(Length::Unit::NAME);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"
#include <vaev-base/defs/lengths.inc>
#pragma clang diagnostic pop
#undef LENGTH

    logWarn("unknown length unit: {}", unit);
    return Error::invalidData("unknown length unit");
}

template <>
struct ValueParser<Length> {
    static Res<Length> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::DIMENSION) {
            Io::SScan scan = c->token.data.str();
            auto value = Io::atof(scan, {.allowExp = false}).unwrapOr(0.0);
            auto unit = try$(_parseLengthUnit(scan.remStr()));
            c.next();

            return Ok(Length{value, unit});
        } else if (c.skip(Css::Token::number("0"))) {
            return Ok(Length{0.0, Length::Unit::PX});
        }

        return Error::invalidData("expected length");
    }
};

template <>
struct ValueParser<LineHeight> {
    static Res<LineHeight> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("normal"))) {
            return Ok(LineHeight::NORMAL);
        }

        {
            auto rb = c.rollbackPoint();
            auto maybeNumber = parseValue<Number>(c);
            if (maybeNumber) {
                rb.disarm();
                return Ok(maybeNumber.unwrap());
            }
        }

        return Ok(LineHeight{try$(parseValue<PercentOr<Length>>(c))});
    }
};

// MARK: MediaType
// https://drafts.csswg.org/mediaqueries/#media-types

template <>
struct ValueParser<MediaType> {
    static Res<MediaType> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("all")))
            return Ok(MediaType::ALL);

        if (c.skip(Css::Token::ident("print")))
            return Ok(MediaType::PRINT);

        if (c.skip(Css::Token::ident("screen")))
            return Ok(MediaType::SCREEN);

        // NOTE: The spec says that we should reconize the following media types
        //       but they should not match anything
        static constexpr Array OTHER = {
            "tty",
            "tv",
            "projection",
            "handheld",
            "braille",
            "embossed",
            "speech",
            "aural",
        };

        for (auto const& item : OTHER) {
            if (c.skip(Css::Token::ident(item)))
                return Ok(MediaType::OTHER);
        }
        return Error::invalidData("expected media type");
    }
};

// MARK: Number
// https://drafts.csswg.org/css-values/#numbers

template <>
struct ValueParser<Number> {
    static Res<Number> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::NUMBER) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(try$(Io::atof(scan)));
        }

        return Error::invalidData("expected number");
    }
};

// MARK: Orientation
// https://drafts.csswg.org/mediaqueries/#orientation

template <>
struct ValueParser<Print::Orientation> {
    static Res<Print::Orientation> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("portrait")))
            return Ok(Print::Orientation::PORTRAIT);
        else if (c.skip(Css::Token::ident("landscape")))
            return Ok(Print::Orientation::LANDSCAPE);
        else
            return Error::invalidData("expected orientation");
    }
};

// MARK: Overflow
// https://www.w3.org/TR/css-overflow/#overflow-control

template <>
struct ValueParser<Overflow> {
    static Res<Overflow> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("overlay")))
            // https://www.w3.org/TR/css-overflow/#valdef-overflow-overlay
            return Ok(Overflow::AUTO);
        else if (c.skip(Css::Token::ident("visible")))
            return Ok(Overflow::VISIBLE);
        else if (c.skip(Css::Token::ident("hidden")))
            return Ok(Overflow::HIDDEN);
        else if (c.skip(Css::Token::ident("scroll")))
            return Ok(Overflow::SCROLL);
        else if (c.skip(Css::Token::ident("auto")))
            return Ok(Overflow::AUTO);
        else
            return Error::invalidData("expected overflow value");
    }
};

// MARK: OverflowBlock
// https://drafts.csswg.org/mediaqueries/#mf-overflow-block

template <>
struct ValueParser<OverflowBlock> {
    static Res<OverflowBlock> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok(OverflowBlock::NONE);
        else if (c.skip(Css::Token::ident("scroll")))
            return Ok(OverflowBlock::SCROLL);
        else if (c.skip(Css::Token::ident("paged")))
            return Ok(OverflowBlock::PAGED);
        else
            return Error::invalidData("expected overflow block value");
    }
};

// MARK: OverflowInline
// https://drafts.csswg.org/mediaqueries/#mf-overflow-inline

template <>
struct ValueParser<OverflowInline> {
    static Res<OverflowInline> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok(OverflowInline::NONE);
        else if (c.skip(Css::Token::ident("scroll")))
            return Ok(OverflowInline::SCROLL);
        else
            return Error::invalidData("expected overflow inline value");
    }
};

// MARK: Percentage
// https://drafts.csswg.org/css-values/#percentages

template <>
struct ValueParser<Percent> {
    static Res<Percent> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::PERCENTAGE) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(Percent{Io::atof(scan).unwrapOr(0.0)});
        }

        return Error::invalidData("expected percentage");
    }
};

// MARK: Pointer
// https://drafts.csswg.org/mediaqueries/#pointer

template <>
struct ValueParser<Pointer> {
    static Res<Pointer> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok(Pointer::NONE);
        else if (c.skip(Css::Token::ident("coarse")))
            return Ok(Pointer::COARSE);
        else if (c.skip(Css::Token::ident("fine")))
            return Ok(Pointer::FINE);
        else
            return Error::invalidData("expected pointer value");
    }
};

// MARK: Position
// https://drafts.csswg.org/css-position-3/#propdef-position

template <>
struct ValueParser<Position> {
    static Res<Position> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("static")))
            return Ok(Position::STATIC);
        else if (c.skip(Css::Token::ident("relative")))
            return Ok(Position::RELATIVE);
        else if (c.skip(Css::Token::ident("absolute")))
            return Ok(Position::ABSOLUTE);
        else if (c.skip(Css::Token::ident("fixed")))
            return Ok(Position::FIXED);
        else if (c.skip(Css::Token::ident("sticky")))
            return Ok(Position::STICKY);
        else
            return Error::invalidData("expected position");
    }
};

// MARK: Resolution
// https://drafts.csswg.org/css-values/#resolution

static Res<Resolution::Unit> _parseResolutionUnit(Str unit) {
    if (eqCi(unit, "dpi"s))
        return Ok(Resolution::Unit::DPI);
    else if (eqCi(unit, "dpcm"s))
        return Ok(Resolution::Unit::DPCM);
    else if (eqCi(unit, "dppx"s) or eqCi(unit, "x"s))
        return Ok(Resolution::Unit::DPPX);
    else
        return Error::invalidData("unknown resolution unit");
}

template <>
struct ValueParser<Resolution> {
    static Res<Resolution> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::DIMENSION) {
            Io::SScan scan = c->token.data.str();
            auto value = Io::atof(scan).unwrapOr(0.0);
            auto unit = try$(_parseResolutionUnit(scan.remStr()));
            return Ok(Resolution{value, unit});
        }

        return Error::invalidData("expected resolution");
    }
};

// MARK: Scan
// https://drafts.csswg.org/mediaqueries/#scan

template <>
struct ValueParser<Scan> {
    static Res<Scan> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("interlace")))
            return Ok(Scan::INTERLACE);
        else if (c.skip(Css::Token::ident("progressive")))
            return Ok(Scan::PROGRESSIVE);
        else
            return Error::invalidData("expected scan value");
    }
};

// MARK: String
// https://drafts.csswg.org/css-values/#strings

template <>
struct ValueParser<String> {
    static Res<String> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::STRING) {
            // TODO: Handle escape sequences
            Io::SScan s = c.next().token.data.str();
            StringBuilder sb{s.rem()};
            auto quote = s.next();
            while (not s.skip(quote) and not s.ended()) {
                if (s.skip('\\') and not s.ended()) {
                    if (s.skip('\\'))
                        sb.append(s.next());
                } else {
                    sb.append(s.next());
                }
            }
            return Ok(sb.take());
        }

        return Error::invalidData("expected string");
    }
};

// MARK: Update
// https://drafts.csswg.org/mediaqueries/#update

template <>
struct ValueParser<Update> {
    static Res<Update> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("none")))
            return Ok(Update::NONE);
        else if (c.skip(Css::Token::ident("slow")))
            return Ok(Update::SLOW);
        else if (c.skip(Css::Token::ident("fast")))
            return Ok(Update::FAST);
        else
            return Error::invalidData("expected update value");
    }
};

template <typename T>
struct ValueParser<Math::Radii<T>> {

    static Res<Math::Radii<T>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto value1 = parseValue<PercentOr<Length>>(c);
        if (not value1)
            return Ok(parsePostSlash(c, Math::Radii<T>{Length{}}));

        auto value2 = parseValue<PercentOr<Length>>(c);
        if (not value2)
            return Ok(parsePostSlash(c, Math::Radii<T>{value1.take()}));

        auto value3 = parseValue<PercentOr<Length>>(c);
        if (not value3)
            return Ok(parsePostSlash(c, Math::Radii<T>{value1.take(), value2.take()}));

        auto value4 = parseValue<PercentOr<Length>>(c);
        if (not value4)
            return Ok(parsePostSlash(c, Math::Radii<T>{value1.take(), value2.take(), value3.take(), value2.take()}));

        return Ok(parsePostSlash(c, Math::Radii<T>{value1.take(), value2.take(), value3.take(), value4.take()}));
    }

    static Math::Radii<T> parsePostSlash(Cursor<Css::Sst>& c, Math::Radii<T> radii) {
        // if parse a /
        // 1 value-- > border all(a, d, e, h)
        // 2 values-- > 1 = top - start + bottom - end 2 = the others
        // 3 values-- > 1 = top - start, 2 = top - end + bottom - start, 3 = bottom - end
        // 4 values-- > 1 = top - start, 2 = top - end 3 = bottom - end, 4 = bottom - start
        if (not c.ended() and c.peek().token.data == "/"s) {
            c.next();
            auto value1 = parseValue<PercentOr<Length>>(c);
            if (not value1) {
                return radii;
            }

            auto value2 = parseValue<PercentOr<Length>>(c);
            if (not value2) {
                radii.a = value1.take();
                radii.d = value1.take();
                radii.e = value1.take();
                radii.h = value1.take();
                return radii;
            }

            auto value3 = parseValue<PercentOr<Length>>(c);
            if (not value3) {
                radii.a = value1.take();
                radii.d = value2.take();
                radii.e = value1.take();
                radii.h = value2.take();
                return radii;
            }

            auto value4 = parseValue<PercentOr<Length>>(c);
            if (not value4) {
                radii.a = value1.take();
                radii.d = value2.take();
                radii.e = value3.take();
                radii.h = value2.take();

                return radii;
            }

            radii.a = value1.take();
            radii.d = value2.take();
            radii.e = value3.take();
            radii.h = value4.take();
            return radii;
        }

        return radii;
    }
};

// MARK: ReducedMotion
// https://drafts.csswg.org/mediaqueries/#reduced-motion

template <>
struct ValueParser<ReducedMotion> {
    static Res<ReducedMotion> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("no-preference")))
            return Ok(ReducedMotion::NO_PREFERENCE);
        else if (c.skip(Css::Token::ident("reduce")))
            return Ok(ReducedMotion::REDUCE);
        else
            return Error::invalidData("expected reduced motion value");
    }
};

// MARK: ReducedTransparency
// https://drafts.csswg.org/mediaqueries/#reduced-transparency

template <>
struct ValueParser<ReducedTransparency> {
    static Res<ReducedTransparency> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("no-preference")))
            return Ok(ReducedTransparency::NO_PREFERENCE);
        else if (c.skip(Css::Token::ident("reduce")))
            return Ok(ReducedTransparency::REDUCE);
        else
            return Error::invalidData("expected reduced transparency value");
    }
};

// MARK: ReducedData
// https://drafts.csswg.org/mediaqueries/#reduced-data

template <>
struct ValueParser<ReducedData> {
    static Res<ReducedData> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("no-preference")))
            return Ok(ReducedData::NO_PREFERENCE);
        else if (c.skip(Css::Token::ident("reduce")))
            return Ok(ReducedData::REDUCE);
        else
            return Error::invalidData("expected reduced data value");
    }
};

// MARK: TableLayout
// https://www.w3.org/TR/CSS21/tables.html#propdef-table-layout

template <>
struct ValueParser<TableLayout> {
    static Res<TableLayout> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("auto"))) {
            return Ok(TableLayout::AUTO);
        } else if (c.skip(Css::Token::ident("fixed"))) {
            return Ok(TableLayout::FIXED);
        }

        return Error::invalidData("expected table layout value");
    }
};

// MARK: Url
// https://www.w3.org/TR/css-values-4/#urls

template <>
struct ValueParser<Mime::Url> {
    static Res<Mime::Url> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::URL) {
            auto urlSize = c.peek().token.data.len() - 5; // "url()" takes 5 chars
            auto urlValue = sub(c.next().token.data, Range<usize>{4u, urlSize});
            return Ok(urlValue);
        } else if (c.peek() != Css::Sst::FUNC or c.peek().prefix != Css::Token::function("url("))
            return Error::invalidData("expected url function");

        auto urlFunc = c.next();
        Cursor<Css::Sst> scanUrl{urlFunc.content};
        eatWhitespace(scanUrl);

        if (scanUrl.ended() or not(scanUrl.peek() == Css::Token::STRING))
            return Error::invalidData("expected base url string");

        auto url = Mime::parseUrlOrPath(try$(parseValue<String>(scanUrl)), NONE);

        // TODO: it is unclear what url-modifiers are and how they are used

        return Ok(url);
    }
};

} // namespace Vaev::Style
