export module Vaev.Engine:css.values.keywords;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.serializer;
import :css.values.base;
import :css.values.primitives;

using namespace Karm;

namespace Vaev::Css {

export template <StrLit K>
struct Keyword {
    void repr(Io::Emit& e) const {
        e("(keyword {})", K);
    }

    template <Karm::StrLit L>
    constexpr bool operator==(Keyword<L> const&) const {
        return K == L;
    }
};

namespace Keywords {

#define KEYWORD(TYPE, VALUE, NAME)      \
    export using TYPE = Keyword<VALUE>; \
    export constexpr inline TYPE NAME{};
#include "defs/keywords.inc"
#undef KEYWORD

} // namespace Keywords

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
