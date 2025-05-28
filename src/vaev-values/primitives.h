#pragma once

#include <karm-io/aton.h>
#include <karm-mime/url.h>

#include "base.h"

namespace Vaev {

// MARK: Integer ---------------------------------------------------------------
// https://drafts.csswg.org/css-values/#integers

using Integer = isize;

template <>
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

// MARK: Number ----------------------------------------------------------------
// https://drafts.csswg.org/css-values/#numbers

using Number = f64;

template <>
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

template <>
struct ValueParser<bool> {
    static Res<bool> parse(Cursor<Css::Sst>& c) {
        return Ok(try$(parseValue<Integer>(c)) > 0);
    }
};

// MARK: String ----------------------------------------------------------------
// https://drafts.csswg.org/css-values/#strings

template <>
struct ValueParser<String> {
    static Res<String> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::STRING) {
            // TODO: Handle escape sequences
            Io::SScan s = c.next().token.data.str();
            StringBuilder sb{s.rem()};
            auto quote = s.next();
            while (not s.skip(quote) and not s.ended()) {
                if (s.skip('\\') and not s.ended()) {
                    if (s.skip('\\'))
                        sb.append(s.next());
                } else {
                    sb.append(s.next());
                }
            }
            return Ok(sb.take());
        }

        return Error::invalidData("expected string");
    }
};

// MARK: Url -------------------------------------------------------------------
// https://www.w3.org/TR/css-values-4/#urls

template <>
struct ValueParser<Mime::Url> {
    static Res<Mime::Url> parse(Cursor<Css::Sst>& c) {
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

        auto url = Mime::parseUrlOrPath(try$(parseValue<String>(scanUrl)), NONE);

        // TODO: it is unclear what url-modifiers are and how they are used

        return Ok(url);
    }
};

template <ValueParseable... Ts>
struct ValueParser<Union<Ts...>> {
    static Res<Union<Ts...>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        return Meta::any<Ts...>([&c]<typename T>() -> Res<Union<Ts...>> {
            return Ok(try$(parseValue<T>(c)));
        });
    }
};

} // namespace Vaev
