module;

#include <karm/macros>

export module Vaev.Engine:values.content;

import Karm.Core;
import :css;
import :values.keywords;
import :values.primitives;

namespace Vaev {

// https://www.w3.org/TR/css-content-3/
// https://www.w3.org/TR/css-gcpm-3/#funcdef-element
export struct ElementContent {
    enum struct Target {
        UNDEFINED,
        FIRST,
        START,
        LAST,
        FIRST_EXCEPT
    };

    CustomIdent customIdent = {""_sym};
    Target target = Target::UNDEFINED;

    explicit ElementContent(CustomIdent customIdent, Target target = Target::UNDEFINED)
        : customIdent(customIdent), target(target) {}

    void repr(Io::Emit& e) const {
        e("element '{}'", customIdent);
    }
};

// https://drafts.csswg.org/css-lists/#auto-numbering
export struct Counter {
    enum struct Type {
        PAGE,
    };
    Type type = Type::PAGE;

    void repr(Io::Emit& e) const {
        e("counter (type:'{}')", type);
    }
};

export using Content = Union<
    Keywords::Normal,
    Keywords::None,
    ElementContent,
    String,
    Counter>;

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
        else if (c->type == Css::Sst::FUNC and c->prefix == Css::Token::function("element(")) {
            Cursor<Css::Sst> cur = c->content;
            auto element = try$(parseValue<ElementContent>(cur));
            c.next();
            return Ok(element);
        } else if (c->type == Css::Sst::FUNC and c->prefix == Css::Token::function("counter(")) {
            Cursor<Css::Sst> cur = c->content;
            auto element = try$(parseValue<Counter>(cur));
            c.next();
            return Ok(element);
        } else if (c->type == Css::Sst::TOKEN and c->token.type == Css::Token::STRING) {
            return Ok(try$(parseValue<String>(c)));
        } else
            return Error::invalidData("expected content");
    }
};

export template <>
struct ValueParser<ElementContent> {
    // https://www.w3.org/TR/css-gcpm-3/#funcdef-element
    static Res<ElementContent> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto ident = ValueParser<CustomIdent>::parse(c);

        if (not ident) {
            return Error::invalidData("ill formed custom ident in content");
        }

        if (c.ended())
            return Ok(ElementContent{ident.take()});

        skipOmmitableComma(c);

        ElementContent::Target target = ElementContent::Target::UNDEFINED;
        if (c.skip(Css::Token::ident("first"))) {
            target = ElementContent::Target::FIRST;
        } else if (c.skip(Css::Token::ident("last"))) {
            target = ElementContent::Target::LAST;
        } else if (c.skip(Css::Token::ident("start"))) {
            target = ElementContent::Target::START;
        } else if (c.skip(Css::Token::ident("first-except"))) {
            target = ElementContent::Target::FIRST_EXCEPT;
        }

        eatWhitespace(c);
        return (Ok(ElementContent{ident.take(), target}));
    }
};

export template <>
struct ValueParser<Counter> {
    // https://www.w3.org/TR/css-gcpm-3/#funcdef-element
    static Res<Counter> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        Counter::Type type = Counter::Type::PAGE;
        if (c.skip(Css::Token::ident("page"))) {
            type = Counter::Type::PAGE;
        } else {
            return Error::invalidData("unsupported counter");
        }
        return (Ok(Counter{type}));
    }
};

} // namespace Vaev
