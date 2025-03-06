#include <karm-scene/box.h>
#include <karm-scene/image.h>
#include <karm-scene/text.h>

#include "base.h"
#include "paint.h"

namespace Vaev::Layout {

static bool _paintBorders(Frag& frag, Gfx::Color currentColor, Gfx::Borders& borders) {
    borders.radii = frag.metrics.radii.cast<f64>(); // This value is needed for the outline

    if (frag.metrics.borders.zero())
        return false;

    currentColor = resolve(frag.style().color, currentColor);

    auto const& bordersLayout = frag.metrics.borders;
    borders.widths.top = bordersLayout.top.cast<f64>();
    borders.widths.bottom = bordersLayout.bottom.cast<f64>();
    borders.widths.start = bordersLayout.start.cast<f64>();
    borders.widths.end = bordersLayout.end.cast<f64>();

    auto const& bordersStyle = *frag.style().borders;
    borders.styles[0] = bordersStyle.top.style;
    borders.styles[1] = bordersStyle.end.style;
    borders.styles[2] = bordersStyle.bottom.style;
    borders.styles[3] = bordersStyle.start.style;

    borders.fills[0] = resolve(bordersStyle.top.color, currentColor);
    borders.fills[1] = resolve(bordersStyle.end.color, currentColor);
    borders.fills[2] = resolve(bordersStyle.bottom.color, currentColor);
    borders.fills[3] = resolve(bordersStyle.start.color, currentColor);

    return true;
}

static bool _paintOutline(Frag& frag, Gfx::Color currentColor, Gfx::Outline& outline) {
    if (frag.metrics.outlineWidth == 0_au)
        return false;

    outline.width = frag.metrics.outlineWidth.cast<f64>();
    outline.offset = frag.metrics.outlineOffset.cast<f64>();

    auto const& outlineStyle = *frag.style().outline;
    if (outlineStyle.style.is<Keywords::Auto>()) {
        outline.style = Gfx::BorderStyle::SOLID;
    } else {
        outline.style = outlineStyle.style.unwrap<Gfx::BorderStyle>();
    }

    if (outlineStyle.color.is<Keywords::Auto>()) {
        if (outlineStyle.style.is<Keywords::Auto>()) {
            outline.fill = resolve(Color(SystemColor::ACCENT_COLOR), currentColor);
        } else {
            outline.fill = currentColor;
        }
    } else {
        outline.fill = resolve(outlineStyle.color.unwrap<Color>(), currentColor);
    }

    return true;
}

static void _paintFragBordersAndBackgrounds(Frag& frag, Scene::Stack& stack) {
    auto const& cssBackground = frag.style().backgrounds;

    Gfx::Borders borders;
    Gfx::Outline outline;

    Vec<Gfx::Fill> backgrounds;
    auto color = resolve(cssBackground->color, frag.style().color);
    if (color.alpha != 0)
        backgrounds.pushBack(color);

    auto currentColor = resolve(frag.style().color, color);
    bool hasBorders = _paintBorders(frag, currentColor, borders);
    bool hasOutline = _paintOutline(frag, currentColor, outline);
    Math::Rectf bound = frag.metrics.borderBox().cast<f64>();

    if (any(backgrounds) or hasBorders or hasOutline)
        stack.add(makeRc<Scene::Box>(bound, std::move(borders), std::move(outline), std::move(backgrounds)));
}

static void _establishStackingContext(Frag& frag, Scene::Stack& stack);
static void _paintStackingContext(Frag& frag, Scene::Stack& stack);

static void _paintFrag(Frag& frag, Scene::Stack& stack) {
    _paintFragBordersAndBackgrounds(frag, stack);

    if (auto prose = frag.box->content.is<Rc<Text::Prose>>()) {
        (*prose)->_style.color = frag.style().color;

        stack.add(makeRc<Scene::Text>(
            frag.metrics.borderBox().topStart().cast<f64>(),
            *prose
        ));
    } else if (auto image = frag.box->content.is<Karm::Image::Picture>()) {
        stack.add(makeRc<Scene::Image>(
            frag.metrics.borderBox().cast<f64>(),
            *image
        ));
    }
}

static void _paintChildren(Frag& frag, Scene::Stack& stack, auto predicate) {
    for (auto& c : frag.children) {
        auto& s = c.style();

        auto zIndex = s.zIndex;
        if (zIndex != ZIndex::AUTO) {
            if (predicate(s))
                _establishStackingContext(c, stack);
            continue;
        }

        // NOTE: Positioned elements act as if they establish a stacking context
        auto position = s.position;
        if (position != Position::STATIC) {
            if (predicate(s))
                _paintStackingContext(c, stack);
            continue;
        }

        if (predicate(s))
            _paintFrag(c, stack);
        _paintChildren(c, stack, predicate);
    }
}

static void _paintStackingContext(Frag& frag, Scene::Stack& stack) {
    // 1. the background and borders of the element forming the stacking context.
    _paintFrag(frag, stack);

    // 2. the child stacking contexts with negative stack levels (most negative first).
    _paintChildren(frag, stack, [](Style::Computed const& s) {
        return s.zIndex.value < 0;
    });

    // 3. the in-flow, non-inline-level, non-positioned descendants.
    _paintChildren(frag, stack, [](Style::Computed const& s) {
        return s.zIndex == ZIndex::AUTO and s.display != Display::INLINE and s.position == Position::STATIC;
    });

    // 4. the non-positioned floats.
    _paintChildren(frag, stack, [](Style::Computed const& s) {
        return s.zIndex == ZIndex::AUTO and s.position == Position::STATIC and s.float_ != Float::NONE;
    });

    // 5. the in-flow, inline-level, non-positioned descendants, including inline tables and inline blocks.
    _paintChildren(frag, stack, [](Style::Computed const& s) {
        return s.zIndex == ZIndex::AUTO and s.display == Display::INLINE and s.position == Position::STATIC;
    });

    // 6. the child stacking contexts with stack level 0 and the positioned descendants with stack level 0.
    _paintChildren(frag, stack, [](Style::Computed const& s) {
        return s.zIndex.value == 0 and s.position != Position::STATIC;
    });

    // 7. the child stacking contexts with positive stack levels (least positive first).
    _paintChildren(frag, stack, [](Style::Computed const& s) {
        return s.zIndex.value > 0;
    });
}

static void _establishStackingContext(Frag& frag, Scene::Stack& stack) {
    auto innerStack = makeRc<Scene::Stack>();
    innerStack->zIndex = frag.style().zIndex.value;
    _paintStackingContext(frag, *innerStack);
    stack.add(std::move(innerStack));
}

void paint(Frag& frag, Scene::Stack& stack) {
    _paintStackingContext(frag, stack);
}

void wireframe(Frag& frag, Gfx::Canvas& g) {
    for (auto& c : frag.children)
        wireframe(c, g);

    g.strokeStyle({
        .fill = Gfx::BLACK,
        .width = 1,
        .align = Gfx::INSIDE_ALIGN,
    });

    g.stroke(frag.metrics.borderBox().cast<f64>());
}

} // namespace Vaev::Layout
