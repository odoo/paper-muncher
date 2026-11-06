export module Vaev.Engine:values.color;

import Karm.Core;
import Karm.Gfx;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev {

export constexpr Gfx::Color TRANSPARENT = Gfx::Color::fromRgba(0, 0, 0, 0);

} // namespace Vaev
