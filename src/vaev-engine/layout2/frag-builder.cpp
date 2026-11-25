export module Vaev.Engine:layout2.fragBuilder;

import :layout2.layout;
import :layout.layout;

namespace Vaev::Layout2 {

export struct FragBuilder {
    Layout::Box& _box;
    Vec<Rc<Layout::Fragment>> _children;

    FragBuilder(Layout::Box& box)
        : _box(box) {}

    void addChild(Rc<Layout::Fragment>& child, Vec2Au offset, LogicalInsets<Au> const& margins) {
        auto writingMode = _box.style->writingMode;
        auto direction = _box.style->direction;

        if (auto [boxFrag] = child.is<Layout::BoxFragment>()) {
            boxFrag.metrics.position = offset;
            boxFrag.metrics.margin = margins.toPhysical(writingMode, direction);
        }

        _children.pushBack(child);
    }

    Rc<Layout::Fragment> buildBox(Input const& input, LogicalSize<Au> size) {
        auto writingMode = _box.style->writingMode;
        auto direction = _box.style->direction;

        auto metrics = Layout::BoxMetrics {
            .padding = input.metrics.paddings.toPhysical(writingMode, direction),
            .borders = input.metrics.borders.toPhysical(writingMode, direction),
            .position = {0_au, 0_au},
            .borderSize = size.toPhysical(writingMode),
            .margin = {0_au},
        };

        return makeRc<Layout::BoxFragment>(_box, metrics, std::move(_children));
    }
};

} // namespace Vaev::Layout2
