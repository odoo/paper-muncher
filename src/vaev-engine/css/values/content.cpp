module;

#include <karm/macros>

export module Vaev.Engine:css.values.content;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.gcpm;
import :css.values.keywords;
import :css.values.lists;
import :css.values.primitives;

using namespace Karm;

namespace Vaev::Css {

// https://drafts.csswg.org/css-content-3/#content-property
export using ContentValue = Union<
    Keywords::Normal,
    Keywords::None,
    String,
    ElementFunc,
    CounterFunc,
    CountersFunc>;

export template <>
struct ValueParser<ContentValue> {
    static Res<ContentValue> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Token::ident("normal")))
            return Ok(Keywords::NORMAL);
        else if (c.skip(Token::ident("open-quote")))
            return Ok(String{"“"});
        else if (c.skip(Token::ident("close-quote")))
            return Ok(String{"”"});
        else if (c->prefix == Token::function("element(")) {
            Cursor<Sst> cur = c->content;
            auto element = try$(parseValue<ElementFunc>(cur));
            c.next();
            return Ok(element);
        } else if (c->prefix == Token::function("counter(")) {
            return parseValue<CounterFunc>(c);
        } else if (c->prefix == Token::function("counters(")) {
            return parseValue<CountersFunc>(c);
        } else if (c->type == Sst::TOKEN and c->token.type == Token::STRING) {
            return Ok(try$(parseValue<String>(c)));
        } else
            return Error::invalidData("expected content");
    }
};

} // namespace Vaev::Css
