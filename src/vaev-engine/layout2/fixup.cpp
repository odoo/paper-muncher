export module Vaev.Engine:layout2.fixup;

import :layout.fragment;

namespace Vaev::Layout2 {

export void absolutize(Rc<Layout::Fragment> node, Vec2Au absPos = {0_au}) {
    if (auto [boxFrag] = node.is<Layout::BoxFragment>()) {
        boxFrag.metrics.position = absPos + boxFrag.metrics.position;
        for (auto const& child : boxFrag.children()) {
            absolutize(child, boxFrag.metrics.position);
        }
    } else {
        // TODO
    }
}

} // namespace Vaev::Layout2
