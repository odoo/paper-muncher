module;

#include <karm/macros>

export module Vaev.Engine:values.content;

import Karm.Core;
import :css;
import :values.keywords;
import :values.primitives;
import :values.counter;

namespace Vaev {

// https://www.w3.org/TR/css-content-3/
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
};

export using Content = Union<
    Keywords::Normal,
    Keywords::None,
    String,
    ElementFunc,
    CounterFunc,
    CountersFunc>;

export template <>
struct ValueParser<Content> {
    // https://www.w3.org/TR/css-content-3/
    // https://www.w3.org/TR/css-gcpm-3/#funcdef-element
    static Res<Content> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::ident("normal")))
            return Ok(Keywords::NORMAL);
        else if (c.skip(Css::Token::ident("open-quote")))
            return Ok(String{"“"});
        else if (c.skip(Css::Token::ident("close-quote")))
            return Ok(String{"”"});
        else if (c->prefix == Css::Token::function("element(")) {
            Cursor<Css::Sst> cur = c->content;
            auto element = try$(parseValue<ElementFunc>(cur));
            c.next();
            return Ok(element);
        } else if (c->prefix == Css::Token::function("counter(")) {
            return parseValue<CounterFunc>(c);
        } else if (c->prefix == Css::Token::function("counters(")) {
            return parseValue<CountersFunc>(c);
        } else if (c->type == Css::Sst::TOKEN and c->token.type == Css::Token::STRING) {
            return Ok(try$(parseValue<String>(c)));
        } else
            return Error::invalidData("expected content");
    }
};

export template <>
struct ValueParser<ElementFunc> {
    // https://www.w3.org/TR/css-gcpm-3/#funcdef-element
    static Res<ElementFunc> parse(Cursor<Css::Sst>& c) {
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
        if (c.skip(Css::Token::ident("first"))) {
            target = ElementFunc::Target::FIRST;
        } else if (c.skip(Css::Token::ident("last"))) {
            target = ElementFunc::Target::LAST;
        } else if (c.skip(Css::Token::ident("start"))) {
            target = ElementFunc::Target::START;
        } else if (c.skip(Css::Token::ident("first-except"))) {
            target = ElementFunc::Target::FIRST_EXCEPT;
        }

        eatWhitespace(c);
        return (Ok(ElementFunc{ident.take(), target}));
    }
};

} // namespace Vaev
