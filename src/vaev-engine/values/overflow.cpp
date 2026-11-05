export module Vaev.Engine:values.overflow;

import Karm.Core;

import :css;
import :values.base;
import :values.keywords;

using namespace Karm;

namespace Vaev::Css {

// MARK: Overflow
// https://www.w3.org/TR/css-overflow/#overflow-control
export using Overflow = Union<
    Keywords::Visible,
    Keywords::Hidden,
    Keywords::Scroll,
    Keywords::Auto>;

export template <>
struct ValueParser<Overflow> {
    static Res<Overflow> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("overlay")))
            // https://www.w3.org/TR/css-overflow/#valdef-overflow-overlay
            return Ok(Keywords::AUTO);
        else if (c.skip(Token::ident("visible")))
            return Ok(Keywords::VISIBLE);
        else if (c.skip(Token::ident("hidden")))
            return Ok(Keywords::HIDDEN);
        else if (c.skip(Token::ident("scroll")))
            return Ok(Keywords::SCROLL);
        else if (c.skip(Token::ident("auto")))
            return Ok(Keywords::AUTO);
        else
            return Error::invalidData("expected overflow value");
    }
};

export struct Overflows {
    Overflow x = Keywords::VISIBLE;
    Overflow y = Keywords::VISIBLE;
    Overflow block = Keywords::VISIBLE;
    Overflow inline_ = Keywords::VISIBLE;

    void repr(Io::Emit& e) const {
        e("(overflows");
        e(" x={}", x);
        e(" y={}", y);
        e(" block={}", block);
        e(" inline={}", inline_);
        e(")");
    }
};

} // namespace Vaev::Css

