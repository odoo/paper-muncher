export module Vaev.Engine:values.text;

import Karm.Core;
import :values.keywords;

using namespace Karm;

namespace Vaev::Css {

// CSS Text Module Level 4
// https://drafts.csswg.org/css-text-4

// MARK: Text Alignment --------------------------------------------------------
// https://drafts.csswg.org/css-text/#text-align-property

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

// MARK: Text Transform --------------------------------------------------------
// https://drafts.csswg.org/css-text-4/#text-transform

export using TextTransform = Union<
    Keywords::None,
    Keywords::Uppercase,
    Keywords::Lowercase>;

// MARK: White Space -----------------------------------------------------------
// https://drafts.csswg.org/css-text/#white-space-property

export using WhiteSpace = Union<
    Keywords::Normal,
    Keywords::Pre,
    Keywords::Nowrap,
    Keywords::PreWrap,
    Keywords::BreakSpaces,
    Keywords::PreLine>;

} // namespace Vaev::Css

namespace Vaev::Style {

export struct Text {
    Css::TextAlign align = Keywords::START;
    Css::TextTransform transform = Keywords::NONE;
    Css::WhiteSpace whiteSpace = Keywords::NORMAL;
};

} // namespace Vaev::Style

