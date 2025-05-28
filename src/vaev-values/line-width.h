#pragma once

#include "calc.h"
#include "keywords.h"
#include "length.h"

namespace Vaev {

// MARK: Line Width -------------------------------------------------------------
// https://drafts.csswg.org/css-backgrounds/#typedef-line-width

static constexpr Au THIN_VALUE = 1_au;
static constexpr Au MEDIUM_VALUE = 3_au;
static constexpr Au THICK_VALUE = 5_au;

using LineWidth = Union<
    Keywords::Thin,
    Keywords::Medium,
    Keywords::Thick,
    CalcValue<Length>>;

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

} // namespace Vaev
