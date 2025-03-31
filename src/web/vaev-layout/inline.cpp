module;

#include <karm-base/func.h>
#include <karm-text/prose.h>
#include <vaev-base/display.h>
#include <vaev-base/keywords.h>

export module Vaev.Layout:inline_;

import :base;
import :layout;

namespace Vaev::Layout {

struct InlineFormatingContext : public FormatingContext {

    // https://www.w3.org/TR/css-inline-3/#baseline-source
    BaselinePositionsSet getUsedBaselineFromBox(Box const& childBox, Output const& output) {
        if (childBox.style->baseline->source == Keywords::FIRST)
            return output.firstBaselineSet;
        if (childBox.style->baseline->source == Keywords::LAST)
            return output.lastBaselineSet;

        if (childBox.style->display == Display::INLINE and childBox.style->display == Display::FLOW_ROOT)
            return output.lastBaselineSet;
        return output.firstBaselineSet;
    }

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

        // FIXME: prose has a ongoing state that is not reset between layout calls, but it should be
        prose->_blocksMeasured = false;

        struct InlineOutput {
            Vec2Au size;
            BaselinePositionsSet baselineSet;
        };

        Map<usize, InlineOutput> atomicBoxData;

        auto size = prose->layout(inlineSize, [&](Rc<Text::Prose::CellContent> cell) -> Au {
            if (not cell.is<InlineBox::BoxStrutCell>())
                panic("strut cell to be measured needs to be of type BoxStrutCell");

            auto const& boxStrutCell = cell.unwrap<InlineBox::BoxStrutCell>();
            auto& atomicBox = *inlineBox.atomicBoxes[boxStrutCell.boxIndex];

            // We set the same availableSpace to child inline boxes since line wrapping is possible i.e. in the worst
            // case, they will take up the whole availableSpace, and a line break will be done right before them
            auto atomicBoxOutput = layout(
                tree,
                atomicBox,
                Input{
                    .knownSize = {NONE, NONE},
                    .availableSpace = {inlineSize, input.availableSpace.y},
                }
            );

            atomicBoxData.put(
                boxStrutCell.boxIndex,
                {atomicBoxOutput.size, getUsedBaselineFromBox(atomicBox, atomicBoxOutput)}
            );

            return atomicBoxOutput.size.x;
        });

        auto firstBaselineSet = _computeBaselinePositions(inlineBox, first(prose->_lines).baseline);
        auto lastBaselineSet = _computeBaselinePositions(inlineBox, last(prose->_lines).baseline);

        for (auto strutCell : prose->cellsWithStruts()) {
            auto runeIdx = strutCell->runeRange.start;
            auto position = prose->queryPosition(runeIdx);

            auto& boxStrutCell = strutCell->_content.unwrap<InlineBox::BoxStrutCell>();

            if (not atomicBoxData.has(boxStrutCell.boxIndex)) {
                logWarn("could not find layout output data from box strut cell: index {}", boxStrutCell.boxIndex);
                continue;
            }

            Math::Vec2<Opt<Au>> knownSize = {
                atomicBoxData.get(boxStrutCell.boxIndex).size.x,
                atomicBoxData.get(boxStrutCell.boxIndex).size.y
            };

            auto childBaselineSet = atomicBoxData.get(boxStrutCell.boxIndex).baselineSet;

            auto alignedPosition = input.position + position;

            // FIXME: hard-coding alphabetic alignment, missing alignment-baseline and dominant-baseline
            alignedPosition.y -= childBaselineSet.alphabetic;

            layout(
                tree,
                inlineBox.atomicBoxes[boxStrutCell.boxIndex],
                Input{
                    .fragment = input.fragment,
                    .knownSize = knownSize,
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
            .firstBaselineSet = firstBaselineSet,
            .lastBaselineSet = lastBaselineSet,
        };
    }
};

export Rc<FormatingContext> constructInlineFormatingContext(Box&) {
    return makeRc<InlineFormatingContext>();
}

} // namespace Vaev::Layout
