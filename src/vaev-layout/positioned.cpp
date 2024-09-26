#include "positioned.h"

namespace Vaev::Layout {

void layoutPositioned(Tree &t, Frag &f, RectPx containingBlock) {
    if (f.style->position == Position::ABSOLUTE or f.style->position == Position::RELATIVE) {
        auto origin = containingBlock.topStart();
        if (f.style->position == Position::RELATIVE)
            origin = f.layout.position;

        auto top = f.layout.borderBox().top();
        auto start = f.layout.borderBox().start();

        auto topOffset = f.style->offsets->top;
        if (topOffset != Width::AUTO) {
            top = origin.y + resolve(t, f, topOffset, containingBlock.height);
        }

        auto startOffset = f.style->offsets->start;
        if (startOffset != Width::AUTO) {
            start = origin.x + resolve(t, f, startOffset, containingBlock.width);
        }

        layout(
            t,
            f,
            {
                .commit = Commit::YES,
                .knownSize = f.layout.borderBox().size().cast<Opt<Px>>(),
                .position = {start, top},
            }
        );

        containingBlock = f.layout.contentBox();
    }

    for (auto &c : f.children()) {
        layoutPositioned(t, c, containingBlock);
    }
}
} // namespace Vaev::Layout
