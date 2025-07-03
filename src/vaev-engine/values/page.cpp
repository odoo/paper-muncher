module;

#include <karm-base/res.h>

export module Vaev.Engine:values.page;

import Karm.Print;
import :css;
import :values.base;

namespace Vaev {

// MARK: Orientation -----------------------------------------------------------
// https://drafts.csswg.org/mediaqueries/#orientation

export template <>
struct ValueParser<Print::Orientation> {
    static Res<Print::Orientation> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("portrait")))
            return Ok(Print::Orientation::PORTRAIT);
        else if (c.skip(Css::Token::ident("landscape")))
            return Ok(Print::Orientation::LANDSCAPE);
        else
            return Error::invalidData("expected orientation");
    }
};

} // namespace Vaev
