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

// https://drafts.csswg.org/css-fonts-4/#font-width-prop
export using FontWidth = Union<
    Keywords::Normal,
    Percentage,
    Keywords::UltraCondensed,
    Keywords::ExtraCondensed,
    Keywords::Condensed,
    Keywords::SemiCondensed,
    Keywords::SemiExpanded,
    Keywords::Expanded,
    Keywords::ExtraExpanded,
    Keywords::UltraExpanded>;

// https://drafts.csswg.org/css-fonts-4/#font-style-prop
export using FontStyle = Union<
    Keywords::Normal,
    Keywords::Italic,
    Keywords::Left,
    Keywords::Right,
    Opt<Angle>>;

export template <>
struct ValueParser<FontStyle> {
    using FontStyleKeyword = Union<
        Keywords::Normal,
        Keywords::Italic,
        Keywords::Left,
        Keywords::Right,
        Keywords::Left,
        Keywords::Oblique>;

    static Res<FontStyle> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto keyword = try$(parseValue<FontStyleKeyword>(c));

        return Ok(keyword.visit(
            [&](Keywords::Oblique) -> FontStyle {
                return parseValue<Angle>(c).ok();
            },
            [](auto const& other) -> FontStyle {
                return other;
            }
        ));
    }
};

export template <>
struct ValueSerializer<FontStyle> {
    static Res<> serialize(Io::TextWriter& w, FontStyle const& value) {
        return value.visit(
            [&](Opt<Angle> const& angle) -> Res<> {
                if (not angle)
                    return w.writeStr("oblique"s);

                // https://drafts.csswg.org/cssom/#serialize-a-css-value
                // If component values can be omitted or replaced with a shorter representation
                // without changing the meaning of the value, omit/replace them.
                // oblique 0deg is normal.
                auto plain = angle->calcTree().is<UnitValue>();
                if (plain and plain->to(UnitValue::Unit::DEG) == 0.0)
                    return w.writeStr("normal"s);

                try$(w.writeStr("oblique "s));
                return serializeValue(w, *angle);
            },
            [&](auto const& keyword) -> Res<> {
                return serializeValue(w, keyword);
            }
        );
    }
};

// https://drafts.csswg.org/css-fonts-4/#font-weight-absolute-values
export using FontWeightAbsolute = Union<Keywords::Normal, Keywords::Bold, Number>;

// https://drafts.csswg.org/css-fonts-4/#propdef-font-weight
using FontWeight = Union<FontWeightAbsolute, Keywords::Lighter, Keywords::Bolder>;

// https://drafts.csswg.org/css-fonts/#font-family-name-value
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
