#include "positioned.h"

#include "layout.h"

namespace Vaev::Layout {

void layoutPositioned(Tree &t, Frag &f, RectPx containingBlock) {
    if (f.style().position == Position::ABSOLUTE or f.style().position == Position::RELATIVE) {
        auto origin = containingBlock.topStart();
        if (f.style().position == Position::RELATIVE)
            origin = f.metrics.position;

        auto top = f.metrics.position.y;
        auto start = f.metrics.position.x;

        auto topOffset = f.style().offsets->top;
        if (topOffset != Width::AUTO) {
            top = origin.y + resolve(t, f.box, topOffset, containingBlock.height);
        }

        auto startOffset = f.style().offsets->start;
        if (startOffset != Width::AUTO) {
            start = origin.x + resolve(t, f.box, startOffset, containingBlock.width);
        }

        auto endOffset = f.style().offsets->end;
        if (endOffset != Width::AUTO) {
            start = (origin.x + containingBlock.width) - resolve(t, f, endOffset, containingBlock.width) - f.metrics.borderSize.width;
        }

        f.offset(Vec2Px{start, top} - f.metrics.position);

        containingBlock = f.metrics.contentBox();
    }

    for (auto &c : f.children) {
        layoutPositioned(t, c, containingBlock);
    }
}
} // namespace Vaev::Layout
