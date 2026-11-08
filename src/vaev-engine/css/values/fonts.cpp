module;

#include <karm/macros>

export module Vaev.Engine:css.values.fonts;

import Karm.Ref;
import Karm.Gfx;

import :css.lexer;
import :css.parser;
import :css.serializer;
import :css.values.base;
import :css.values.keywords;
import :css.values.primitives;
import :css.values.unitValue;
import :values.fonts;
import :values.primitives;

namespace Vaev::Css {

// MARK: FontWidth -------------------------------------------------------------
// https://www.w3.org/TR/css-fonts-4/#propdef-font-width

export struct FontWidth {
    enum struct _Named {
        ULTRA_CONDENSED = 500,
        EXTRA_CONDENSED = 625,
        CONDENSED = 750,
        SEMI_CONDENSED = 875,
        NORMAL = 1000,
        SEMI_EXPANDED = 1125,
        EXPANDED = 1250,
        EXTRA_EXPANDED = 1500,
        ULTRA_EXPANDED = 2000,

        _LEN,
    };

    static PurePercentage toPercent(_Named stretch) {
        return PurePercentage{static_cast<int>(stretch) / 10.0};
    }

    using enum _Named;

    PurePercentage _val;

    constexpr FontWidth()
        : FontWidth(NORMAL) {
    }

    constexpr FontWidth(_Named named)
        : _val(toPercent(named)) {
    }

    constexpr FontWidth(PurePercentage value)
        : _val(value) {
    }

    constexpr auto val() const {
        return _val;
    }

    void repr(Io::Emit& e) const {
        e("{}", _val);
    }

    bool operator==(FontWidth const& other) const {
        return _val == other._val;
    }

    auto operator<=>(FontWidth const& other) const {
        return _val <=> other._val;
    }
};

export template <>
struct ValueParser<FontWidth> {
    static Res<FontWidth> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("normal"))) {
            return Ok(FontWidth::NORMAL);
        } else if (c.skip(Token::ident("condensed"))) {
            return Ok(FontWidth::CONDENSED);
        } else if (c.skip(Token::ident("expanded"))) {
            return Ok(FontWidth::EXPANDED);
        } else if (c.skip(Token::ident("ultra-condensed"))) {
            return Ok(FontWidth::ULTRA_CONDENSED);
        } else if (c.skip(Token::ident("extra-condensed"))) {
            return Ok(FontWidth::EXTRA_CONDENSED);
        } else if (c.skip(Token::ident("semi-condensed"))) {
            return Ok(FontWidth::SEMI_CONDENSED);
        } else if (c.skip(Token::ident("semi-expanded"))) {
            return Ok(FontWidth::SEMI_EXPANDED);
        } else if (c.skip(Token::ident("extra-expanded"))) {
            return Ok(FontWidth::EXTRA_EXPANDED);
        } else if (c.skip(Token::ident("ultra-expanded"))) {
            return Ok(FontWidth::ULTRA_EXPANDED);
        }

        return Ok(try$(parseValue<PurePercentage>(c)));
    }
};

export template <>
struct ValueSerializer<FontWidth> {
    // https://drafts.csswg.org/css-fonts-4/#font-width-prop
    static Res<> serialize(Io::TextWriter& w, FontWidth const& value) {
        return serializeValue(w, value.val());
    }
};

// MARK: FontStyle -------------------------------------------------------------
// https://drafts.csswg.org/css-fonts-4/#propdef-font-style

export struct FontStyle {
    using enum Gfx::FontStyle;
    Gfx::FontStyle val;

    // The lack of an <angle> represents 14deg.
    PureAngle obliqueAngle = PureAngle::fromDegree(14.0);

    constexpr FontStyle(Gfx::FontStyle named = NORMAL)
        : val(named) {
    }

    constexpr FontStyle(PureAngle angle)
        : val(OBLIQUE), obliqueAngle(angle) {}

    void repr(Io::Emit& e) const {
        e("{}", val);
        if (val == OBLIQUE)
            e(" {}", obliqueAngle);
    }

    bool operator==(FontStyle const& other) const {
        return val == other.val and
               (val != OBLIQUE or obliqueAngle == other.obliqueAngle);
    }
};

export template <>
struct ValueParser<FontStyle> {
    static Res<FontStyle> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("normal"))) {
            return Ok(FontStyle::NORMAL);
        } else if (c.skip(Token::ident("italic"))) {
            return Ok(FontStyle::ITALIC);
        } else if (c.skip(Token::ident("oblique"))) {
            auto angle = parseValue<PureAngle>(c);
            if (angle)
                return Ok(angle.expect());

            return Ok(FontStyle::OBLIQUE);
        }

        return Error::invalidData("expected font style");
    }
};

export template <>
struct ValueSerializer<FontStyle> {
    // https://drafts.csswg.org/css-fonts-4/#font-style-prop
    static Res<> serialize(Io::TextWriter& w, FontStyle const& value) {
        // This represents an oblique value of "0".
        // https://drafts.csswg.org/cssom/#serialize-a-css-value
        // If component values can be omitted or replaced with a shorter representation
        // without changing the meaning of the value, omit/replace them.
        if (value.val == FontStyle::OBLIQUE and value.obliqueAngle.toDegree() == 0.0)
            return w.writeStr("normal"s);

        // the keyword specified, plus angle in degrees if specified
        try$(serializeValue(w, value.val));

        // The lack of an <angle> represents 14deg.
        if (value.val != FontStyle::OBLIQUE or value.obliqueAngle.toDegree() == 14.0)
            return Ok();

        try$(w.writeRune(' '));
        return serializeValue(w, PureAngle::fromDegree(value.obliqueAngle.toDegree()));
    }
};

// MARK: FontWeight
// https://www.w3.org/TR/css-fonts-4/#font-weight-absolute-values

export template <>
struct ValueParser<Gfx::FontWeight> {
    static Res<Gfx::FontWeight> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("normal"))) {
            return Ok(Gfx::FontWeight::REGULAR);
        } else if (c.skip(Token::ident("bold"))) {
            return Ok(Gfx::FontWeight::BOLD);
        } else {
            auto weight = try$(parseValue<Integer>(c));
            if (weight < 0 or weight > 1000)
                return Error::invalidData("font weight should be between 0 and 1000");
            return Ok(Gfx::FontWeight{static_cast<u16>(weight)});
        }
    }
};

export template <>
struct ValueSerializer<Gfx::FontWeight> {
    // https://drafts.csswg.org/css-fonts-4/#font-weight-prop
    static Res<> serialize(Io::TextWriter& w, Gfx::FontWeight const& value) {
        return serializeValue(w, static_cast<Integer>(value.value()));
    }
};

// MARK: FontFamily ------------------------------------------------------------

export template <>
struct ValueParser<FontFamily> {
    static Res<FontFamily> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Token::STRING)
            return Ok<FontFamily>(Symbol::from(try$(parseValue<String>(c))));

        StringBuilder familyStrBuilder;
        usize amountOfIdents = 0;

        if (c.peek() == Token::IDENT) {
            while (not c.ended() and c.peek() == Token::IDENT) {
                if (++amountOfIdents > 1)
                    familyStrBuilder.append(' ');
                familyStrBuilder.append(c.next().token.data);
                eatWhitespace(c);
            }
        } else {
            return Error::invalidData("expected font family name");
        }

        return Ok<FontFamily>(Symbol::from(familyStrBuilder.take()));
    }
};

} // namespace Vaev::Css
