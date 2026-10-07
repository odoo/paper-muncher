export module Vaev.Engine:layout2.fragBuilder;

import :layout2.layout;
import :layout.layout;

namespace Vaev::Layout2 {

export struct FragBuilder {
    Layout::Tree& _tree;
    Layout::Box& _box;
    Vec<Rc<Layout::Fragment>> _children;

    FragBuilder(Layout::Tree& tree, Layout::Box& box)
        : _tree(tree),
          _box(box) {}

    void addChildIfAny(Opt<Rc<Layout::Fragment>> child, Vec2Au offset) {
        if (child) {
            if (auto [boxFrag] = child->is<Layout::BoxFragment>()) {
                boxFrag.metrics.position = offset;
            }

            _children.pushBack(child.take());
        }
    }

    Rc<Layout::Fragment> buildBox(Input const& input, LogicalSize<Au> size) {
        auto writingMode = _box.style->writingMode;
        auto direction = _box.style->direction;

        auto metrics = Layout::BoxMetrics {
            .padding = input.metrics.paddings.toPhysical(writingMode, direction),
            .borders = input.metrics.borders.toPhysical(writingMode, direction),
            .position = {0_au, 0_au},
            .borderSize = size.toPhysical(writingMode),
            .margin = input.constraints.margins.toPhysical(writingMode, direction),
        };

        return makeRc<Layout::BoxFragment>(_box, metrics, std::move(_children));
    }
};

} // namespace Vaev::Layout2
