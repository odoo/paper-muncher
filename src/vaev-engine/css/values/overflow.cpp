export module Vaev.Engine:css.values.overflow;

import Karm.Core;

import :css.values.keywords;

using namespace Karm;

// https://drafts.csswg.org/css-overflow-3/

namespace Vaev::Css {

// https://www.w3.org/TR/css-overflow/#overflow-control
export using Overflow = Union<
    Keywords::Visible,
    Keywords::Hidden,
    Keywords::Scroll,
    Keywords::Auto>;

} // namespace Vaev::Css
