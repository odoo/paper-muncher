export module Vaev.Engine:values.common.font;

import Karm.Core;
import Karm.Gfx;

import :values.common.keywords;

using namespace Karm;

namespace Vaev::Experimental {

export template <typename A>
using _FontStyle = Union<Keywords::Normal, Keywords::Italic, Keywords::Left, Keywords::Right, Pair<Keywords::Oblique, A>>;

} // namespace Vaev::Experimental
