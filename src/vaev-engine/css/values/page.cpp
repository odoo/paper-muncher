export module Vaev.Engine:css.values.page;

import Karm.Core;
import Karm.Print;

import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.keywords;
import :css.values.primitives;

using namespace Karm;

namespace Vaev::Css {

// MARK: Orientation -----------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#orientation

export template <>
struct ValueParser<Print::Orientation> {
    static Res<Print::Orientation> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("portrait")))
            return Ok(Print::Orientation::PORTRAIT);
        else if (c.skip(Token::ident("landscape")))
            return Ok(Print::Orientation::LANDSCAPE);
        else
            return Error::invalidData("expected orientation");
    }
};

} // namespace Vaev::Css
