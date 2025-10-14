module Vaev.Engine;

import Karm.Scene;
import Karm.Gfx;
import Karm.Math;
import Karm.Core;

import :layout.base;
import :layout.values;

namespace Vaev::Paint {

void _paintReplaced(Layout::Frag& frag, Scene::Stack& stack) {
    auto& s = frag.style();

    if (auto image = frag.box->content.is<Rc<Scene::Node>>()) {
        auto bound = (*image)->bound();

        auto contentBox = frag.metrics.contentBox().cast<f64>();
        auto trans = Math::Trans2f::map(bound, contentBox);
        Rc<Scene::Node> node = makeRc<Scene::Transform>(*image, trans);

        auto radii = frag.metrics.radii;
        if (radii.zero()) {
            node = makeRc<Scene::Clip>(node, contentBox);
        } else {
            Math::Path path;
            path.rect(contentBox, radii.cast<f64>());
            node = makeRc<Scene::Clip>(node, std::move(path));
        }
        stack.add(node);
    } else if (auto svgRoot = frag.content.is<Layout::SvgRootFrag>()) {
        if (min(frag.metrics.borderSize.x, frag.metrics.borderSize.y) == 0_au)
            return;

        stack.add(
            makeRc<Scene::Clip>(
                _paintSvg(*svgRoot, s.color),
                frag.metrics.contentBox().cast<f64>()
            )
        );
    } else {
        unreachable();
    }
}

// https://www.w3.org/TR/CSS22/zindex.html#:~:text=For%20inline%20elements%3A
void _paintInlineLevel(Layout::Frag& frag, Scene::Stack& stack) {
    if (_requiresStackingContext(frag.style())) {
        _paintStackingContext(frag, stack);
        return;
    }

    // 1. background color of element.
    _paintBackgroundColor(frag, stack);

    // 2. background image of element.
    // TODO

    // 3. border of element.
    _paintBorders(frag, stack);

    // 4. For inline elements

    // 1. For all the element's in-flow, non-positioned, inline-level children
    //    that are in this line box, and all runs of text inside the element
    //    that is on this line box, in tree order:
    if (frag.box->hasLineBoxes()) {
        auto& lineBoxes = frag.box->lineBoxes();
        auto position = frag.metrics.contentBox().topStart().cast<f64>();
        stack.add(makeRc<Scene::Text>(position, lineBoxes.prose));
    }

    for (auto& c : frag.children()) {
        if (c.box->isPositioned())
            continue;

        // Replaced elements
        if (c.box->isReplaced())
            _paintReplaced(c, stack);
        // For inline-block and inline-table elements:
        else
            _paintStackingContext(c, stack);
    }
}

void _paintBlockLevel(Layout::Frag& frag, Scene::Stack& stack) {
    // For all its in-flow, non-positioned, block-level descendants in tree order
    for (auto& c : frag.children()) {
        if (c.box->isPositioned() or not c.box->isBlockLevel())
            continue;

        // 1. If the element is a block-level replaced element, then: the replaced content, atomically.
        if (c.box->isReplaced())
            _paintReplaced(c, stack);
        // Otherwise, for each line box of that element:
        else if (c.box->hasLineBoxes())
            _paintInlineLevel(c, stack);

        _paintBlockLevel(c, stack);
    }
}

} // namespace Vaev::Paint
