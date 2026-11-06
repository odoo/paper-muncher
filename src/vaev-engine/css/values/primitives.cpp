module;

#include <karm/macros>

export module Vaev.Engine:css.values.primitives;

import Karm.Core;
import Karm.Ref;

import :css.lexer;
import :css.parser;
import :css.serializer;
import :css.values.base;
import :values.primitives;

using namespace Karm;

namespace Vaev::Css {

// MARK: Integer ---------------------------------------------------------------
// https://drafts.csswg.org/css-values/#integers

export template <>
struct ValueParser<Integer> {
    static Res<Integer> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Token::NUMBER) {
            Io::SScan scan = c->token.data.str();
            c.next();
            Integer result = try$(Io::atoi(scan));
            if (scan.ended())
                return Ok(result);
        }

        return Error::invalidData("expected integer");
    }
};

export template <>
struct ValueSerializer<Integer> {
    // https://drafts.csswg.org/cssom/#serialize-a-css-component-value
    static Res<> serialize(Io::TextWriter& w, Integer const& value) {
        // A base-ten integer using digits 0-9 (U+0030 to U+0039) in the shortest form possible,
        // preceded by "-" (U+002D) if it is negative.
        return Io::format(w, "{}", value);
    }
};

export template <>
struct ValueParser<f64> {
    static Res<f64> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Token::NUMBER) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(try$(Io::atof(scan)));
        }

        return Error::invalidData("expected number");
    }
};

export template <>
struct ValueSerializer<f64> {
    static Res<> serialize(Io::TextWriter& w, f64 const& value) {
        return serializeNumber(w, value);
    }
};

export template <>
struct ValueParser<bool> {
    static Res<bool> parse(Cursor<Sst>& c) {
        return Ok(try$(parseValue<Integer>(c)) > 0);
    }
};

/// Helper to serialize an enum variant as if it was a keyword.
export template <Meta::Enum E>
struct ValueSerializer<E> {
    static Res<> serialize(Io::TextWriter& w, E const& value) {
        for (auto r : iterRunes(nameOf(value))) {
            if (r == '_') {
                try$(w.writeRune('-'));
            } else {
                try$(w.writeRune(toAsciiLower(r)));
            }
        }

        return Ok();
    }
};

// MARK: String ----------------------------------------------------------------
// https://drafts.csswg.org/css-values/#strings

export template <>
struct ValueParser<String> {
    static Res<String> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");
        if (c.peek() == Token::STRING)
            return Ok(c.next().token.data);
        return Error::invalidData("expected string");
    }
};

export template <>
struct ValueSerializer<String> {
    // https://drafts.csswg.org/cssom/#serialize-a-css-component-value
    static Res<> serialize(Io::TextWriter& w, String const& value) {
        // The string serialized as a string.
        return Css::serialize(w, Token{Token::STRING, value});
    }
};

// MARK: Custom Ident ----------------------------------------------------------
// https://www.w3.org/TR/css-values-4/#identifier-value

export template <>
struct ValueParser<CustomIdent> {
    static Res<CustomIdent> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (*c != Token::IDENT)
            return Error::invalidData("expected custom ident");

        // TODO: Don't really know what to do with the following... lol
        //       Represents any valid CSS identifier that would not be
        //       misinterpreted as a pre-defined keyword in that property’s value definition
        //       (See https://www.w3.org/TR/css-values-4/#custom-idents)
        CustomIdent ident = CustomIdent{Symbol::from(c->token.data)};
        c.next();
        return Ok(std::move(ident));
    }
};

// MARK: Url -------------------------------------------------------------------
// https://www.w3.org/TR/css-values-4/#urls

export Res<String> parseUrlIntoString(Cursor<Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.peek() == Token::URL) {
        auto urlSize = c.peek().token.data.len() - 5; // "url()" takes 5 chars
        auto urlValue = sub(c.next().token.data, Range<usize>{4u, urlSize});
        return Ok(urlValue);
    } else if (c.peek() != Sst::FUNC or c.peek().prefix != Token::function("url("))
        return Error::invalidData("expected url function");

    auto urlFunc = c.next();
    Cursor<Sst> scanUrl{urlFunc.content};
    eatWhitespace(scanUrl);

    if (scanUrl.ended() or not(scanUrl.peek() == Token::STRING))
        return Error::invalidData("expected base url string");

    // TODO: it is unclear what url-modifiers are and how they are used
    return parseValue<String>(scanUrl);
}

export template <>
struct ValueParser<Ref::Url> {
    static Res<Ref::Url> parse(Cursor<Sst>& c) {
        return Ok(Ref::Url::parse(try$(parseUrlIntoString(c))));
    }
};

// MARK: Combinator ------------------------------------------------------------
// https://drafts.csswg.org/css-values-4/#component-combinators

// https://drafts.csswg.org/css-values-4/#comb-one
export template <ValueParseable... Ts>
struct ValueParser<Union<Ts...>> {
    static Res<Union<Ts...>> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        return Meta::any<Ts...>([&c]<typename T>() -> Res<Union<Ts...>> {
            return Ok(try$(parseValue<T>(c)));
        });
    }
};

export template <ValueSerializable... Ts>
struct ValueSerializer<Union<Ts...>> {
    static Res<> serialize(Io::TextWriter& w, Union<Ts...> const& value) {
        return value.visit([&](auto const& inner) {
            return serializeValue(w, inner);
        });
    }
};

// MARK: Component Value Multipliers -------------------------------------------
// https://drafts.csswg.org/css-values-4/#component-multipliers

// https://drafts.csswg.org/css-values-4/#mult-zero-plus
export template <ValueParseable T>
struct ValueParser<Vec<T>> {
    static Res<Vec<T>> parse(Cursor<Sst>& c) {
        Vec<T> result = {};

        while (true) {
            if (c.ended())
                return Ok(std::move(result));
            auto maybeValue = parseValue<T>(c);
            if (not maybeValue)
                return Ok(std::move(result));
            result.pushBack(maybeValue.take());
        }
    }
};

export template <Sliceable S>
    requires ValueSerializable<typename S::Inner>
struct ValueSerializer<S> {
    static Res<> serialize(Io::TextWriter& w, S const& value) {
        bool first = true;

        for (auto const& item : value) {
            if (first) {
                first = false;
            } else {
                try$(w.writeRune(' '));
            }

            try$(serializeValue(w, item));
        }

        return Ok();
    }
};

} // namespace Vaev::Css
