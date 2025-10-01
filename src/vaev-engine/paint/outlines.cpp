module;

#include <karm-math/au.h>

export module Vaev.Engine:paint.outlines;

import :layout.base;
import :layout.values;

namespace Vaev::Paint {

static Opt<Gfx::Outline> _buildOutline(Layout::Metrics const& metrics, Style::SpecifiedValues const& style, Gfx::Color currentColor) {
    if (metrics.outlineWidth == 0_au)
        return NONE;

    Gfx::Outline outline;

    outline.width = metrics.outlineWidth.cast<f64>();
    outline.offset = metrics.outlineOffset.cast<f64>();

    auto const& outlineStyle = *style.outline;
    if (outlineStyle.style.is<Keywords::Auto>()) {
        outline.style = Gfx::BorderStyle::SOLID;
    } else {
        outline.style = outlineStyle.style.unwrap<Gfx::BorderStyle>();
    }

    if (outlineStyle.color.is<Keywords::Auto>()) {
        outline.fill = resolve(Color(SystemColor::ACCENT_COLOR), currentColor);
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
        makeRc<Scene::Box>(bound, Gfx::Borders{.radii = radii}, std::move(outline), {})
    );
}

} // namespace Vaev::Paint