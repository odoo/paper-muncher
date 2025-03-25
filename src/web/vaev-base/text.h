#pragma once

#include <karm-io/emit.h>
#include <vaev-base/keywords.h>
#include <vaev-base/length.h>
#include <vaev-base/percent.h>

namespace Vaev {

// CSS Text Module Level 4
// https://drafts.csswg.org/css-text-4

// MARK: Text Alignment --------------------------------------------------------
// https://drafts.csswg.org/css-text/#text-align-property

enum struct TextAlign {
    START,
    END,
    LEFT,
    RIGHT,
    CENTER,
    JUSTIFY,
    MATCH_PARENT,
    JUSTIFY_ALL,

    _LEN,
};

// MARK: Text Transform --------------------------------------------------------
// https://drafts.csswg.org/css-text-4/#text-transform

enum struct TextTransform {
    NONE, //< https://drafts.csswg.org/css-text-4/#valdef-text-transform-none

    UPPERCASE, //< https://drafts.csswg.org/css-text-4/#valdef-text-transform-uppercase
    LOWERCASE, //< https://drafts.csswg.org/css-text-4/#valdef-text-transform-lowercase

    _LEN,
};

// MARK: White Space -----------------------------------------------------------
// https://drafts.csswg.org/css-text/#white-space-property

enum struct WhiteSpace {
    NORMAL, //< https://drafts.csswg.org/css-text/#valdef-white-space-normal

    PRE,          //< https://drafts.csswg.org/css-text/#valdef-white-space-pre
    NOWRAP,       //< https://drafts.csswg.org/css-text/#valdef-white-space-nowrap
    PRE_WRAP,     //< https://drafts.csswg.org/css-text/#valdef-white-space-pre-wrap
    BREAK_SPACES, //< https://drafts.csswg.org/css-text/#valdef-white-space-break-spaces
    PRE_LINE,     //< https://drafts.csswg.org/css-text/#valdef-white-space-pre-line

    _LEN,
};

// MARK: Word spacing ------------------------------------------------------------
// https://www.w3.org/TR/css-text-4/#word-spacing-property

struct WordSpacing {
    Union<PercentOr<Length>, Keywords::Normal> _value;

    WordSpacing(Keywords::Normal) : _value(Keywords::NORMAL) {}

    WordSpacing(PercentOr<Length> value) : _value(value) {}

    Opt<PercentOr<Length>> value() const {
        if (_value.is<Keywords::Normal>())
            return NONE;
        return _value.unwrap<PercentOr<Length>>();
    }

    void repr(Io::Emit& e) const {
        e("(word-spacing: ");
        e("{}", _value);
        e(")");
    }
};

struct TextProps {
    TextAlign align = TextAlign::START;
    TextTransform transform = TextTransform::NONE;
    WhiteSpace whiteSpace = WhiteSpace::NORMAL;
    WordSpacing wordSpacing = Keywords::NORMAL;

    void repr(Io::Emit& e) const {
        e("(text");
        e(" align: {}", align);
        e(" transform: {}", transform);
        e(" whiteSpace: {}", whiteSpace);
        e(")");
    }
};

} // namespace Vaev
