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

export void layoutPositioned(Tree& tree, Rc<Fragment> frag, RectAu containingBlock, Input input) {
    auto& box = frag->originatingBox();
    auto& style = frag->style();
    auto borderBox = frag->borderBox();

    if (box.isRemovedFromFlow() or style.position == Keywords::RELATIVE) {
        auto origin = _resolveOrigin(borderBox.xy, containingBlock.topStart(), style.position);
        auto relativeTo = style.position == Keywords::FIXED
                              ? tree.viewport.small
                              : containingBlock;

        auto top = borderBox.top();
        auto start = borderBox.start();

        auto topOffset = style.offsets->top;
        if (auto topOffsetCalc = topOffset.is<CalcValue<PercentOr<Length>>>()) {
            top = origin.y + resolve(tree, box, *topOffsetCalc, relativeTo.height);
        }

        auto startOffset = style.offsets->start;
        if (auto startOffsetCalc = startOffset.is<CalcValue<PercentOr<Length>>>()) {
            start = origin.x + resolve(tree, box, *startOffsetCalc, relativeTo.width);
        }

        auto endOffset = style.offsets->end;
        if (auto endOffsetCalc = endOffset.is<CalcValue<PercentOr<Length>>>()) {
            start = (origin.x + relativeTo.width) - resolve(tree, box, *endOffsetCalc, relativeTo.width) - borderBox.width;
        }

        Vec2Au newPositionOffset = Vec2Au{start, top} - borderBox.xy;
        frag->offset(newPositionOffset);

        containingBlock = frag->contentBox();
    }

    for (auto& c : frag->children()) {
        layoutPositioned(tree, c, containingBlock, input);
    }
}

export void lookForRunningPosition(Input& input, Box& box) {
    if (not input.runningPosition)
        return;

    if (box.isRunningPositionedBox()) {
        auto& runningMap = input.runningPosition.peek();
        runningMap.add(input.pageNumber, box);
    }
}

} // namespace Vaev::Layout
