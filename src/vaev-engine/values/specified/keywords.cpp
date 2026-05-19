export module Vaev.Engine:values.specified.keywords;

import :css;
import :values.common.keywords;
import :values.specified.base;

namespace Vaev::Experimental {

export template <StrLit K>
struct ValueTraits<Keywords::Keyword<K>> {
    static Res<Keywords::Keyword<K>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->token == Css::Token::ident(K)) {
            c.next();
            return Ok(Keywords::Keyword<K>{});
        }

        return Error::invalidData("expected keyword");
    }
};

} // namespace Vaev::Experimental
