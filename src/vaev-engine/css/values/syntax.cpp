module;

#include <karm/macros>

export module Vaev.Engine:css.values.syntax;

import Karm.Core;

import :css.parser;
import :css.values.base;

using namespace Karm;

// Component value multipliers.
// https://drafts.csswg.org/css-values-4/#component-multipliers

namespace Vaev::Css {

// https://drafts.csswg.org/css-values-4/#mult-opt
export template <typename T>
struct ValueParser<Opt<T>> {
    static Res<Opt<T>> parse(Cursor<Sst>& c) {
        auto save = c;
        eatWhitespace(c);
        auto res = parseValue<T>(c);
        if (not res) {
            c = save;
            return Ok(NONE);
        }
        return Ok(Some(res.take()));
    }
};

export template <typename T>
struct ValueSerializer<Opt<T>> {
    static Res<> serialize(Io::TextWriter& w, Opt<T> const& value) {
        if (not value)
            return Ok();
        return serializeValue(w, *value);
    }
};

// https://drafts.csswg.org/css-values-4/#mult-comma
// A hash mark (#) indicates that the preceding type, word, or group occurs one or more times,
// separated by comma tokens.
export template <typename T>
struct CommaSeparated : Vec<T> {
    using Vec<T>::Vec;
};

export template <typename T>
struct ValueParser<CommaSeparated<T>> {
    static Res<CommaSeparated<T>> parse(Cursor<Sst>& c) {
        CommaSeparated<T> result;
        while (true) {
            eatWhitespace(c);
            result.pushBack(try$(parseValue<T>(c)));
            eatWhitespace(c);
            if (not c.skip(Token::COMMA))
                return Ok(std::move(result));
        }
    }
};

export template <typename T>
struct ValueSerializer<CommaSeparated<T>> {
    static Res<> serialize(Io::TextWriter& w, CommaSeparated<T> const& value) {
        bool first = true;
        for (auto const& item : value) {
            if (not first)
                try$(w.writeStr(", "s));
            first = false;
            try$(serializeValue(w, item));
        }
        return Ok();
    }
};

} // namespace Vaev::Css
