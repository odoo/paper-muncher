module;

#include <karm/macros>

export module Vaev.Engine:values.primitives;

import Karm.Core;
import Karm.Ref;
import Karm.Math;

import :css;
import :values.base;
import :values.resolved;

using namespace Karm;

namespace Vaev {

// MARK: Integer ---------------------------------------------------------------
// https://drafts.csswg.org/css-values/#integers

export using Integer = isize;

export template <>
struct ValueParser<Integer> {
    static Res<Integer> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::NUMBER) {
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

// MARK: Number ----------------------------------------------------------------
// https://drafts.csswg.org/css-values/#numbers

export using Number = f64;

export template <>
struct _Resolved<Number> {
    using Type = Number;
};

export Number resolve(Number const& value, [[maybe_unused]] auto const& ctx = NONE) {
    return value;
}

export template <>
struct ValueParser<Number> {
    static Res<Number> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::NUMBER) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(try$(Io::atof(scan)));
        }

        return Error::invalidData("expected number");
    }
};

export template <>
struct ValueSerializer<Number> {
    // https://drafts.csswg.org/cssom/#serialize-a-css-component-value
    static Res<> serialize(Io::TextWriter& w, Number const& value) {
        // A base-ten number using digits 0-9 (U+0030 to U+0039) in the shortest form possible,
        // using "." to separate decimals (if any), rounding the value if necessary to not produce more than 6 decimals,
        // preceded by "-" (U+002D) if it is negative.
        // FIXME: It truncates instead of rounding, see https://codeberg.org/skift/karm/issues/73.
        return Io::format(w, "{:.6}", value);
    }
};

export template <>
struct ValueParser<bool> {
    static Res<bool> parse(Cursor<Css::Sst>& c) {
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
    static Res<String> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");
        if (c.peek() == Css::Token::STRING)
            return Ok(c.next().token.data);
        return Error::invalidData("expected string");
    }
};

export template <>
struct ValueSerializer<String> {
    // https://drafts.csswg.org/cssom/#serialize-a-css-component-value
    static Res<> serialize(Io::TextWriter& w, String const& value) {
        // The string serialized as a string.
        return Css::serialize(w, Css::Token{Css::Token::STRING, value});
    }
};

// MARK: Custom Ident ----------------------------------------------------------
// https://www.w3.org/TR/css-values-4/#identifier-value

export struct CustomIdent {
    Symbol _symbol = ""_sym;

    bool operator==(CustomIdent const& other) const = default;

    auto operator<=>(CustomIdent const& other) const = default;

    explicit operator bool() const {
        return static_cast<bool>(_symbol);
    }

    Str str() const {
        return _symbol.str();
    }

    void hash(Meta::Derive<Hasher> auto& h) const {
        Karm::hash(h, _symbol);
    }

    void repr(Io::Emit& e) const {
        e("(custom-ident '{}')", _symbol);
    }
};

export template <>
struct ValueParser<CustomIdent> {
    static Res<CustomIdent> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (*c != Css::Token::IDENT)
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

export Res<String> parseUrlIntoString(Cursor<Css::Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    if (c.peek() == Css::Token::URL) {
        auto urlSize = c.peek().token.data.len() - 5; // "url()" takes 5 chars
        auto urlValue = sub(c.next().token.data, Range<usize>{4u, urlSize});
        return Ok(urlValue);
    } else if (c.peek() != Css::Sst::FUNC or c.peek().prefix != Css::Token::function("url("))
        return Error::invalidData("expected url function");

    auto urlFunc = c.next();
    Cursor<Css::Sst> scanUrl{urlFunc.content};
    eatWhitespace(scanUrl);

    if (scanUrl.ended() or not(scanUrl.peek() == Css::Token::STRING))
        return Error::invalidData("expected base url string");

    // TODO: it is unclear what url-modifiers are and how they are used
    return parseValue<String>(scanUrl);
}

export template <>
struct ValueParser<Ref::Url> {
    static Res<Ref::Url> parse(Cursor<Css::Sst>& c) {
        return Ok(Ref::Url::parse(try$(parseUrlIntoString(c))));
    }
};

// MARK: Combinator ------------------------------------------------------------
// https://drafts.csswg.org/css-values-4/#component-combinators

// https://drafts.csswg.org/css-values-4/#comb-one
export template <ValueParseable... Ts>
struct ValueParser<Union<Ts...>> {
    static Res<Union<Ts...>> parse(Cursor<Css::Sst>& c) {
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
    static Res<Vec<T>> parse(Cursor<Css::Sst>& c) {
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

} // namespace Vaev
