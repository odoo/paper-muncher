module Vaev.Engine;

import Karm.Scene;
import Karm.Core;
import Karm.Math;

import :layout.fragment;

namespace Vaev::Paint {

// https://drafts.csswg.org/css-position-4/#painting-order
// https://chromium.googlesource.com/chromium/src/+/HEAD/third_party/blink/renderer/core/paint/README.md

void _paintBoxInLineBox(Layout::Frag& root, Scene::Stack& canvas);
void _paintStackingContainer(Layout::Frag& root, Scene::Stack& canvas);

enum struct TreeIteration {
    CONTINUE,
    SKIP_CHILDREN
};

bool _doEstablishStackingContext(Style::SpecifiedValues const& s) {
    return s.clip->has() or
           s.transform->has() or
           s.opacity < 1.0;
}

bool _isStackingContainer(Layout::Frag& frag) {
    if (frag.box->isPositioned())
        return false;

    return frag.style().zIndex == Keywords::AUTO or frag.box->isInlineLevel();
}

void _visitChildrenInTreeOrder(Layout::Frag& root, auto&& visitor) {
    for (auto& c : root.children()) {
        if (visitor(c) == TreeIteration::CONTINUE) {
            _visitChildrenInTreeOrder(c, visitor);
        }
    }
}

void _visitInTreeOrder(Layout::Frag& root, auto&& visitor) {
    if (visitor(root) == TreeIteration::CONTINUE) {
        _visitChildrenInTreeOrder(root, visitor);
    }
}

void _paintBlockDecoration(Layout::Frag& root, Scene::Stack& canvas) {
    // 1. If root is not a table wrapper box:

    // Paint root’s background to canvas if it is not the root element’s principal box.
    if (not root.box->isRootElementPrincipalBox())
        _paintBackgroundColor(root, canvas);

    // Paint root’s border to canvas.
    _paintBorders(root, canvas);

    // 1. TODO: If root is a table wrapper box:

    // Paint root’s background to canvas if it is not the root element’s principal box.

    // For each column group of root in tree order, paint the column group’s background to canvas.

    // For each column of root in tree order, paint the column’s background to canvas.

    // For each row group of root in tree order, paint the row group’s background to canvas.

    // For each row of root in tree order, paint the row’s background to canvas.

    // For each cell of root in tree order, paint the cell’s background to canvas.

    // Paint the borders of all of the table elements of root. If the borders are separated, do so in tree order; if connected, do so as specified in [css-tables-3].
}

void _paintStackingContextInternal(Layout::Frag& root, Scene::Stack& canvas, Options const& options) {
    // 1. If root is an element, paint a stacking context given root’s principal box and canvas, then return.

    // 2. Assert: root is a box, and generates a stacking context.

    // 3. If root is a root element’s principal box
    if (root.box->isRootElementPrincipalBox()) {
        // paint root’s background over the entire canvas,
        // with the origin of the background positioning area being the position on canvas that would
        // be used if root’ s background was being painted normally.
        _paintBackgroundColor(root, canvas, options.canvasBound);
    }

    // 4. If root is a block-level box, paint a block’s decorations given root and canvas.
    if (root.box->isBlockLevel()) {
        _paintBlockDecoration(root, canvas);
    }

    // 5. For each of root’s positioned descendants with negative (non-zero) z-index values,
    _visitChildrenInTreeOrder(
        root,
        [&](Layout::Frag& c) {
            if (not c.box->isPositioned() or c.box->style->zIndex.unwrapOr<Integer>(0) >= 0)
                return TreeIteration::CONTINUE;

            // sort those descendants by z-index order (most negative first) then tree order,
            // and paint a stacking context given each descendant and canvas.
            if (options.paintStackingContainer)
                return TreeIteration::SKIP_CHILDREN;

            _paintStackingContext(c, canvas);
            return TreeIteration::SKIP_CHILDREN;
        }
    );

    // 6. For each of root’s in-flow, non-positioned, block-level descendants, in tree order,
    _visitChildrenInTreeOrder(
        root,
        [&](Layout::Frag& c) {
            if (c.box->isPositioned() or not c.box->isBlockLevel())
                return TreeIteration::SKIP_CHILDREN;

            // paint a block’ s decorations given the descendant and canvas.
            if (_isStackingContainer(root) and not options.paintStackingContainer)
                return TreeIteration::CONTINUE;

            _paintBlockDecoration(c, canvas);
            return TreeIteration::CONTINUE;
        }
    );

    // 7. For each of root’s non-positioned floating descendants, in tree order,
    _visitChildrenInTreeOrder(
        root,
        [&](Layout::Frag& c) {
            if (c.box->isPositioned() or not c.box->isFloating())
                return TreeIteration::CONTINUE;

            // paint a stacking container given the descendant and canvas.
            if (options.paintStackingContainer)
                return TreeIteration::SKIP_CHILDREN;

            _paintStackingContext(c, canvas);
            return TreeIteration::SKIP_CHILDREN;
        }
    );

    // 8. If root is an inline-level box
    if (root.box->isInlineLevel()) {
        // For each line box root is in, paint a box in a line box given root, the line box, and canvas.
        _paintBoxInLineBox(root, canvas);
    }
    // Otherwise
    else {
        // First for root, then for all its in-flow, non-positioned, block-level descendant boxes, in tree order:
        _visitInTreeOrder(
            root,
            [&](Layout::Frag& c) {
                if (c.box->isPositioned() or not c.box->isBlockLevel())
                    return TreeIteration::SKIP_CHILDREN;

                if (_isStackingContainer(root) and not options.paintStackingContainer)
                    return TreeIteration::CONTINUE;

                // 1. If the box is a replaced element, paint the replaced content into canvas, atomically.
                if (c.box->isReplaced())
                    _paintReplaced(c, canvas);

                // 2. Otherwise,
                else {
                    // for each line box of the box, paint a box in a line box given the box, the line box, and canvas.
                    if (c.box->hasLineBoxes()) {
                        auto& lineBoxes = c.box->lineBoxes();
                        auto position = c.metrics.contentBox().topStart().cast<f64>();
                        canvas.add(makeRc<Scene::Text>(position, lineBoxes.prose));
                    }
                }

                // 3. If the UA uses in-band outlines, paint the outlines of the box into canvas.
                _paintOutline(root, canvas);

                return TreeIteration::CONTINUE;
            }
        );
    }

    // 9. For each of root’s positioned descendants with z-index: auto or z-index: 0, in tree order:
    _visitChildrenInTreeOrder(
        root,
        [&](Layout::Frag& c) {
            if (not c.box->isPositioned())
                return TreeIteration::CONTINUE;

            if (options.paintStackingContainer)
                return TreeIteration::SKIP_CHILDREN;

            // descendant has z-index: auto
            if (c.box->style->zIndex.is<Keywords::Auto>()) {
                //     Paint a stacking container given the descendant and canvas.
                _paintStackingContainer(c, canvas);
                return TreeIteration::SKIP_CHILDREN;
            }

            // descendant has z-index: 0
            if (c.box->style->zIndex.unwrap<Integer>() == 0) {
                // Paint a stacking context given the descendant and canvas.
                _paintStackingContext(c, canvas);
                return TreeIteration::SKIP_CHILDREN;
            }

            return TreeIteration::CONTINUE;
        }
    );

    // 10. For each of root’ s positioned descendants with positive (non-zero) z-index values,
    _visitChildrenInTreeOrder(
        root,
        [&](Layout::Frag& c) {
            if (not c.box->isPositioned() or c.box->style->zIndex.unwrapOr<Integer>(0) <= 0)
                return TreeIteration::CONTINUE;

            // sort those descendants by z-index order (smallest first) then tree order,
            // and paint a stacking context given each descendant and canvas.
            if (options.paintStackingContainer)
                return TreeIteration::SKIP_CHILDREN;

            _paintStackingContext(c, canvas);
            return TreeIteration::CONTINUE;
        }
    );

    // 11. If the UA uses out-of-band outlines,
    // draw all of root’ s outlines (those that it skipped drawing due to not using in-band
    // outlines during the current invocation of this algorithm) into canvas.
    // TODO: Support out-of-band outlines
}

void _establishStackingContext(Layout::Frag& frag, Scene::Stack& stack, Options const& options) {
    auto inner = makeRc<Scene::Stack>();
    _paintStackingContextInternal(frag, *inner, options);
    Rc<Scene::Node> out = inner;
    if (frag.style().clip->has())
        out = _applyClip(frag, out);
    if (frag.style().transform->has())
        out = _applyTransform(frag, out);
    if (frag.style().opacity != 1.0)
        out = makeRc<Scene::Opacity>(out, frag.style().opacity);
    stack.add(std::move(out));
}

void _paintStackingContext(Layout::Frag& frag, Scene::Stack& stack, Options const& options) {
    if (_doEstablishStackingContext(frag.style()))
        _establishStackingContext(frag, stack, options);
    else
        _paintStackingContextInternal(frag, stack, options);
}

void _paintBoxInLineBox(Layout::Frag& root, Scene::Stack& canvas) {
    // 1. Paint the backgrounds of root’s fragments that are in line box into canvas.
    _paintBackgroundColor(root, canvas);

    // 2. Paint the borders of root’s fragments that are in line box into canvas.
    _paintBorders(root, canvas);

    // 3. If root is an inline box
    // TODO: Inline level box

    // If root is an inline-level block or table wrapper box
    if (root.box->isInlineLevel()) {
        // Paint a stacking container given root and canvas.
        _paintStackingContainer(root, canvas);
    }

    // If root is an inline-level replaced element
    else if (root.box->isReplaced()) {
        // Paint the replaced content into canvas, atomically.
        _paintReplaced(root, canvas);
    }

    // 4. If the UA uses in-band outlines, paint the outlines of root’s fragments that are in line box into canvas.
    _paintOutline(root, canvas);
}

void _paintStackingContainer(Layout::Frag& root, Scene::Stack& canvas) {
    // 1. Paint a stacking context given root and canvas,
    //    treating root as if it created a new stacking context,
    //    but omitting any positioned descendants or descendants that actually create a stacking context
    //    (letting the parent stacking context paint them, instead).
    _paintStackingContext(root, canvas, {.paintStackingContainer = true});
}

void paintDocument(Layout::Frag& root, Scene::Stack& canvas, Math::Rectf canvasBound) {
    // 1. Paint a stacking context given doc’s root element and canvas.

    _paintStackingContext(
        root, canvas,
        {.rootElement = true, .canvasBound = canvasBound}
    );

    // 2. TODO: For each element el in doc’s top layer:
    // 1. Paint a stacking context given el's ::backdrop pseudo-element and canvas.
    // 2. Paint a stacking context given el and canvas, treating el as a stacking context, with the initial containing block as its containing block.
}

} // namespace Vaev::Paint
