export module Vaev.Engine:css.values.insets;

import Karm.Core;
import Karm.Math;

import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.calc;
import :css.values.keywords;
import :css.values.primitives;
import :values.insets;
import :values.keywords;
import :values.primitives;

using namespace Karm;

namespace Vaev::Css {

// MARK: Position --------------------------------------------------------------

export template <>
struct ValueParser<Position> {
    // https://drafts.csswg.org/css-position-3/#propdef-position
    static Res<Position> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("static")))
            return Ok(Keywords::STATIC);
        else if (c.skip(Token::ident("relative")))
            return Ok(Keywords::RELATIVE);
        else if (c.skip(Token::ident("absolute")))
            return Ok(Keywords::ABSOLUTE);
        else if (c.skip(Token::ident("fixed")))
            return Ok(Keywords::FIXED);
        else if (c.skip(Token::ident("sticky")))
            return Ok(Keywords::STICKY);
        else if (c->type == Sst::FUNC and c->prefix == Token::function("running(")) {
            Cursor<Sst> cur = c->content;
            auto res = parseValue<CustomIdent>(cur);
            if (not res) {
                return Error::invalidData("ill formed custom-ident in running position");
            }
            c.next();
            return Ok(RunningPosition{res.take()});

        } else
            return Error::invalidData("expected position");
    }
};

export template <typename T>
struct ValueParser<Math::Insets<T>> {
    static Res<Math::Insets<T>> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        eatWhitespace(c);
        auto top = parseValue<T>(c);
        if (not top)
            return top.none();

        eatWhitespace(c);
        auto right = parseValue<T>(c);
        if (not right)
            return Ok(Math::Insets<T>{top.take()});

        eatWhitespace(c);
        auto bottom = parseValue<T>(c);
        if (not bottom)
            return Ok(Math::Insets<T>{top.take(), right.take()});

        eatWhitespace(c);
        auto left = parseValue<T>(c);
        if (not left)
            return Ok(Math::Insets<T>{top.take(), right.take(), bottom.take()});

        return Ok(Math::Insets<T>{top.take(), right.take(), bottom.take(), left.take()});
    }
};

// https://drafts.csswg.org/css-align-3/#column-row-gap
export using Gap = Union<
    Keywords::Normal,
    LengthPercentage>;

} // namespace Vaev::Css
