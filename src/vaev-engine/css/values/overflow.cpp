export module Vaev.Engine:css.values.overflow;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.keywords;
import :css.values.primitives;
import :values.keywords;
import :values.overflow;

using namespace Karm;

namespace Vaev::Css {

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

} // namespace Vaev::Css
