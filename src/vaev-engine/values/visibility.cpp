export module Vaev.Engine:values.visibility;

import Karm.Core;

import :values.keywords;

using namespace Karm;

namespace Vaev {

export using Visibility = Union<
    Keywords::Visible,
    Keywords::Hidden,
    Keywords::Collapse>;

} // namespace Vaev
