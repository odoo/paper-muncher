export module Vaev.Engine:css.values.pageFloats;

import Karm.Core;

import :css.values.keywords;

using namespace Karm;

// https://drafts.csswg.org/css-page-floats-3/

namespace Vaev::Css {

export using Float = Union<
    Keywords::None,
    Keywords::InlineStart,
    Keywords::InlineEnd,
    Keywords::Left,
    Keywords::Right>;

export using Clear = Union<
    Keywords::None,
    Keywords::Left,
    Keywords::Right,
    Keywords::Both,
    Keywords::InlineStart,
    Keywords::InlineEnd>;

} // namespace Vaev::Css
