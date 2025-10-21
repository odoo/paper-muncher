export module Vaev.Engine:layout.positioned;

import Karm.Math;

import :values;
import :layout.layout;
import :layout.values;

namespace Vaev::Layout {

Vec2Au _resolveOrigin(Vec2Au fragPosition, Vec2Au containingBlockOrigin, Position position) {
    if (position == Keywords::RELATIVE) {
        return fragPosition;
    } else if (position == Keywords::ABSOLUTE) {
        return containingBlockOrigin;
    } else {
        return {0_au, 0_au};
    }
}

export void layoutPositioned(Tree& tree, Frag& frag, RectAu containingBlock, Input input) {

    auto& style = frag.style();

    auto& metrics = frag.metrics;

    if (impliesRemovingFromFlow(style.position) or style.position == Keywords::RELATIVE) {
        auto origin = _resolveOrigin(metrics.position, containingBlock.topStart(), style.position);
        auto relativeTo = style.position == Keywords::FIXED
                              ? tree.viewport.small
                              : containingBlock;

        auto top = metrics.position.y;
        auto start = metrics.position.x;

        auto topOffset = style.offsets->top;
        if (auto topOffsetCalc = topOffset.is<CalcValue<PercentOr<Length>>>()) {
            top = origin.y + resolve(tree, *frag.box, *topOffsetCalc, relativeTo.height);
        }

        auto startOffset = style.offsets->start;
        if (auto startOffsetCalc = startOffset.is<CalcValue<PercentOr<Length>>>()) {
            start = origin.x + resolve(tree, *frag.box, *startOffsetCalc, relativeTo.width);
        }

        auto endOffset = frag.style().offsets->end;
        if (auto endOffsetCalc = endOffset.is<CalcValue<PercentOr<Length>>>()) {
            start = (origin.x + relativeTo.width) - resolve(tree, *frag.box, *endOffsetCalc, relativeTo.width) - metrics.borderSize.width;
        }

        Vec2Au newPositionOffset = Vec2Au{start, top} - metrics.position;
        frag.offset(newPositionOffset);

        containingBlock = metrics.contentBox();
    }

    for (auto& c : frag.children()) {
        layoutPositioned(tree, c, containingBlock, input);
    }
}

export void lookForRunningPosition(Input& input, Box& box) {
    if (not input.runningPosition)
        return;

    if (box.style->position.is<RunningPosition>()) {
        auto& runningMap = input.runningPosition.peek();
        runningMap.add(input.pageIndex, box);
    }
}

} // namespace Vaev::Layout
