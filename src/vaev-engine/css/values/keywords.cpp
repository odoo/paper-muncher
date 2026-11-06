export module Vaev.Engine:css.values.keywords;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.serializer;
import :css.values.base;
import :css.values.primitives;
import :values.keywords;

using namespace Karm;

namespace Vaev::Css {

export template <StrLit K>
struct ValueParser<Keyword<K>> {
    static Res<Keyword<K>> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->token == Token::ident(K)) {
            c.next();
            return Ok(Keyword<K>{});
        }

        return Error::invalidData("expected keyword");
    }
};

export template <StrLit K>
struct ValueSerializer<Keyword<K>> {
    // https://drafts.csswg.org/cssom/#serialize-a-css-component-value
    static Res<> serialize(Io::TextWriter& w, Keyword<K> const&) {
        // The keyword converted to ASCII lowercase.
        return w.writeStr(static_cast<Str>(K));
    }
};

} // namespace Vaev::Css
