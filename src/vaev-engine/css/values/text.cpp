export module Vaev.Engine:css.values.text;

import Karm.Core;

import :css.values.keywords;

using namespace Karm;

namespace Vaev::Css {

// https://drafts.csswg.org/css-text-4/#propdef-text-align
export using TextAlign = Union<
    Keywords::Start,
    Keywords::End,
    Keywords::Left,
    Keywords::Right,
    Keywords::Center,
    Keywords::Justify,
    Keywords::MatchParent,
    Keywords::JustifyAll,
    Keywords::VaevBlockCenter>;

// https://drafts.csswg.org/css-text-4/#propdef-text-transform
export using TextTransform = Union<
    Keywords::None,
    Keywords::Uppercase,
    Keywords::Lowercase>;

// https://drafts.csswg.org/css-text-4/#propdef-white-space
export using WhiteSpace = Union<
    Keywords::Normal,
    Keywords::Pre,
    Keywords::Nowrap,
    Keywords::PreWrap,
    Keywords::BreakSpaces,
    Keywords::PreLine>;

} // namespace Vaev::Css
