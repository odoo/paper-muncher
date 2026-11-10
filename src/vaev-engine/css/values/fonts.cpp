module;

#include <karm/macros>

export module Vaev.Engine:css.values.fonts;

import Karm.Gfx;

import :css.lexer;
import :css.parser;
import :css.serializer;
import :css.values.base;
import :css.values.keywords;
import :css.values.primitives;
import :css.values.numeric;

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

    static Percentage toPercent(_Named stretch) {
        return percent(static_cast<int>(stretch) / 10.0);
    }

    using enum _Named;

    Percentage _val;

    FontWidth()
        : FontWidth(NORMAL) {
    }

    FontWidth(_Named named)
        : _val(toPercent(named)) {
    }

    FontWidth(Percentage value)
        : _val(std::move(value)) {
    }

    Percentage const& val() const {
        return _val;
    }

    void repr(Io::Emit& e) const {
        e("{}", _val);
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

        return Ok(try$(parseValue<Percentage>(c)));
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

// normal | italic | oblique <angle [-90deg,90deg]>?
export struct FontStyle {
    using enum Gfx::FontStyle;
    Gfx::FontStyle val;

    // The angle of oblique, if specified. The lack of an <angle> represents 14deg.
    Opt<Angle> obliqueAngle = NONE;

    FontStyle(Gfx::FontStyle named = NORMAL)
        : val(named) {
    }

    FontStyle(Angle angle)
        : val(OBLIQUE), obliqueAngle(Some(std::move(angle))) {}

    void repr(Io::Emit& e) const {
        e("{}", val);
        if (obliqueAngle)
            e(" {}", obliqueAngle);
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
            eatWhitespace(c);
            auto angle = parseValue<Angle>(c);
            if (angle)
                return Ok(angle.take());

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
        if (value.obliqueAngle) {
            auto plain = value.obliqueAngle->calcTree().is<UnitValue>();
            if (plain and plain->to(UnitValue::Unit::DEG) == 0.0)
                return w.writeStr("normal"s);
        }

        try$(serializeValue(w, value.val));

        if (not value.obliqueAngle)
            return Ok();

        try$(w.writeRune(' '));
        return serializeValue(w, *value.obliqueAngle);
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

// https://www.w3.org/TR/css-fonts-4/#font-weight-prop
// <font-weight-absolute> | bolder | lighter
export enum struct RelativeFontWeight : u8 {
    LIGHTER,
    BOLDER,

    _LEN
};

export struct FontWeight : Union<Gfx::FontWeight, RelativeFontWeight> {
    using _Base = Union<Gfx::FontWeight, RelativeFontWeight>;
    using _Base::_Base;

    FontWeight()
        : FontWeight(Gfx::FontWeight::REGULAR) {}

    void repr(Io::Emit& e) const {
        e("{}", static_cast<_Base const&>(*this));
    }
};

export template <>
struct ValueParser<FontWeight> {
    static Res<FontWeight> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("bolder")))
            return Ok(RelativeFontWeight::BOLDER);
        if (c.skip(Token::ident("lighter")))
            return Ok(RelativeFontWeight::LIGHTER);
        return Ok(try$(parseValue<Gfx::FontWeight>(c)));
    }
};

export template <>
struct ValueSerializer<FontWeight> {
    static Res<> serialize(Io::TextWriter& w, FontWeight const& value) {
        return value.visit([&](auto const& v) {
            return serializeValue(w, v);
        });
    }
};

// MARK: FontFamily ------------------------------------------------------------

// https://drafts.csswg.org/css-fonts/#family-name-value
export struct FontFamily {
    Symbol name;

    FontFamily(Symbol name)
        : name(name) {
    }

    void repr(Io::Emit& e) const {
        e("(FontFamily name:{})", name);
    }

    bool operator==(FontFamily const&) const = default;
    auto operator<=>(FontFamily const&) const = default;
};

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
