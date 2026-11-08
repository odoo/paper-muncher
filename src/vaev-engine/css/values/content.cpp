module;

#include <karm/macros>

export module Vaev.Engine:css.values.content;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.keywords;
import :css.values.primitives;
import :values.content;
import :values.primitives;

namespace Vaev::Css {

export template <>
struct ValueParser<ElementFunc> {
    // https://www.w3.org/TR/css-gcpm-3/#funcdef-element
    static Res<ElementFunc> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto ident = ValueParser<CustomIdent>::parse(c);

        if (not ident) {
            return Error::invalidData("ill formed custom ident in content");
        }

        if (c.ended())
            return Ok(ElementFunc{ident.take()});

        skipOmmitableComma(c);

        ElementFunc::Target target = ElementFunc::Target::UNDEFINED;
        if (c.skip(Token::ident("first"))) {
            target = ElementFunc::Target::FIRST;
        } else if (c.skip(Token::ident("last"))) {
            target = ElementFunc::Target::LAST;
        } else if (c.skip(Token::ident("start"))) {
            target = ElementFunc::Target::START;
        } else if (c.skip(Token::ident("first-except"))) {
            target = ElementFunc::Target::FIRST_EXCEPT;
        }

        eatWhitespace(c);
        return (Ok(ElementFunc{ident.take(), target}));
    }
};

} // namespace Vaev::Css
