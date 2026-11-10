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
// normal | <percentage [0,∞]> | ultra-condensed | extra-condensed | condensed | semi-condensed |
// semi-expanded | expanded | extra-expanded | ultra-expanded
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

// https://drafts.csswg.org/css-fonts-4/#font-weight-absolute-values
export using FontWeightAbsolute = Union<Keywords::Normal, Keywords::Bold, Number>;

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
