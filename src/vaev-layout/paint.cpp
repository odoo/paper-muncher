#include <vaev-paint/borders.h>
#include <vaev-paint/box.h>
#include <vaev-paint/text.h>

#include "paint.h"

namespace Vaev::Layout {

static void _paintBorders(Frag &frag, Gfx::Color currentColor, Paint::Stack &stack) {
    Paint::Borders paint;

    currentColor = resolve(frag.style->color, currentColor);

    auto bordersLayout = frag.layout.borders;
    auto bordersStyle = frag.style->borders;

    paint = Paint::Borders();
    paint.bound = frag.layout.paddingBox().cast<f64>();
    paint.radii = frag.layout.radii.cast<f64>().reduceOverlap(paint.bound.size());

    paint.top.width = bordersLayout.top.cast<f64>();
    paint.top.style = bordersStyle->top.style;
    paint.top.fill = resolve(bordersStyle->top.color, currentColor);

    paint.bottom.width = bordersLayout.bottom.cast<f64>();
    paint.bottom.style = bordersStyle->bottom.style;
    paint.bottom.fill = resolve(bordersStyle->bottom.color, currentColor);

    paint.start.width = bordersLayout.start.cast<f64>();
    paint.start.style = bordersStyle->start.style;
    paint.start.fill = resolve(bordersStyle->start.color, currentColor);

    paint.end.width = bordersLayout.end.cast<f64>();
    paint.end.style = bordersStyle->end.style;
    paint.end.fill = resolve(bordersStyle->end.color, currentColor);

    stack.add(makeStrong<Paint::Borders>(std::move(paint)));
}

static void _paintBackground(Frag &frag, Gfx::Color currentColor, Paint::Stack &stack) {
    auto const &backgrounds = frag.style->backgrounds;

    if (isEmpty(backgrounds))
        return;

    Paint::Box paint;

    paint.backgrounds.ensure(backgrounds.len());
    for (auto &bg : backgrounds) {
        auto color = resolve(bg.fill, currentColor);

        // Skip transparent backgrounds
        if (color.alpha == 0)
            continue;

        paint.backgrounds.pushBack(color);
    }

    paint.radii = frag.layout.radii.cast<f64>();
    paint.bound = frag.layout.borderBox().cast<f64>();

    // Skip if there are no backgrounds to paint
    if (isEmpty(paint.backgrounds))
        return;

    stack.add(makeStrong<Paint::Box>(std::move(paint)));
}

static void _establishStackingContext(Frag &frag, Paint::Stack &stack);
static void _paintStackingContext(Frag &frag, Paint::Stack &stack);

static void _paintFrag(Frag &frag, Paint::Stack &stack) {
    Gfx::Color currentColor = Gfx::BLACK;
    currentColor = resolve(frag.style->color, currentColor);

    _paintBackground(frag, currentColor, stack);

    if (not frag.layout.borders.zero())
        _paintBorders(frag, currentColor, stack);

    if (auto run = frag.content.is<Strong<Text::Run>>()) {
        Math::Vec2f baseline = {0, frag.font.metrics().ascend};
        stack.add(makeStrong<Paint::Text>(
            frag.layout.borderBox().topStart().cast<f64>() + baseline,
            *run,
            currentColor
        ));
    }
}

static void _paintChildren(Frag &frag, Paint::Stack &stack, auto predicate) {
    for (auto &c : frag.children()) {
        auto &s = *c.style;

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

static void _paintStackingContext(Frag &frag, Paint::Stack &stack) {
    // 1. the background and borders of the element forming the stacking context.
    _paintFrag(frag, stack);

    // 2. the child stacking contexts with negative stack levels (most negative first).
    _paintChildren(frag, stack, [](Style::Computed const &s) {
        return s.zIndex.value < 0;
    });

    // 3. the in-flow, non-inline-level, non-positioned descendants.
    _paintChildren(frag, stack, [](Style::Computed const &s) {
        return s.zIndex == ZIndex::AUTO and s.display != Display::INLINE and s.position == Position::STATIC;
    });

    // 4. the non-positioned floats.
    _paintChildren(frag, stack, [](Style::Computed const &s) {
        return s.zIndex == ZIndex::AUTO and s.position == Position::STATIC and s.float_ != Float::NONE;
    });

    // 5. the in-flow, inline-level, non-positioned descendants, including inline tables and inline blocks.
    _paintChildren(frag, stack, [](Style::Computed const &s) {
        return s.zIndex == ZIndex::AUTO and s.display == Display::INLINE and s.position == Position::STATIC;
    });

    // 6. the child stacking contexts with stack level 0 and the positioned descendants with stack level 0.
    _paintChildren(frag, stack, [](Style::Computed const &s) {
        return s.zIndex.value == 0 and s.position != Position::STATIC;
    });

    // 7. the child stacking contexts with positive stack levels (least positive first).
    _paintChildren(frag, stack, [](Style::Computed const &s) {
        return s.zIndex.value > 0;
    });
}

static void _establishStackingContext(Frag &frag, Paint::Stack &stack) {
    auto innerStack = makeStrong<Paint::Stack>();
    innerStack->zIndex = frag.style->zIndex.value;
    _paintStackingContext(frag, *innerStack);
    stack.add(std::move(innerStack));
}

void paint(Frag &frag, Paint::Stack &stack) {
    _paintStackingContext(frag, stack);
}

} // namespace Vaev::Layout
