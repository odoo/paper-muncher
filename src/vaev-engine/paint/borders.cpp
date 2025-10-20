module Vaev.Engine;

import Karm.Gfx;
import Karm.Scene;
import Karm.Math;
import Karm.Core;

import :layout.base;
import :layout.values;

using namespace Karm;

namespace Vaev::Paint {

Opt<Gfx::Borders> _buildBorders(Layout::Metrics const& metrics, Style::SpecifiedValues const& style, Gfx::Color currentColor) {
    if (metrics.borders.zero())
        return NONE;

    Gfx::Borders borders;

    auto const& bordersLayout = metrics.borders;
    borders.widths.top = bordersLayout.top.cast<f64>();
    borders.widths.bottom = bordersLayout.bottom.cast<f64>();
    borders.widths.start = bordersLayout.start.cast<f64>();
    borders.widths.end = bordersLayout.end.cast<f64>();

    auto const& bordersStyle = *style.borders;
    borders.styles[0] = bordersStyle.top.style;
    borders.styles[1] = bordersStyle.end.style;
    borders.styles[2] = bordersStyle.bottom.style;
    borders.styles[3] = bordersStyle.start.style;

    borders.fills[0] = Vaev::resolve(bordersStyle.top.color, currentColor);
    borders.fills[1] = Vaev::resolve(bordersStyle.end.color, currentColor);
    borders.fills[2] = Vaev::resolve(bordersStyle.bottom.color, currentColor);
    borders.fills[3] = Vaev::resolve(bordersStyle.start.color, currentColor);

    return borders;
}

void _paintBorders(Layout::Frag& frag, Scene::Stack& stack) {
    auto bordersWithoutRadii = _buildBorders(frag.metrics, frag.style(), frag.style().color);
    if (not bordersWithoutRadii)
        return;

    Math::Rectf bound = frag.metrics.borderBox().round().cast<f64>();

    auto borders = bordersWithoutRadii ? bordersWithoutRadii.take() : Gfx::Borders{};
    borders.radii = frag.metrics.radii.cast<f64>();

    stack.add(
        makeRc<Scene::Box>(bound, std::move(borders), Gfx::Outline{}, Vec<Gfx::Fill>{})
    );
}

} // namespace Vaev::Paint
