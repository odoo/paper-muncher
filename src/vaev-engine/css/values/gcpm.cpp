export module Vaev.Engine:css.values.gcpm;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.primitives;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Css {

// https://www.w3.org/TR/css-gcpm-3/#funcdef-element
export struct ElementFunc {
    enum struct Target {
        UNDEFINED,
        FIRST,
        START,
        LAST,
        FIRST_EXCEPT
    };

    CustomIdent customIdent = {""_sym};
    Target target = Target::UNDEFINED;

    explicit ElementFunc(CustomIdent customIdent, Target target = Target::UNDEFINED)
        : customIdent(customIdent), target(target) {}

    void repr(Io::Emit& e) const {
        e("element '{}'", customIdent);
    }

    static Res<ElementFunc> parse(Cursor<Sst>& c);
};

// https://www.w3.org/TR/css-gcpm-3/#funcdef-element
Res<ElementFunc> ElementFunc::parse(Cursor<Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    auto ident = parseValue<CustomIdent>(c);

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

export struct RunningPosition {
    CustomIdent customIdent;

    void repr(Io::Emit& e) const {
        e("running '{}'", customIdent);
    }
};

} // namespace Vaev::Css
