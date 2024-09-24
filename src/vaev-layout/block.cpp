#include "block.h"

#include "frag.h"
#include "values.h"

namespace Vaev::Layout {

static Px _blockLayoutDetermineWidth(Tree &t, Frag &f, Input input) {
    Px width = Px{0};
    for (auto &c : f.children()) {

        if (c.style->sizing->width == Size::AUTO)
            width = max(width, input.knownSize.width.unwrapOr(Px{0}));
        else {
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
    }

    return width;
}

static InsetsPx _computeOffsets(Tree &t, Frag &f, Input input) {
    InsetsPx res;
    auto offsets = f.style->offsets;
    res.top = resolve(t, f, offsets->top, input.containingBlock.height);
    res.end = resolve(t, f, offsets->end, input.containingBlock.width);
    res.bottom = resolve(t, f, offsets->bottom, input.containingBlock.height);
    res.start = resolve(t, f, offsets->start, input.containingBlock.width);
    return res;
}

Output blockLayout(Tree &t, Frag &f, Input input) {
    Px blockSize = Px{0};
    Px inlineSize = input.knownSize.x.unwrapOrElse([&] {
        return _blockLayoutDetermineWidth(t, f, input);
    });

    for (auto &c : f.children()) {
        Opt<Px> childInlineSize = NONE;
        if (c.style->sizing->width == Size::AUTO)
            childInlineSize = inlineSize;

        auto ouput = layout(
            t,
            c,
            {
                .commit = input.commit,
                .knownSize = {childInlineSize, NONE},
                .availableSpace = {inlineSize, Px{0}},
                .containingBlock = {inlineSize, Px{0}},
            }
        );

        if (input.commit == Commit::YES) {
            auto offsets = _computeOffsets(t, c, input);
            c.layout.position = {Px{0} + offsets.start, blockSize + offsets.top};
        }

        if (c.style->position != Position::ABSOLUTE) {
            blockSize += ouput.size.y;
        }
    }

    return Output::fromSize({
        inlineSize,
        blockSize,
    });
}

} // namespace Vaev::Layout
