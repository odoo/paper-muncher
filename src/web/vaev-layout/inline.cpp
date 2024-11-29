#include "inline.h"

#include "box.h"
#include "layout.h"

namespace Vaev::Layout {

Output inlineLayout(Tree &tree, Box &box, Input input) {
    // NOTE: We are not supposed to get there if the content is not a prose
    auto &[prose, boxes] =
        box.content.unwrap<Inline>();

    // Do inline box layout
    for (usize i = 0; i < boxes.len(); i++) {
        auto &child = boxes[i];
        Opt<Px> knowWidth = NONE;

        // If 'width' is 'auto', the used value is the shrink-to-fit
        // width as for floating elements.
        if (child.style->sizing->width == Size::AUTO) {
            knowWidth =
                computeShrinkToFitWidth(
                    tree, child,
                    input.containingBlock,
                    input.availableSpace
                );
        }

        auto size =
            layout(
                tree, child,
                {.knownSize = {knowWidth, NONE}}
            ).size;

        auto &inlineBox = prose->_boxes[i];
        inlineBox.size = size.cast<f64>();
    }

    // Do line box layout
    auto inlineSize = input.knownSize.x.unwrapOrElse(
        [&] {
            if (input.intrinsic == IntrinsicSize::MIN_CONTENT) {
                return 0_px;
            } else if (input.intrinsic == IntrinsicSize::MAX_CONTENT) {
                return Limits<Px>::MAX;
            } else {
                return input.availableSpace.x;
            }
        }
    );

    auto res = prose->layout(inlineSize.cast<f64>());

    // Commit inline box sizes
    for (usize i = 0; i < boxes.len(); i++) {
        auto &box = boxes[i];
        auto &inlineBox = prose->_boxes[i];

        layout(
            tree, box,
            {
                .commit = Commit::YES,
                .knownSize = inlineBox.size.cast<Opt<Px>>(),
                .position = input.position + inlineBox.pos.cast<Px>(),
            }
        );
    }

    return Output::fromSize(res.cast<Px>());
}

} // namespace Vaev::Layout
