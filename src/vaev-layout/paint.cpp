#include <vaev-paint/borders.h>
#include <vaev-paint/box.h>
#include <vaev-paint/text.h>

#include "paint.h"

namespace Vaev::Layout {

static Paint::Borders _paintBorders(Frag &frag, Math::Vec2f pos) {
    Paint::Borders paint;

    auto currentColor = Gfx::BLACK;
    currentColor = resolve(frag.style->color, currentColor);

    auto bordersLayout = frag.layout.borders;
    auto bordersStyle = frag.style->borders;

    paint = Paint::Borders();
    paint.bound = frag.layout.paddingBox().cast<f64>().offset(pos);
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

    return paint;
}

static void _paintAbsoluteChildren(Frag &frag, Strong<Paint::Stack> &stack, Math::Vec2f initPos) {
    for (auto &c : frag.children()) {
        auto position = c.style->position;
        if (position == Position::ABSOLUTE) {
            paint(c, *stack, initPos);
        } else if (position != Position::RELATIVE) {
            _paintAbsoluteChildren(c, stack, initPos);
        }
    }
}

static void _paintInner(Frag &frag, Paint::Stack &stack, Math::Vec2f pos) {
    auto const &backgrounds = frag.style->backgrounds;

    Gfx::Color currentColor = Gfx::BLACK;
    currentColor = resolve(frag.style->color, currentColor);

    if (backgrounds.len()) {
        Paint::Box paint;

        paint.backgrounds.ensure(backgrounds.len());
        for (auto &bg : backgrounds) {
            paint.backgrounds.pushBack(resolve(bg.fill, currentColor));
        }

        paint.radii = frag.layout.radii.cast<f64>();
        paint.bound = frag.layout.borderBox().cast<f64>().offset(pos);

        stack.add(makeStrong<Paint::Box>(std::move(paint)));
    }

    Math::Vec2f initPos = pos + frag.layout.contentBox().topStart().cast<f64>();

    auto position = frag.style->position;
    auto isRelative = position == Position::ABSOLUTE || position == Position::RELATIVE;
    if (isRelative) {
        auto absoluteStack = makeStrong<Paint::Stack>();
        absoluteStack->zIndex = frag.style->zIndex.value;
        _paintAbsoluteChildren(frag, absoluteStack, initPos);
        stack.add(absoluteStack);
    }

    for (auto &c : frag.children()) {
        if (c.style->position != Position::ABSOLUTE) {
            paint(c, stack, initPos);
        }
    }

    if (auto run = frag.content.is<Strong<Text::Run>>()) {
        Math::Vec2f baseline = {0, frag.font.metrics().ascend};
        stack.add(makeStrong<Paint::Text>(
            pos + frag.layout.borderBox().topStart().cast<f64>() + baseline,
            *run,
            currentColor
        ));
    }

    if (not frag.layout.borders.zero()) {
        auto paint = _paintBorders(frag, pos);
        stack.add(makeStrong<Paint::Borders>(std::move(paint)));
    }
}

void paint(Frag &frag, Paint::Stack &stack, Math::Vec2f pos) {
    if (frag.style->zIndex == ZIndex::AUTO) {
        _paintInner(frag, stack, pos);
    } else {
        auto innerStack = makeStrong<Paint::Stack>();
        innerStack->zIndex = frag.style->zIndex.value;
        _paintInner(frag, *innerStack, pos);
        stack.add(std::move(innerStack));
    }
}

} // namespace Vaev::Layout
