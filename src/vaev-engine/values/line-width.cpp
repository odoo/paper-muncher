module;

#include <karm/macros>

export module Vaev.Engine:values.lineWidth;

import Karm.Math;

import :css;
import :values.base;
import :values.calc2;
import :values.keywords;
import :values.length;
import :values.unitValue;

using namespace Vaev::Literals;

namespace Vaev::Css {

// MARK: Line Width -------------------------------------------------------------
// https://drafts.csswg.org/css-backgrounds/#typedef-line-width

export using LineWidth = Union<
    Keywords::Thin,
    Keywords::Medium,
    Keywords::Thick,
    Length>;

} // namespace Vaev::Css

namespace Vaev::Style {

export Pixel resolve(Css::LineWidth const& value, Css::LengthContext auto const& ctx) {
    return value.visit(
        [](Keywords::Thin const&) {
            return 1_px;
        },
        [](Keywords::Medium const&) {
            return 3_px;
        },
        [](Keywords::Thick const&) {
            return 5_px;
        },
        [&](Css::Length const& length) {
            return resolve(length, ctx);
        }
    );
}

} // namespace Vaev::Style

namespace Vaev::Css {

export template <>
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

        return Ok(try$(parseValue<Length>(c)));
    }
};

} // namespace Vaev::Css

