export module Vaev.Engine:css.values.sizing;

import Karm.Core;

import :css.values.keywords;

using namespace Karm;

// https://drafts.csswg.org/css-sizing-3/

namespace Vaev::Css {

// https://www.w3.org/TR/css-sizing-3/#box-sizing
export using BoxSizing = Union<Keywords::ContentBox, Keywords::BorderBox>;

} // namespace Vaev::Css
