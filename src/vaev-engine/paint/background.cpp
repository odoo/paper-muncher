export module Vaev.Engine:paint.background;

import Karm.Core;
import Karm.Math;
import :layout.values;

using namespace Karm;

namespace Vaev::Paint {

Math::Vec2f resolveBackgroundPosition(Style::Position const& position, Layout::RectAu const& referenceBox) {
    return {
        Layout::resolve(position.x, referenceBox.width).cast<f64>(),
        Layout::resolve(position.y, referenceBox.height).cast<f64>(),
    };
}

} // namespace Vaev::Paint
