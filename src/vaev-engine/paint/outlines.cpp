module Vaev.Engine;

import Karm.Gfx;
import Karm.Scene;
import Karm.Math;
import Karm.Core;

import :layout.base;
import :layout.values;

namespace Vaev::Paint {

static Opt<Gfx::Outline> _buildOutline(Layout::Metrics const& metrics, Style::SpecifiedValues const& style, Gfx::Color currentColor) {
    if (metrics.outlineWidth == 0_au)
        return NONE;

    Gfx::Outline outline;

    auto const& outlineStyle = *style.outline;
    if (outlineStyle.style == Gfx::BorderStyle::NONE)
        return NONE;

    if (outlineStyle.style == Keywords::AUTO) {
        outline.style = Gfx::BorderStyle::SOLID;
    } else {
        outline.style = outlineStyle.style.unwrap<Gfx::BorderStyle>();
    }

    outline.width = metrics.outlineWidth.cast<f64>();
    outline.offset = metrics.outlineOffset.cast<f64>();

    if (outlineStyle.color == Keywords::AUTO) {
        if (outlineStyle.style == Keywords::AUTO) {
            outline.fill = resolve(Color(SystemColor::ACCENT_COLOR), currentColor);
        } else {
            outline.fill = currentColor;
        }
    } else {
        outline.fill = resolve(outlineStyle.color.unwrap<Color>(), currentColor);
    }

    return outline;
}

void _paintOutline(Layout::Frag& frag, Scene::Stack& stack) {
    auto outline = _buildOutline(frag.metrics, frag.style(), frag.style().color);
    if (not outline)
        return;

    Math::Rectf bound = frag.metrics.borderBox().round().cast<f64>();
    auto radii = frag.metrics.radii.cast<f64>();

    stack.add(
        makeRc<Scene::Box>(
            bound,
            Gfx::Borders{
                .radii = radii,
                .widths = 0,
                .fills = {Gfx::ALPHA},
                .styles = {Gfx::BorderStyle::NONE}
            },
            outline.take(),
            Vec<Gfx::Fill>{}
        )
    );
}

} // namespace Vaev::Paint
