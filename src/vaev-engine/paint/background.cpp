module Vaev.Engine;

import Karm.Gfx;
import Karm.Scene;
import Karm.Math;
import Karm.Core;

import :layout.values;

using namespace Karm;

namespace Vaev::Paint {

Math::Radiif _resolveRadii(Layout::Resolver& resolver, Math::Radii<CalcValue<PercentOr<Length>>> const& baseRadii, RectAu const& referenceBox) {
    Math::Radiif radii;
    radii.a = resolver.resolve(baseRadii.a, referenceBox.height).cast<f64>();
    radii.b = resolver.resolve(baseRadii.b, referenceBox.width).cast<f64>();
    radii.c = resolver.resolve(baseRadii.c, referenceBox.width).cast<f64>();
    radii.d = resolver.resolve(baseRadii.d, referenceBox.height).cast<f64>();
    radii.e = resolver.resolve(baseRadii.e, referenceBox.height).cast<f64>();
    radii.f = resolver.resolve(baseRadii.f, referenceBox.width).cast<f64>();
    radii.g = resolver.resolve(baseRadii.g, referenceBox.width).cast<f64>();
    radii.h = resolver.resolve(baseRadii.h, referenceBox.height).cast<f64>();
    return radii;
}

Math::Vec2f _resolveBackgroundPosition(Layout::Resolver& resolver, BackgroundPosition const& position, RectAu const& referenceBox) {
    Math::Vec2f result;

    if (position.horizontalAnchor.is<Keywords::Left>()) {
        result.x = resolver.resolve(position.horizontal, referenceBox.width).cast<f64>();
    } else if (position.horizontalAnchor.is<Keywords::Right>()) {
        result.x = (referenceBox.width - resolver.resolve(position.horizontal, referenceBox.width)).cast<f64>();
    } else if (position.horizontalAnchor.is<Keywords::Center>()) {
        result.x = referenceBox.width.cast<f64>() / 2.0;
    }

    if (position.verticalAnchor.is<Keywords::Top>()) {
        result.y = resolver.resolve(position.vertical, referenceBox.height).cast<f64>();
    } else if (position.verticalAnchor.is<Keywords::Bottom>()) {
        result.y = (referenceBox.height - resolver.resolve(position.vertical, referenceBox.height)).cast<f64>();
    } else if (position.verticalAnchor.is<Keywords::Center>()) {
        result.y = referenceBox.height.cast<f64>() / 2.0;
    }

    return result;
}

void _paintBackgroundColor(Layout::Frag& frag, Scene::Stack& stack, Math::Rectf bound) {
    auto const& cssBackground = frag.style().backgrounds;

    auto color = resolve(cssBackground->color, frag.style().color);
    if (color.alpha == 0)
        return;

    stack.add(
        makeRc<Scene::Box>(
            bound,
            Gfx::Borders{
                .radii = frag.metrics.radii.cast<f64>(),
                .widths = 0,
                .fills = {Gfx::ALPHA},
                .styles = {Gfx::BorderStyle::NONE}
            },
            Gfx::Outline{},
            Vec<Gfx::Fill>{color}
        )
    );
}

void _paintBackgroundColor(Layout::Frag& frag, Scene::Stack& stack) {
    Math::Rectf bound =
        frag
            .metrics.borderBox()
            .round()
            .cast<f64>();

    _paintBackgroundColor(frag, stack, bound);
}

} // namespace Vaev::Paint
