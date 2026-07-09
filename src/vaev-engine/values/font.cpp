module;

#include <karm/macros>

export module Vaev.Engine:values.font;

import Karm.Ref;
import Karm.Gfx;

import :css;
import :values.base;
import :values.angle;
import :values.length;
import :values.percent;
import :values.primitives;
import :values.keywords;

using namespace Vaev::Literals;

namespace Vaev {

export enum struct FontDisplay {
    AUTO,
    BLOCK,
    SWAP,
    FALLBACK,
    OPTIONAL,

    _LEN,
};

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

    static Percent toPercent(_Named stretch) {
        return Percent{static_cast<int>(stretch) / 10.0};
    }

    using enum _Named;

    Percent _val;

    constexpr FontWidth()
        : FontWidth(NORMAL) {
    }

    constexpr FontWidth(_Named named)
        : _val(toPercent(named)) {
    }

    constexpr FontWidth(Percent value)
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

    Percent operator-(FontWidth const& other) const {
        return _val - other._val;
    }

    FontWidth operator+(Percent value) const {
        return FontWidth{_val + value};
    }
};

export template <>
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

// MARK: FontStyle -------------------------------------------------------------
// https://drafts.csswg.org/css-fonts-4/#propdef-font-style

export struct FontStyle {
    using enum Gfx::FontStyle;
    Gfx::FontStyle val;

    // The lack of an <angle> represents 14deg.
    Angle obliqueAngle = Angle(14.0, Angle::Unit::DEGREE);

    constexpr FontStyle(Gfx::FontStyle named = NORMAL)
        : val(named) {
    }

    constexpr FontStyle(Angle angle)
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

export enum struct RelativeFontWeight : u8 {
    LIGHTER,
    BOLDER,
};

using _FontWeight = Union<
    Gfx::FontWeight,
    RelativeFontWeight>;

export struct FontWeight : _FontWeight {
    using _FontWeight::_FontWeight;

    bool isRelative() const {
        return is<RelativeFontWeight>();
    }

    FontWeight()
        : FontWeight(Gfx::FontWeight::REGULAR) {
    }

    void repr(Io::Emit& e) const {
        e("{}", static_cast<Union<Gfx::FontWeight, RelativeFontWeight>>(*this));
    }

    Gfx::FontWeight resolve() const {
        if (is<RelativeFontWeight>())
            return Gfx::FontWeight::REGULAR;

        return unwrap<Gfx::FontWeight>();
    }

    Gfx::FontWeight resolve(Gfx::FontWeight const& parent) const {
        if (is<RelativeFontWeight>()) {
            if (unwrap<RelativeFontWeight>() == RelativeFontWeight::LIGHTER)
                return parent.lighter();
            return parent.bolder();
        }

        return unwrap<Gfx::FontWeight>();
    }
};

// MARK: FontWeight
// https://www.w3.org/TR/css-fonts-4/#font-weight-absolute-values

export template <>
struct ValueParser<Gfx::FontWeight> {
    static Res<Gfx::FontWeight> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("normal"))) {
            return Ok(Gfx::FontWeight::REGULAR);
        } else if (c.skip(Css::Token::ident("bold"))) {
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
struct ValueParser<FontWeight> {
    static Res<FontWeight> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("bolder"))) {
            return Ok(RelativeFontWeight::BOLDER);
        } else if (c.skip(Css::Token::ident("lighter"))) {
            return Ok(RelativeFontWeight::LIGHTER);
        } else {
            return Ok(try$(parseValue<Gfx::FontWeight>(c)));
        }
    }
};

// MARK: FontSize --------------------------------------------------------------
// https://www.w3.org/TR/css-fonts-4/#font-size-prop

using FontSize = Union<
    Length,
    Percent,
    Keywords::XxSmall,
    Keywords::XSmall,
    Keywords::Small,
    Keywords::Medium,
    Keywords::Large,
    Keywords::XLarge,
    Keywords::XxLarge,
    Keywords::Smaller,
    Keywords::Larger>;

export template <>
struct _Resolved<FontSize> {
    using Type = Au;
};

export struct FontSizeContextData {
    f64 userFontSize = 16;   /// Font size of the user agent
    f64 parentFontSize = 16; /// Font size of the parent box
};

export template <typename T>
concept FontSizeContext = LengthContext<T> and requires(T t) {
    { t.userFontSize } -> Meta::Convertible<f64>;
    { t.parentFontSize } -> Meta::Convertible<f64>;
};

Au resolve(FontSize const& value, FontSizeContext auto const& ctx) {
    return value.visit(
        [&](Length const& v) {
            return resolve(v, ctx);
        },
        [&](Percent const& v) {
            return Au::fromFloatNearest(ctx.parentFontSize * (v.value() / 100.0));
        },
        [&](Keywords::XxSmall) {
            return Au::fromFloatNearest(ctx.userFontSize * 0.5);
        },
        [&](Keywords::XSmall) {
            return Au::fromFloatNearest(ctx.userFontSize * 0.75);
        },
        [&](Keywords::Small) {
            return Au::fromFloatNearest(ctx.userFontSize * 0.875);
        },
        [&](Keywords::Medium) {
            return Au::fromFloatNearest(ctx.userFontSize);
        },
        [&](Keywords::Large) {
            return Au::fromFloatNearest(ctx.userFontSize * 1.125);
        },
        [&](Keywords::XLarge) {
            return Au::fromFloatNearest(ctx.userFontSize * 1.25);
        },
        [&](Keywords::XxLarge) {
            return Au::fromFloatNearest(ctx.userFontSize * 1.5);
        },
        [&](Keywords::Larger) {
            return Au::fromFloatNearest(ctx.parentFontSize * 1.25);
        },
        [&](Keywords::Smaller) {
            return Au::fromFloatNearest(ctx.parentFontSize * 0.875);
        }
    );
}

// MARK: FontFeature -----------------------------------------------------------

export using FontTag = Array<char, 4>;

export FontTag makeTag(Str tag) {
    if (tag.len() != 4)
        return {' ', ' ', ' ', ' '};
    return {tag[0], tag[1], tag[2], tag[3]};
}

export struct FontFeature {
    enum struct _Value {
        OFF,
        ON,
    };

    FontTag tag;
    isize val;

    FontFeature(Str tag, isize val)
        : tag(makeTag(tag)), val(val) {
    }

    FontFeature(Str tag, _Value val)
        : tag(makeTag(tag)), val(static_cast<isize>(val)) {
    }

    void repr(Io::Emit& e) const {
        e("(font-feature {} {})", tag, val);
    }

    bool operator==(FontFeature const& other) const {
        return tag == other.tag and val == other.val;
    }

    bool operator==(FontTag const& other) const {
        return tag == other;
    }

    bool operator==(_Value const& other) const {
        return val == static_cast<isize>(other);
    }
};

export struct FontVariation {
    FontTag tag;
    f64 val;

    FontVariation(Str tag, f64 val)
        : tag(makeTag(tag)), val(val) {
    }

    void repr(Io::Emit& e) const {
        e("(font-variation {} {})", tag, val);
    }

    bool operator==(FontTag const& other) const {
        return tag == other;
    }
};

struct FontFamily {
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

export struct FontSource {
    Union<Ref::Url, FontFamily> identifier;
    Opt<String> format;

    FontSource(Ref::Url url) : identifier(url) {}

    FontSource(FontFamily family) : identifier(family) {}

    void repr(Io::Emit& e) const {
        e("(font-source {}", identifier);
        if (format)
            e(" format({})", *format);
        e(")");
    }
};

// MARK: FontFamily ------------------------------------------------------------

export template <>
struct ValueParser<FontFamily> {
    static Res<FontFamily> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::STRING)
            return Ok<FontFamily>(Symbol::from(try$(parseValue<String>(c))));

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

        return Ok<FontFamily>(Symbol::from(familyStrBuilder.take()));
    }
};

} // namespace Vaev
