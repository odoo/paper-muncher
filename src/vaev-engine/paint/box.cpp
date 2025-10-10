module Vaev.Engine;

import Karm.Scene;
import Karm.Gfx;
import Karm.Math;
import Karm.Core;

import :layout.base;
import :layout.values;

namespace Vaev::Paint {

void _paintInlineLevel(Layout::Frag& frag, Scene::Stack& stack) {
    // 1. For each box that is a child of that element, in that line box, in tree order
    if (auto ic = frag.box->content.is<Layout::InlineBox>())
        stack.add(makeRc<Scene::Text>(frag.metrics.contentBox().topStart().cast<f64>(), ic->prose));

    // 4. For inline elements:
    for (auto& c : frag.children()) {
        if (c.style().position != Position::STATIC)
            continue;

        // For inline-block and inline-table elements:

        // For each one of these, treat the element as if it created a new stacking
        // context, but any positioned descendants and descendants which actually
        // create a new stacking context should be considered part of the parent
        // stacking context, not this new one.
        _paintStackingContext(c, stack);
    }
}

void _paintBlockLevel(Layout::Frag& frag, Scene::Stack& stack) {
    auto& s = frag.style();

    // 1. If the element is a block-level replaced element, then: the replaced content, atomically.
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
    } else if (auto svgRoot = frag.content.is<Layout::SVGRootFrag>()) {
        if (min(frag.metrics.borderSize.x, frag.metrics.borderSize.y) == 0_au)
            return;

        stack.add(
            makeRc<Scene::Clip>(
                _paintSvg(*svgRoot, s.color),
                frag.metrics.contentBox().cast<f64>()
            )
        );
    }
    // Otherwise, for each line box of that element:
    else {
        _paintInlineLevel(frag, stack);
    }
}

} // namespace Vaev::Paint
