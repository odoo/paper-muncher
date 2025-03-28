module;

#include <karm-base/func.h>
#include <karm-text/prose.h>

export module Vaev.Layout:inline_;

import :base;
import :layout;

namespace Vaev::Layout {

struct InlineFormatingContext : public FormatingContext {

    BaselinePositionsSet _computeBaselinePositions(InlineBox& inlineBox, Au baselinePosition) {
        // FIXME
        auto nonConstCopy = inlineBox._style.font;
        auto baselineSet = nonConstCopy.baselineSet();

        return BaselinePositionsSet{
            .alphabetic = Au{baselineSet.alphabetic} + baselinePosition,
            .xHeight = Au{baselineSet.xHeight} + baselinePosition,
            .xMiddle = Au{baselineSet.xMiddle} + baselinePosition,
            .capHeight = Au{baselineSet.capHeight} + baselinePosition,
        };
    }

    virtual Output run([[maybe_unused]] Tree& tree, Box& box, Input input, [[maybe_unused]] usize startAt, [[maybe_unused]] Opt<usize> stopAt) override {
        // NOTE: We are not supposed to get there if the content is not a prose
        auto& inlineBox = box.content.unwrap<InlineBox>("inlineLayout");

        auto inlineSize = input.knownSize.x.unwrapOrElse([&] {
            if (input.intrinsic == IntrinsicSize::MIN_CONTENT) {
                return 0_au;
            } else if (input.intrinsic == IntrinsicSize::MAX_CONTENT) {
                return Limits<Au>::MAX;
            } else {
                return input.availableSpace.x;
            }
        });

        auto& prose = inlineBox.prose;

        auto size = prose->layout(inlineSize, [&](Rc<Text::Prose::CellContent> cell) -> Au {
            if (not cell.is<InlineBox::BoxStrutCell>())
                panic("xiiii");

            auto& boxStrutCell = cell.unwrap<InlineBox::BoxStrutCell>();

            return layout(
                       tree,
                       inlineBox.atomicBoxes[boxStrutCell.boxIndex],
                       Input{
                           .knownSize = {NONE, NONE},
                       }
            )
                .size.x;
        });

        auto baselineSetPositions = _computeBaselinePositions(inlineBox, prose->_lines[0].baseline);

        for (auto strutCell : prose->cellsWithStruts()) {
            auto runeIdx = strutCell->runeRange.start;
            auto position = prose->queryPosition(runeIdx);

            auto& boxStrutCell = strutCell->_content.unwrap<InlineBox::BoxStrutCell>();

            // logDebug("estou mandando a box {}", inlineBox.atomicBoxes[boxStrutCell.boxIndex]);

            auto outputWithBaselines = layout(
                tree,
                inlineBox.atomicBoxes[boxStrutCell.boxIndex],
                Input{
                    .knownSize = {NONE, NONE},
                    .position = input.position + position,
                }
            );

            auto childBaselineSet = outputWithBaselines.baselineSet;

            // logDebug("mandei a box {} e ganhei {}", inlineBox.atomicBoxes[boxStrutCell.boxIndex], childBaselineSet);

            auto alignedPosition = input.position + position;
            alignedPosition.y -= childBaselineSet.alphabetic;

            layout(
                tree,
                inlineBox.atomicBoxes[boxStrutCell.boxIndex],
                Input{
                    .fragment = input.fragment,
                    .knownSize = {NONE, NONE},
                    .position = alignedPosition,
                }
            );
        }

        if (tree.fc.allowBreak() and not tree.fc.acceptsFit(
                                         input.position.y,
                                         size.y,
                                         input.pendingVerticalSizes
                                     )) {
            return {
                .size = {},
                .completelyLaidOut = false,
                .breakpoint = Breakpoint::overflow()
            };
        }

        return {
            .size = size,
            .completelyLaidOut = true,
            .baselineSet = baselineSetPositions,
        };
    }
};

export Rc<FormatingContext> constructInlineFormatingContext(Box&) {
    return makeRc<InlineFormatingContext>();
}

} // namespace Vaev::Layout
