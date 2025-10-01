module;

#include <karm-logger/logger.h>
#include <karm-math/au.h>
#include <karm-math/path.h>

export module Vaev.Engine:layout.paint;

import Karm.Image;
import Karm.Gc;
import Karm.Scene;
import Karm.Debug;
import Karm.Gfx;

import :style;
import :layout.base;
import :layout.values;

namespace Vaev::Layout {

static void _paintFrag(Frag& frag, Scene::Stack& stack) {
    auto& s = frag.style();
    if (s.visibility == Visibility::HIDDEN)
        return;

    _paintFragBordersAndBackgrounds(frag, stack);

    if (auto ic = frag.box->content.is<InlineBox>()) {
        stack.add(makeRc<Scene::Text>(frag.metrics.contentBox().topStart().cast<f64>(), ic->prose));
    } else if (auto image = frag.box->content.is<Rc<Gfx::Surface>>()) {
        stack.add(makeRc<Scene::Image>(frag.metrics.borderBox().cast<f64>(), *image, frag.metrics.radii.cast<f64>()));
    } else if (auto svgRoot = frag.content.is<SVGRootFrag>()) {
        if (min(frag.metrics.borderSize.x, frag.metrics.borderSize.y) == 0_au)
            return;

        stack.add(
            makeRc<Scene::Clip>(
                _paintSVGRoot(*svgRoot, s.color),
                frag.metrics.contentBox().cast<f64>()
            )
        );
    }
}

} // namespace Vaev::Layout
