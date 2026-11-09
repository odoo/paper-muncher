export module Vaev.Engine:css.values.text;

import Karm.Core;

import :css.values.keywords;

using namespace Karm;

// https://drafts.csswg.org/css-text-3/

namespace Vaev::Css {

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

export using TextTransform = Union<
    Keywords::None,
    Keywords::Uppercase,
    Keywords::Lowercase>;

export using WhiteSpace = Union<
    Keywords::Normal,
    Keywords::Pre,
    Keywords::Nowrap,
    Keywords::PreWrap,
    Keywords::BreakSpaces,
    Keywords::PreLine>;

} // namespace Vaev::Css
