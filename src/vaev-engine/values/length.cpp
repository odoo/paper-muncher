module;

#include <karm/macros>

export module Vaev.Engine:values.length;

import Karm.Core;
import Karm.Math;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev {

export using Math::Au;
export constexpr Au INDEFINITE = Limits<Au>::MAX;

export using Math::InsetsAu;
export using Math::RectAu;
export using Math::Vec2Au;
export using Math::RadiiAu;
export using Math::EllipseAu;

} // namespace Vaev
