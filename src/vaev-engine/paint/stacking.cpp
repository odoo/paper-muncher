module;

#include <karm-math/rect.h>

export module Vaev.Engine:paint.stacking;

import Karm.Core;
import Karm.Scene;
import :layout.base;
import :paint.background;
import :paint.borders;
import :paint.clip;
import :paint.transform;
import :paint.outlines;

namespace Vaev::Paint {

struct Options {
    Math::Rectf canvasBound;
    bool rootElement = false;
};

void _paintStackingContext(Layout::Frag& frag, Scene::Stack& stack, Options const& options = {});
void _paintStackingContextInternal(Layout::Frag& frag, Scene::Stack& stack, Options const& options);

bool _needsNewStackingContext(Style::SpecifiedValues const& s) {
    return s.clip.has() or
           s.transform->has() or
           s.opacity != 1.0;
}

void _establishStackingContext(Layout::Frag& frag, Scene::Stack& stack, Options const& options) {
    auto inner = makeRc<Scene::Stack>();
    _paintStackingContextInternal(frag, *inner, options);
    Rc<Scene::Node> out = inner;
    if (frag.style().clip.has())
        out = _applyClip(frag, out);
    if (frag.style().transform->has())
        out = _applyTransform(frag, out);
    if (frag.style().opacity != 1.0)
        out = makeRc<Scene::Opacity>(out, frag.style().opacity);
    stack.add(std::move(out));
}

// https://www.w3.org/TR/CSS22/zindex.html
void _paintStackingContextInternal(Layout::Frag& frag, Scene::Stack& stack, Options const& options) {
    auto const& style = frag.style();

    // 1. If the element is a root element:
    if (options.rootElement) {
        // 1. background color of element over the entire canvas.
        _paintBackgroundColor(frag, stack, options.canvasBound);

        // 2. background image of element, over the entire canvas, anchored at the origin that would be used if it was painted for the root element.
        // TODO
    }

    // 2. If the element is a block, list-item, or other block equivalent:
    if (style.display == Display::BLOCK or style.display == Display::Item::YES) {
        if (not options.rootElement) {
            // 1. background color of element unless it is the root element.
            _paintBackgroundColor(frag, stack);

            // 2. background image of element unless it is the root element.
            // TODO
        }

        // 3. border of element.
        _paintBorders(frag, stack);
    }

    // 3. Stacking contexts formed by positioned descendants with negative z-indices (excluding 0) in z-index order (most negative first) then tree order
    for (auto& c : frag.children()) {
        if (c.style().zIndex.unwrapOr<Integer>(0) >= 0)
            break;
        _paintStackingContext(c, stack);
    }

    // 4. For all its in-flow, non-positioned, block-level descendants in tree order: If the element is a block, list-item, or other block equivalent:
    for (auto& c : frag.children()) {
        // 1. background color of element.
        _paintBackgroundColor(c, stack);
        // 2. background image of element.
        // TODO

        // 3. border of element.
        _paintBorders(c, stack);
    }

    // 5. All non-positioned floating descendants, in tree order.
    for (auto& c : frag.children()) {
        if (c.style().position == Position::STATIC and c.style().float_ != Float::NONE) {
            // For each one of these, treat the element as if it created a new stacking context,
            // but any positioned descendants and descendants which actually create a new stacking context should be considered part of the parent stacking context, not this new one.
            _paintStackingContext(frag, stack);
        }
    }

    // 6. If the element is an inline element that generates a stacking context
    if (frag.box->content.is<Layout::InlineBox>()) {
        // For each line box that the element is in:
        //     Jump to 7.2.1 for the box(es) of the element in that line box (in tree order).
    }

    // 7. Otherwise: first for the element, then for all its in-flow, non-positioned, block-level descendants in tree order:
    else {
    }

    // 8. All positioned descendants with 'z-index: auto' or 'z-index: 0', in tree order.
    for (auto& c : frag.children()) {
        if (style.position == Position::STATIC)
            continue;

        //     For those with 'z-index: auto', treat the element as if it created a new stacking context, but any positioned descendants and descendants which actually create a new stacking context should be considered part of the parent stacking context, not this new one.
        //     For those with 'z-index: 0', treat the stacking context generated atomically.
        _paintStackingContext(frag, stack);
    }

    // 9. Stacking contexts formed by positioned descendants with z-indices greater than or equal to 1 in z-index order (smallest first) then tree order.
    for (auto& c : frag.children()) {
        if (c.style().zIndex.unwrapOr<Integer>(0) >= 1)
            _paintStackingContext(c, stack);
    }

    // 10. Finally, draw outlines from this stacking context
    _paintOutline(frag, stack);
}

void _paintStackingContext(Layout::Frag& frag, Scene::Stack& stack, Options const& options) {
    if (_needsNewStackingContext(frag.style()))
        _establishStackingContext(frag, stack, options);
    else
        _paintStackingContextInternal(frag, stack, options);
}

void _sortZIndex(Layout::Frag& frag) {
    auto children = frag.children();
    stableSort(children, [](Layout::Frag& a, Layout::Frag& b) {
        return a.style().zIndex.unwrapOr<Integer>(0) <=> b.style().zIndex.unwrapOr<Integer>(0);
    });
    for (auto& c : children)
        _sortZIndex(c);
}

export void paint2(Layout::Frag& frag, Scene::Stack& stack) {
    _sortZIndex(frag);
    _paintStackingContext(
        frag,
        stack,
        {
            .rootElement = true,
        }
    );
}

} // namespace Vaev::Paint