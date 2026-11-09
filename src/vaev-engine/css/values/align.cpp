module;

#include <karm/macros>

export module Vaev.Engine:css.values.align;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.serializer;
import :css.values.base;
import :css.values.primitives;

using namespace Karm;

namespace Vaev::Css {

// https://drafts.csswg.org/css-align-3/#propdef-align-content
// https://drafts.csswg.org/css-align-3/#propdef-justify-content
// https://drafts.csswg.org/css-align-3/#propdef-justify-self
// https://drafts.csswg.org/css-align-3/#propdef-align-self
// https://drafts.csswg.org/css-align-3/#propdef-justify-items
// https://drafts.csswg.org/css-align-3/#propdef-align-items
export struct [[gnu::packed]] Align {
    enum Prefix : u8 {
        NO_PREFIX,

        LEGACY,
        SAFE,
        UNSAFE,

        // NOTE: This enum is pack with 2 bits, so the max value is 3

        _LEN0,
    };

    enum Keywords : u8 {
        NO_KEYWORD,

        AUTO,
        NORMAL,

        // Positional Alignment
        CENTER,
        START,
        END,
        SELF_START,
        SELF_END,
        FLEX_START,
        FLEX_END,
        LEFT,
        RIGHT,

        // BaseLine Alignment
        FIRST_BASELINE,
        LAST_BASELINE,

        // Distribution Alignment
        STRETCH,
        SPACE_BETWEEN,
        SPACE_AROUND,
        SPACE_EVENLY,

        // NOTE: This enum is pack with 6 bits, so the max value is 63

        _LEN1,
    };

    Prefix prefix : 2;
    Keywords keyword : 6;

    constexpr Align() : prefix(NO_PREFIX), keyword(NO_KEYWORD) {}

    constexpr Align(Prefix prefix) : prefix(prefix), keyword(NO_KEYWORD) {}

    constexpr Align(Keywords keyword) : prefix(NO_PREFIX), keyword(keyword) {}

    constexpr Align(Prefix prefix, Keywords keyword) : prefix(prefix), keyword(keyword) {}

    bool operator==(Prefix prefix) const { return this->prefix == prefix; }

    bool operator==(Keywords keyword) const { return this->keyword == keyword; }

    void repr(Io::Emit& e) const {
        e("{} {}", prefix, keyword);
    }

    static Res<Align> parse(Cursor<Sst>& c);
    static Res<> serialize(Io::TextWriter& w, Align const& value);
};

Res<Align> Align::parse(Cursor<Sst>& c) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    Align align;

    if (c.skip(Token::ident("legacy"))) {
        align.prefix = Align::Prefix::LEGACY;
    } else if (c.skip(Token::ident("safe"))) {
        align.prefix = Align::Prefix::SAFE;
    } else if (c.skip(Token::ident("unsafe"))) {
        align.prefix = Align::Prefix::UNSAFE;
    }

    if (c.skip(Token::ident("auto"))) {
        align.keyword = Align::Keywords::AUTO;
    } else if (c.skip(Token::ident("normal"))) {
        align.keyword = Align::Keywords::NORMAL;
    } else if (c.skip(Token::ident("center"))) {
        align.keyword = Align::Keywords::CENTER;
    } else if (c.skip(Token::ident("start"))) {
        align.keyword = Align::Keywords::START;
    } else if (c.skip(Token::ident("end"))) {
        align.keyword = Align::Keywords::END;
    } else if (c.skip(Token::ident("self-start"))) {
        align.keyword = Align::Keywords::SELF_START;
    } else if (c.skip(Token::ident("self-end"))) {
        align.keyword = Align::Keywords::SELF_END;
    } else if (c.skip(Token::ident("flex-start"))) {
        align.keyword = Align::Keywords::FLEX_START;
    } else if (c.skip(Token::ident("flex-end"))) {
        align.keyword = Align::Keywords::FLEX_END;
    } else if (c.skip(Token::ident("left"))) {
        align.keyword = Align::Keywords::LEFT;
    } else if (c.skip(Token::ident("right"))) {
        align.keyword = Align::Keywords::RIGHT;
    } else if (c.skip(Token::ident("baseline"))) {
        // https://drafts.csswg.org/css-align-3/#valdef-justify-self-baseline
        align.keyword = Align::Keywords::FIRST_BASELINE;
    } else if (c.skip(Token::ident("first-baseline"))) {
        align.keyword = Align::Keywords::FIRST_BASELINE;
    } else if (c.skip(Token::ident("last-baseline"))) {
        align.keyword = Align::Keywords::LAST_BASELINE;
    } else if (c.skip(Token::ident("stretch"))) {
        align.keyword = Align::Keywords::STRETCH;
    } else if (c.skip(Token::ident("space-between"))) {
        align.keyword = Align::Keywords::SPACE_BETWEEN;
    } else if (c.skip(Token::ident("space-around"))) {
        align.keyword = Align::Keywords::SPACE_AROUND;
    } else if (c.skip(Token::ident("space-evenly"))) {
        align.keyword = Align::Keywords::SPACE_EVENLY;
    }

    if (align == Align::NO_PREFIX and align == Align::NO_KEYWORD)
        return Error::invalidData("expected align value");

    return Ok(align);
}

Res<> Align::serialize(Io::TextWriter& w, Align const& value) {
    if (value.prefix != Align::NO_PREFIX)
        try$(serializeValue(w, value.prefix));

    if (value.keyword == Align::NO_KEYWORD)
        return Ok();

    if (value.prefix != Align::NO_PREFIX)
        try$(w.writeRune(' '));

    // https://drafts.csswg.org/css-align-3/#baseline-values
    // Because they are equivalent, and baseline is shorter, the CSSOM serializes first baseline as baseline.
    if (value.keyword == Align::FIRST_BASELINE)
        return w.writeStr("baseline"s);

    if (value.keyword == Align::LAST_BASELINE)
        return w.writeStr("last baseline"s);

    return serializeValue(w, value.keyword);
}

} // namespace Vaev::Css
