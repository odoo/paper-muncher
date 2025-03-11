module;

#include <vaev-base/insets.h>

export module Vaev.Layout:positioned;

import :layout;
import :values;

namespace Vaev::Layout {

export void layoutPositioned(Tree& tree, Frag& frag, RectAu containingBlock) {
    auto& style = frag.style();
    auto& metrics = frag.metrics;

    if (style.position == Position::ABSOLUTE or style.position == Position::RELATIVE) {
        auto origin = containingBlock.topStart();
        if (style.position == Position::RELATIVE)
            origin = metrics.position;

        auto top = metrics.position.y;
        auto start = metrics.position.x;

        auto topOffset = style.offsets->top;
        if (auto topOffsetCalc = topOffset.is<CalcValue<PercentOr<Length>>>()) {
            top = origin.y + resolve(tree, *frag.box, *topOffsetCalc, containingBlock.height);
        }

        auto startOffset = style.offsets->start;
        if (auto startOffsetCalc = startOffset.is<CalcValue<PercentOr<Length>>>()) {
            start = origin.x + resolve(tree, *frag.box, *startOffsetCalc, containingBlock.width);
        }

        auto endOffset = frag.style().offsets->end;
        if (auto endOffsetCalc = endOffset.is<CalcValue<PercentOr<Length>>>()) {
            start = (origin.x + containingBlock.width) - resolve(tree, *frag.box, *endOffsetCalc, containingBlock.width) - metrics.borderSize.width;
        }

        Vec2Au newPositionOffset = Vec2Au{start, top} - metrics.position;
        frag.offset(newPositionOffset);

        containingBlock = metrics.contentBox();
    }

    for (auto& c : frag.children) {
        layoutPositioned(tree, c, containingBlock);
    }
}

} // namespace Vaev::Layout
