module;

#include <karm/macros>

export module Vaev.Engine:values.lineWidth;

import Karm.Math;

import :css;
import :values.base;
import :values.calc;
import :values.keywords;
import :values.length;
import :values.resolved;

using namespace Karm::Math::Literals;

namespace Vaev {

// MARK: Line Width -------------------------------------------------------------
// https://drafts.csswg.org/css-backgrounds/#typedef-line-width

export constexpr Au THIN_VALUE = 1_au;
export constexpr Au MEDIUM_VALUE = 3_au;
export constexpr Au THICK_VALUE = 5_au;

export using LineWidth = Union<
    Keywords::Thin,
    Keywords::Medium,
    Keywords::Thick,
    Calc<Length>>;

export Au resolve(LineWidth const& value, auto const& ctx) {
    return value.visit(
        [](Keywords::Thin const&) {
            return THIN_VALUE;
        },
        [](Keywords::Medium const&) {
            return MEDIUM_VALUE;
        },
        [](Keywords::Thick const&) {
            return THICK_VALUE;
        },
        [&](auto const& length) {
            return resolve(length, ctx);
        }
    );
}

// Computed value: absolute length
export template <>
struct _Computed<LineWidth> {
    using Type = Au;
};

export Au compute(LineWidth const& value, auto const& ctx) {
    return resolve(value, ctx);
}

export template <>
struct _Specify<LineWidth> {
    static LineWidth from(Au value) {
        return Calc<Length>{Length{value}};
    }
};

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

        return Ok(try$(parseValue<Calc<Length>>(c)));
    }
};

} // namespace Vaev
