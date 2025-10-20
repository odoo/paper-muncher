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

} // namespace Vaev::Paint
