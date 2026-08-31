export module Vaev.Engine:paint.background;

import Karm.Core;
import Karm.Math;
import :layout.values;

using namespace Karm;

namespace Vaev::Paint {

Math::Vec2f resolveBackgroundPosition(BackgroundPosition const& position, RectAu const& referenceBox) {
    Layout::Resolver resolver = {};

    auto horizontalPosition = resolver.resolve(position.horizontal, referenceBox.width);
    auto verticalPosition = resolver.resolve(position.vertical, referenceBox.height);

    Math::Vec2f result;
    if (position.horizontalAnchor.is<Keywords::Left>()) {
        result.x = horizontalPosition.cast<f64>();
    } else if (position.horizontalAnchor.is<Keywords::Right>()) {
        result.x = (referenceBox.width - horizontalPosition).cast<f64>();
    } else if (position.horizontalAnchor.is<Keywords::Center>()) {
        result.x = (referenceBox.width / 2.0 - horizontalPosition / 2.0).cast<f64>();
    }

    if (position.verticalAnchor.is<Keywords::Top>()) {
        result.y = verticalPosition.cast<f64>();
    } else if (position.verticalAnchor.is<Keywords::Bottom>()) {
        result.y = (referenceBox.height - verticalPosition).cast<f64>();
    } else if (position.verticalAnchor.is<Keywords::Center>()) {
        result.y = (referenceBox.height / 2.0 - verticalPosition / 2.0).cast<f64>();
    }

    return result;
}

} // namespace Vaev::Paint
