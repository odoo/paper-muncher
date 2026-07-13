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

    Output run([[maybe_unused]] Tree& tree, Box& box, Input input, [[maybe_unused]] usize startAt, [[maybe_unused]] Opt<usize> stopAt) override {
        auto fragBuilder = FragmentBuilder{tree, box};

        tree.fc.enterMonolithicBox();
        Defer _ = [&] {
            tree.fc.leaveMonolithicBox();
        };

        auto inlineSize = input.knownSize.x.unwrapOrElse([&] {
            if (input.intrinsic == IntrinsicSize::MIN_CONTENT) {
                return 0_au;
            } else if (input.intrinsic == IntrinsicSize::MAX_CONTENT) {
                return Limits<Au>::MAX;
            } else {
                return input.availableSpace.x;
            }
        });

        // NOTE: We are not supposed to get there if the content is not a prose
        auto& prose = box.content.unwrap<Rc<Gfx::Prose>>();

        Vec<Rc<PlaceholderFragment>> outOfFlowChildren = {};

        for (auto strutCell : prose->cellsWithStruts()) {
            auto& boxStrutCell = *strutCell->strut();
            auto& atomicBox = box.children()[boxStrutCell.id];

            if (atomicBox.isRemovedFromFlow())
                continue;

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
                .availableSpace = {inlineSize, input.availableSpace.y},
                .containingBlock = childContainingBlock,
            };

            childInput.knownSize.width = computeSpecifiedBorderBoxWidth(
                tree, atomicBox, atomicBox.style->sizing->width, childInput.containingBlock,
                usedSpacings.padding.horizontal() + usedSpacings.borders.horizontal()
            );

            childInput.knownSize.height = computeSpecifiedBorderBoxHeight(
                tree, atomicBox, atomicBox.style->sizing->height, childInput.containingBlock,
                usedSpacings.padding.vertical() + usedSpacings.borders.vertical()
            );

            // NOTE: We set the same availableSpace to child inline boxes since line wrapping is possible i.e. in the
            // worst case, they will take up the whole availableSpace, and a line break will be done right before them
            auto atomicBoxOutput = layoutBorderBox(tree, atomicBox, childInput);

            boxStrutCell.size = atomicBoxOutput.size;
            // FIXME: hard-coding alphabetic alignment, missing alignment-baseline and dominant-baseline
            boxStrutCell.baseline = getUsedBaselineFromBox(atomicBox, atomicBoxOutput).alphabetic;
        }

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

            if (oneOf(atomicBox.style->position, Keywords::ABSOLUTE, Keywords::FIXED)) {
                if (input.generateFragment) {
                    // https://www.w3.org/TR/css-position-3/#staticpos-rect

                    // TODO:
                    RectAu staticPosRect = {
                        Vec2Au{},
                        Vec2Au{}
                    };

                    auto placeholder = makeRc<PlaceholderFragment>(atomicBox, staticPosRect);

                    fragBuilder.addChild(placeholder);
                    outOfFlowChildren.pushBack(placeholder);
                }

                continue;
            }

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

            outOfFlowChildren.pushBack(output.outOfFlowStash);

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
            .outOfFlowStash = std::move(outOfFlowChildren),
        };
    }
};

export Rc<FormatingContext> constructInlineFormatingContext(Box&) {
    return makeRc<InlineFormatingContext>();
}

} // namespace Vaev::Layout
