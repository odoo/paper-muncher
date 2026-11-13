export module Vaev.Engine:values.overflow;

import Karm.Core;

import :css;
import :values.base;

using namespace Karm;

namespace Vaev {

// MARK: Overflow
// https://www.w3.org/TR/css-overflow/#overflow-control
export enum struct Overflow : u8 {
    VISIBLE,
    HIDDEN,
    CLIP,
    SCROLL,
    AUTO,

    _LEN
};

export template <>
struct ValueParser<Overflow> {
    static Res<Overflow> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("overlay")))
            // https://www.w3.org/TR/css-overflow/#valdef-overflow-overlay
            return Ok(Overflow::AUTO);
        else if (c.skip(Css::Token::ident("visible")))
            return Ok(Overflow::VISIBLE);
        else if (c.skip(Css::Token::ident("hidden")))
            return Ok(Overflow::HIDDEN);
        else if (c.skip(Css::Token::ident("clip")))
            return Ok(Overflow::CLIP);
        else if (c.skip(Css::Token::ident("scroll")))
            return Ok(Overflow::SCROLL);
        else if (c.skip(Css::Token::ident("auto")))
            return Ok(Overflow::AUTO);
        else
            return Error::invalidData("expected overflow value");
    }
};

export struct Overflows {
    Overflow x = Overflow::VISIBLE;
    Overflow y = Overflow::VISIBLE;
    Overflow block = Overflow::VISIBLE;
    Overflow inline_ = Overflow::VISIBLE;

    // https://www.w3.org/TR/css-overflow-3/#scrollable-overflow-value
    bool scrollable() const {
        auto scrollableValue = [](Overflow overflow) {
            return overflow == Overflow::SCROLL or
                   overflow == Overflow::AUTO or
                   overflow == Overflow::HIDDEN;
        };

        return scrollableValue(x) or
               scrollableValue(y) or
               scrollableValue(block) or
               scrollableValue(inline_);
    }

    // https://www.w3.org/TR/css-overflow-3/#valdef-overflow-visible
    bool clips() const {
        return x != Overflow::VISIBLE or
               y != Overflow::VISIBLE or
               block != Overflow::VISIBLE or
               inline_ != Overflow::VISIBLE;
    }

    void repr(Io::Emit& e) const {
        e("(overflows");
        e(" x={}", x);
        e(" y={}", y);
        e(" block={}", block);
        e(" inline={}", inline_);
        e(")");
    }
};

} // namespace Vaev
