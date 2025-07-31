module;

#include <karm-text/prose.h>

export module Vaev.Engine:layout.inline_;

import :values;
import :layout.base;
import :layout.layout;

namespace Vaev::Layout {

struct InlineFormatingContext : FormatingContext {

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
        auto metrics = inlineBox.prose->_style.font.metrics();

        return BaselinePositionsSet{
            .alphabetic = Au{metrics.alphabeticBaseline()} + baselinePosition,
            .xHeight = Au{metrics.xHeight} + baselinePosition,
            .xMiddle = Au{metrics.xMiddleBaseline()} + baselinePosition,
            .capHeight = Au{metrics.captop} + baselinePosition,
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

        for (auto strutCell : prose->cellsWithStruts()) {
            auto& boxStrutCell = *strutCell->strut();

            auto& atomicBox = *inlineBox.atomicBoxes[boxStrutCell.id];

            Input childInput{
                .knownSize = {NONE, NONE},
                .availableSpace = {inlineSize, input.availableSpace.y},
                .containingBlock = {
                    input.knownSize.x.unwrapOr(0_au),
                    input.knownSize.y.unwrapOr(0_au)
                },
            };

            auto borders = computeBorders(tree, atomicBox);
            auto padding = computePaddings(tree, atomicBox, childInput.containingBlock);

            if (input.intrinsic == IntrinsicSize::AUTO or atomicBox.style->display != Display::INLINE) {
                if (atomicBox.style->sizing->width.is<Keywords::Auto>()) {
                    childInput.knownSize.width = NONE;
                } else {
                    // FIXME: computing border box size. content box sizing should be supported later.
                    auto specifiedWidth = computeSpecifiedSize(
                        tree, atomicBox, atomicBox.style->sizing->width, childInput.containingBlock, true
                    );

                    childInput.knownSize.width = specifiedWidth.unwrap() - borders.horizontal() - padding.horizontal();
                }

                if (atomicBox.style->sizing->height.is<Keywords::Auto>()) {
                    childInput.knownSize.height = NONE;
                } else {
                    // FIXME: computing border box size. content box sizing should be supported later.
                    auto specifiedHeight = computeSpecifiedSize(
                        tree, atomicBox, atomicBox.style->sizing->height, childInput.containingBlock, false
                    );

                    childInput.knownSize.height = specifiedHeight.unwrap() - borders.vertical() - padding.vertical();
                }
            }

            // NOTE: We set the same availableSpace to child inline boxes since line wrapping is possible i.e. in the
            // worst case, they will take up the whole availableSpace, and a line break will be done right before them
            auto atomicBoxOutput = layoutContentBox(
                tree,
                atomicBox,
                childInput
            );

            if (not impliesRemovingFromFlow(atomicBox.style->position)) {
                boxStrutCell.size = atomicBoxOutput.size + borders.all() + padding.all();
                // FIXME: hard-coding alphabetic alignment, missing alignment-baseline and dominant-baseline
                boxStrutCell.baseline = getUsedBaselineFromBox(atomicBox, atomicBoxOutput).alphabetic;
            }
        }

        // FIXME: prose has a ongoing state that is not reset between layout calls, but it should be
        prose->_blocksMeasured = false;
        auto size = prose->layout(inlineSize);

        auto firstBaselineSet = _computeBaselinePositions(inlineBox, first(prose->_lines).baseline);
        auto lastBaselineSet = _computeBaselinePositions(inlineBox, last(prose->_lines).baseline);

        for (auto strutCell : prose->cellsWithStruts()) {
            auto runeIdx = strutCell->runeRange.start;
            auto positionInProse = prose->queryPosition(runeIdx);

            auto& boxStrutCell = *strutCell->strut();
            auto& atomicBox = *inlineBox.atomicBoxes[boxStrutCell.id];

            Math::Vec2<Opt<Au>> knownSize;
            if (not impliesRemovingFromFlow(atomicBox.style->position)) {
                knownSize = {
                    boxStrutCell.size.x,
                    boxStrutCell.size.y
                };
            }

            // TODO: pending vertical sizes here?
            Input childInput{
                .fragment = input.fragment,
                .knownSize = knownSize,
                .position = input.position + positionInProse,
                .containingBlock = {
                    input.knownSize.x.unwrapOr(0_au),
                    input.knownSize.y.unwrapOr(0_au)
                },
            };

            auto borders = computeBorders(tree, atomicBox);
            auto padding = computePaddings(tree, atomicBox, childInput.containingBlock);

            childInput.knownSize.x = childInput.knownSize.x.map(
                [&](Au size) {
                    return size - borders.horizontal() - padding.horizontal();
                }
            );

            childInput.knownSize.y = childInput.knownSize.y.map(
                [&](Au size) {
                    return size - borders.vertical() - padding.vertical();
                }
            );

            childInput.position = childInput.position + borders.topStart() + padding.topStart();

            auto output = layoutContentBox(tree, atomicBox, childInput);

            if (input.fragment) {
                auto& child = input.fragment->children()[input.fragment->children().len() - 1];
                child.metrics = Metrics::commitContentBox(
                    tree, atomicBox,
                    output.size, childInput.position,
                    borders, padding
                );
            }
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
            .size = {input.knownSize.x.unwrapOr(size.x), input.knownSize.y.unwrapOr(size.y)},
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
