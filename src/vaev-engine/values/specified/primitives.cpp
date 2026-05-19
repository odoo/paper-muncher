module;

#include <karm/macros>

export module Vaev.Engine:values.specified.primitives;

import :values.specified.base;

import Karm.Core;

import :css;

using namespace Karm;

namespace Vaev::Experimental {

// MARK: Number ----------------------------------------------------------------
// https://drafts.csswg.org/css-values/#numbers

export template <>
struct ValueTraits<f64> {
    static Res<f64> parse(Cursor<Css::Sst>& c) {
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

// MARK: String ----------------------------------------------------------------
// https://drafts.csswg.org/css-values/#strings

export template <>
struct ValueTraits<String> {
    static Res<String> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");
        if (c.peek() == Css::Token::STRING)
            return Ok(c.next().token.data);
        return Error::invalidData("expected string");
    }
};

} // namespace Vaev::Experimental
