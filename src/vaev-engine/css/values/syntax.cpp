module;

#include <karm/macros>

export module Vaev.Engine:css.values.syntax;

import Karm.Core;

import :css.parser;
import :css.values.base;

using namespace Karm;

// Component value combinators.
// https://drafts.csswg.org/css-values-4/#component-combinators

namespace Vaev::Css {

// Parses one component into its slot, leaving the cursor untouched on failure.
template <typename T>
Res<> _parseComponent(Cursor<Sst>& c, Opt<T>& slot) {
    auto save = c;
    eatWhitespace(c);
    auto res = parseValue<T>(c);
    if (not res) {
        c = save;
        return res.none();
    }
    slot = Some(res.take());
    return Ok();
}

template <typename... Ts>
usize _parseUnordered(Cursor<Sst>& c, Tuple<Opt<Ts>...>& slots) {
    usize parsed = 0;
    while (true) {
        bool progress = slots.apply([&](auto&... slot) {
            return ((not slot and _parseComponent(c, slot)) or ...);
        });
        if (not progress)
            return parsed;
        parsed++;
    }
}

template <typename T>
Res<> _serializeComponent(Io::TextWriter& w, bool& first, T const& component) {
    if (not first)
        try$(w.writeRune(' '));
    first = false;
    return serializeValue(w, component);
}

template <typename T>
Res<> _serializeComponent(Io::TextWriter& w, bool& first, Opt<T> const& component) {
    if (not component)
        return Ok();
    return _serializeComponent(w, first, *component);
}

template <typename... Ts>
Res<> _serializeComponents(Io::TextWriter& w, Tuple<Ts...> const& components) {
    bool first = true;
    return components.apply([&](auto const&... component) -> Res<> {
        Res<> res = Ok();
        // Stops at the first component that fails.
        ((res = _serializeComponent(w, first, component)) and ...);
        return res;
    });
}

template <typename... Ts>
void _reprComponents(Io::Emit& e, Tuple<Ts...> const& components) {
    components.apply([&](auto const&... component) {
        bool first = true;
        ((e(first ? "{}" : " {}", component), first = false), ...);
    });
}

// https://drafts.csswg.org/css-values-4/#comb-all
export template <typename... Ts>
struct All : Tuple<Ts...> {
    void repr(Io::Emit& e) const {
        _reprComponents(e, *this);
    }
};

export template <typename... Ts>
All(Ts...) -> All<Ts...>;

export template <typename... Ts>
struct ValueParser<All<Ts...>> {
    static Res<All<Ts...>> parse(Cursor<Sst>& c) {
        auto save = c;
        Tuple<Opt<Ts>...> slots;
        auto res = slots.apply([&](auto&... slot) -> Res<> {
            Res<> res = Ok();
            // Stops at the first component that fails.
            ((res = _parseComponent(c, slot)) and ...);
            return res;
        });
        if (not res) {
            c = save;
            return res.none();
        }
        return Ok(slots.apply([](auto&... slot) {
            return All<Ts...>{{slot.take()...}};
        }));
    }
};

export template <typename... Ts>
struct ValueSerializer<All<Ts...>> {
    static Res<> serialize(Io::TextWriter& w, All<Ts...> const& value) {
        return _serializeComponents(w, value);
    }
};

// https://drafts.csswg.org/css-values-4/#comb-all
export template <typename First, typename... Rest>
struct AllUnordered : Tuple<First, Rest...> {
    void repr(Io::Emit& e) const {
        _reprComponents(e, *this);
    }
};

export template <typename First, typename... Rest>
AllUnordered(First, Rest...) -> AllUnordered<First, Rest...>;

export template <typename... Ts>
struct ValueParser<AllUnordered<Ts...>> {
    static Res<AllUnordered<Ts...>> parse(Cursor<Sst>& c) {
        auto save = c;
        Tuple<Opt<Ts>...> slots;
        if (_parseUnordered(c, slots) != sizeof...(Ts)) {
            c = save;
            return Error::invalidData("expected all components, in any order");
        }
        return Ok(slots.apply([](auto&... slot) {
            return AllUnordered<Ts...>{{slot.take()...}};
        }));
    }
};

export template <typename... Ts>
struct ValueSerializer<AllUnordered<Ts...>> {
    static Res<> serialize(Io::TextWriter& w, AllUnordered<Ts...> const& value) {
        return _serializeComponents(w, value);
    }
};

// https://drafts.csswg.org/css-values-4/#comb-any
export template <typename First, typename... Rest>
struct Any : Tuple<Opt<First>, Opt<Rest>...> {
    void repr(Io::Emit& e) const {
        _reprComponents(e, *this);
    }
};

export template <typename First, typename... Rest>
Any(Opt<First>, Opt<Rest>...) -> Any<First, Rest...>;

export template <typename... Ts>
struct ValueParser<Any<Ts...>> {
    static Res<Any<Ts...>> parse(Cursor<Sst>& c) {
        Any<Ts...> value;
        if (_parseUnordered(c, static_cast<Tuple<Opt<Ts>...>&>(value)) == 0)
            return Error::invalidData("expected at least one component");
        return Ok(std::move(value));
    }
};

export template <typename... Ts>
struct ValueSerializer<Any<Ts...>> {
    static Res<> serialize(Io::TextWriter& w, Any<Ts...> const& value) {
        return _serializeComponents(w, value);
    }
};

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

export struct Slash {
    void repr(Io::Emit& e) const {
        e("/");
    }
};
export constexpr auto SLASH = Slash{};

template <>
struct ValueParser<Slash> {
    static Res<Slash> parse(Cursor<Sst>& c) {
        if (c.skip(Token::delim("/")))
            return Ok(SLASH);
        return Error::invalidData("expected '/'");
    }
};

template <>
struct ValueSerializer<Slash> {
    static Res<> serialize(Io::TextWriter& w, Slash const&) {
        return w.writeRune('/');
    }
};


} // namespace Vaev::Css
