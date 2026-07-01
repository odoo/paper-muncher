export module Vaev.Engine:layout.inline_;

import Karm.Gfx;
import Karm.Math;
import Karm.Core;

import :values;
import :layout.base;
import :layout.layout;
import :layout.positioned;

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

    BaselinePositionsSet _computeBaselinePositions(Gfx::FontMetrics metrics, Au baselinePosition) {
        return BaselinePositionsSet{
            .alphabetic = Au{metrics.alphabeticBaseline()},
            .xHeight = Au{metrics.xHeight} + baselinePosition,
            .xMiddle = Au{metrics.xMiddleBaseline()} + baselinePosition,
            .capHeight = Au{metrics.captop} + baselinePosition,
        };
    }

    enum struct StrutMeasureMode {
        Fixed,
        MinContent,
        MaxContent,
    };

    void measureStrutCells(Tree& tree, Box& box, Rc<Gfx::Prose>& prose, StrutMeasureMode mode, bool generateFragment, Vec2Au containingBlock, Au fixedInlineSize = 0_au) {
        for (auto strutCell : prose->cellsWithStruts()) {
            auto& boxStrutCell = *strutCell->strut();
            auto& atomicBox = box.children()[boxStrutCell.id];

            if (atomicBox.isRemovedFromFlow())
                continue;

            UsedSpacings usedSpacings{
                .padding = computePaddings(tree, atomicBox, containingBlock),
                .borders = computeBorders(tree, atomicBox),
            };

            Au strutInline;
            if (mode == StrutMeasureMode::Fixed) {
                strutInline = fixedInlineSize;
            } else {
                auto childIntrinsic = computeIntrinsicInlineSizes(tree, atomicBox);
                strutInline = (mode == StrutMeasureMode::MinContent)
                                  ? childIntrinsic.minContent
                                  : childIntrinsic.maxContent;
            }

            Input childInput{
                .generateFragment = generateFragment,
                .usedSpacings = usedSpacings,
                .availableSpace = {strutInline, 0_au},
                .containingBlock = containingBlock,
            };

            // Author-specified width/height still override the intrinsic constraint.
            childInput.knownSize.width = computeSpecifiedBorderBoxWidth(
                tree, atomicBox, atomicBox.style->sizing->width, childInput.containingBlock,
                usedSpacings.padding.horizontal() + usedSpacings.borders.horizontal()
            );
            childInput.knownSize.height = computeSpecifiedBorderBoxHeight(
                tree, atomicBox, atomicBox.style->sizing->height, childInput.containingBlock,
                usedSpacings.padding.vertical() + usedSpacings.borders.vertical()
            );

            auto atomicBoxOutput = layoutBorderBox(tree, atomicBox, childInput);

            boxStrutCell.size = atomicBoxOutput.size;
            // FIXME: hard-coding alphabetic; missing alignment-baseline / dominant-baseline
            boxStrutCell.baseline = getUsedBaselineFromBox(atomicBox, atomicBoxOutput).alphabetic;
        }
    }

    IntrinsicSizes intrinsicInlineContentSizes(Tree& tree, Box& box) override {
        auto& prose = box.content.unwrap<Rc<Gfx::Prose>>();
        Vec2Au cb = {0_au, 0_au};

        // struts must be sized at THEIR min for min-content, THEIR max for max-content
        measureStrutCells(tree, box, prose, StrutMeasureMode::MaxContent, false, cb);
        Au maxContent = prose->maxContentWidth();

        measureStrutCells(tree, box, prose, StrutMeasureMode::MinContent, false, cb);
        Au minContent = prose->minContentWidth();

        return {minContent, maxContent};
    }

    Output run([[maybe_unused]] Tree& tree, Box& box, Input input, [[maybe_unused]] usize startAt, [[maybe_unused]] Opt<usize> stopAt) override {
        auto fragBuilder = FragmentBuilder{tree, box};

        tree.fc.enterMonolithicBox();
        Defer _ = [&] {
            tree.fc.leaveMonolithicBox();
        };

        auto inlineSize = input.knownSize.x.unwrapOr(input.availableSpace.x);

        // NOTE: We are not supposed to get there if the content is not a prose
        auto& prose = box.content.unwrap<Rc<Gfx::Prose>>();

        auto childCb = Vec2Au{input.knownSize.x.unwrapOr(0_au), input.knownSize.y.unwrapOr(0_au)};
        measureStrutCells(tree, box, prose, StrutMeasureMode::Fixed, input.generateFragment, childCb, inlineSize);

        // FIXME: prose has a ongoing state that is not reset between layout calls, but it should be
        prose->_blocksMeasured = false;
        auto size = prose->layout(inlineSize);

        auto firstBaselineSet = _computeBaselinePositions(prose->_rootSpan->style.font.metrics(), first(prose->_lines).baseline);
        auto lastBaselineSet = _computeBaselinePositions(prose->_rootSpan->style.font.metrics(), last(prose->_lines).baseline);

        for (auto strutCell : prose->cellsWithStruts()) {
            auto runeIdx = strutCell->runeRange.start;
            auto positionInProse = prose->queryPosition(runeIdx);

            auto& boxStrutCell = *strutCell->strut();
            auto& atomicBox = box.children()[boxStrutCell.id];

            // FIXME:
            // Look for running position should be called at linebox generation.
            // Here it could register multiple time the same box.
            lookForRunningPosition(input, atomicBox);

            auto childContainingBlock = Vec2Au{
                input.knownSize.x.unwrapOr(0_au),
                input.knownSize.y.unwrapOr(0_au),
            };

            UsedSpacings usedSpacings{
                .padding = computePaddings(tree, atomicBox, childContainingBlock),
                .borders = computeBorders(tree, atomicBox),
            };

            Input childInput{
                .generateFragment = input.generateFragment,
                .usedSpacings = usedSpacings,
                .knownSize = {
                    boxStrutCell.size.x,
                    boxStrutCell.size.y
                },
                .position = input.position + positionInProse,
                .containingBlock = childContainingBlock,
                .runningPosition = input.runningPosition,
                .pageNumber = input.pageNumber,
            };

            if (atomicBox.isRemovedFromFlow()) {
                childInput.knownSize.width = computeSpecifiedBorderBoxWidth(
                    tree, atomicBox, atomicBox.style->sizing->width, childInput.containingBlock,
                    usedSpacings.padding.horizontal() + usedSpacings.borders.horizontal()
                );

                childInput.knownSize.height = computeSpecifiedBorderBoxHeight(
                    tree, atomicBox, atomicBox.style->sizing->height, childInput.containingBlock,
                    usedSpacings.padding.vertical() + usedSpacings.borders.vertical()
                );
            }

            auto output = layoutBorderBox(tree, atomicBox, childInput);

            if (auto [frag] = output.fragment)
                fragBuilder.addChild(frag);
        }

        if (tree.fc.allowBreak() and
            not tree.fc.acceptsFit(
                input.position.y,
                size.y,
                input.pendingVerticalSizes
            )) {
            return {
                .fragment = fragBuilder.buildBoxFromInput(input, {}),
                .size = {},
                .completelyLaidOut = false,
                .breakpoint = Breakpoint::overflow()
            };
        }

        auto outputSize = Vec2Au{
            input.knownSize.x.unwrapOr(size.x),
            input.knownSize.y.unwrapOr(size.y),
        };

        return {
            .fragment = fragBuilder.buildBoxFromInput(input, outputSize),
            .size = outputSize,
            .completelyLaidOut = true,
            .breakpoint = Breakpoint::bottomOfMonolithicBox(box),
            .firstBaselineSet = firstBaselineSet,
            .lastBaselineSet = lastBaselineSet,
        };
    }
};

export Rc<FormatingContext> constructInlineFormatingContext(Box&) {
    return makeRc<InlineFormatingContext>();
}

} // namespace Vaev::Layout
