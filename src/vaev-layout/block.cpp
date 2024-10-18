#include "block.h"

#include "float.h"
#include "frag.h"
#include "values.h"

namespace Vaev::Layout {

static Px _blockLayoutDetermineWidth(Tree &t, Frag &f, Input input) {
    Px width = Px{0};
    for (auto &c : f.children()) {
        auto ouput = layout(
            t,
            c,
            {
                .commit = Commit::NO,
                .intrinsic = input.intrinsic,
            }
        );

        width = max(width, ouput.size.x);
    }

    return width;
}

Output blockLayout(Tree &t, Frag &f, Input input) {
    Px blockSize = Px{0};
    Px inlineSize = input.knownSize.width.unwrapOrElse([&] {
        return _blockLayoutDetermineWidth(t, f, input);
    });

    for (auto &c : f.children()) {
        if (c.style->float_ != Float::NONE)
            continue;

        Opt<Px> childInlineSize = NONE;
        if (c.style->sizing->width == Size::AUTO and
            c.style->display != Display::TABLE) {
            childInlineSize = inlineSize;
        }

        Input childInput = {
            .commit = input.commit,
            .availableSpace = {inlineSize, Px{0}},
            .containingBlock = {inlineSize, Px{0}},
        };

        auto margin = computeMargins(t, c, childInput);

        if (c.style->position != Position::ABSOLUTE) {
            blockSize += margin.top;
            childInput.knownSize.width = childInlineSize;
        }

        childInput.position = input.position + Vec2Px{margin.start, blockSize};

        auto ouput = layout(
            t,
            c,
            childInput
        );

        if (c.style->position != Position::ABSOLUTE) {
            blockSize += ouput.size.y + margin.bottom;
        }
    }

    // layoutFloat(t, f, input.containingBlock);

    return Output::fromSize({
        inlineSize,
        blockSize,
    });
}

} // namespace Vaev::Layout
