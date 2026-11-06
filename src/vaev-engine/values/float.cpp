export module Vaev.Engine:values.float_;

import Karm.Core;

import :values.keywords;

using namespace Karm;

namespace Vaev {

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

} // namespace Vaev
