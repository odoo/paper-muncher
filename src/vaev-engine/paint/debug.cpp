export module Vaev.Engine:paint.debug;

import Karm.Core;
import Karm.Gfx;
import Karm.Gc;

import :dom.node;
import :layout.base;
import :layout.values;

namespace Vaev::Paint {

// MARK: Wireframe -------------------------------------------------------------

export void wireframe(Layout::Frag& frag, Gfx::Canvas& g) {
    for (auto& c : frag.children())
        wireframe(c, g);

    g.strokeStyle({
        .fill = Gfx::GREEN,
        .width = 1,
        .align = Gfx::INSIDE_ALIGN,
    });

    g.stroke(frag.metrics.borderBox().cast<f64>());
}

// MARK: Overlay ---------------------------------------------------------------

export void overlay(Layout::Frag& frag, Gfx::Canvas& g, Gc::Ref<Dom::Node> node) {
    if (frag.box->origin == node) {
        Gfx::Borders border;

        // Margins
        border.widths = frag.metrics.margin.cast<f64>();
        border.withFill(Gfx::YELLOW800.withOpacity(0.5));
        border.withStyle(Gfx::BorderStyle::SOLID);
        border.paint(g, frag.metrics.marginBox().cast<f64>());

        // Borders
        border.widths = frag.metrics.borders.cast<f64>();
        border.withFill(Gfx::YELLOW500.withOpacity(0.5));
        border.withStyle(Gfx::BorderStyle::SOLID);
        border.paint(g, frag.metrics.borderBox().cast<f64>());

        // Paddings
        border.widths = frag.metrics.padding.cast<f64>();
        border.withFill(Gfx::GREEN500.withOpacity(0.5));
        border.withStyle(Gfx::BorderStyle::SOLID);
        border.paint(g, frag.metrics.paddingBox().cast<f64>());

        // Content Box
        g.fillStyle(Gfx::BLUE.withOpacity(0.5));
        g.fill(frag.metrics.contentBox().cast<f64>());
    }

    for (auto& c : frag.children())
        overlay(c, g, node);
}

} // namespace Vaev::Paint
