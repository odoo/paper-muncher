export module Vaev.Engine:css.values.gcpm;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.primitives;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Css {

// https://drafts.csswg.org/css-gcpm/#element-syntax
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
};

// https://www.w3.org/TR/css-gcpm-3/#funcdef-element
export template <>
struct ValueParser<ElementFunc> {
    static Res<ElementFunc> parse(Cursor<Sst>& c) {
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
};

// https://drafts.csswg.org/css-gcpm/#running-syntax
export struct RunningPosition {
    CustomIdent customIdent;

    void repr(Io::Emit& e) const {
        e("running '{}'", customIdent);
    }
};

} // namespace Vaev::Css
