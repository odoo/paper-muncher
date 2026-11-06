module;

#include <karm/macros>

export module Vaev.Engine:css.values.lineWidth;


import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.calc;
import :css.values.keywords;
import :css.values.primitives;
import :values.keywords;

namespace Vaev::Css {

// MARK: Line Width -------------------------------------------------------------
// https://drafts.csswg.org/css-backgrounds/#typedef-line-width

export using LineWidth = Union<
    Keywords::Thin,
    Keywords::Medium,
    Keywords::Thick,
    Length>;

export template <>
struct ValueParser<LineWidth> {
    static Res<LineWidth> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Token::ident("thin")) {
            c.next();
            return Ok(Keywords::THIN);
        }
        if (c.peek() == Token::ident("medium")) {
            c.next();
            return Ok(Keywords::MEDIUM);
        }
        if (c.peek() == Token::ident("thick")) {
            c.next();
            return Ok(Keywords::THICK);
        }

        return Ok(try$(parseValue<Length>(c)));
    }
};

} // namespace Vaev::Css
