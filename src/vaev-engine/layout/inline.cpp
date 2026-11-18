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
        if (childBox.style->inline_->baselineSource == Keywords::FIRST)
            return output.firstBaselineSet;
        if (childBox.style->inline_->baselineSource == Keywords::LAST)
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

    // https://www.w3.org/TR/CSS22/visuren.html#floats
    // However, the current and subsequent line boxes created next to the float are shortened as necessary to make room
    // for the margin box of the float.
    // NOSPEC: Line boxes are not shortened around floats. A float is put at the top of the first line, starting from the
    //         line it appears on, whose text and floats leave room for it, or below the last line.
    static Vec2Au _floatPositionInProse(Gfx::Prose const& prose, Floats const& placed, usize runeIndex, FloatSide side, Au marginWidth, Au inlineSize, Au proseHeight) {
        Range<Au> container = {0_au, inlineSize};
        Au minTop = placed.topNotAboveEarlierFloats(0_au);

        for (auto const& line : prose._lines) {
            if (line.runeRange.end() <= runeIndex)
                continue;

            Au top = line.baseline - line.ascent;
            if (top < minTop or not placed.fits(side, {marginWidth, 0_au}, top, container))
                continue;

            Au x = alignFloatInBand(side, placed.freeBand(top, 0_au, container), marginWidth);
            auto blocks = line.blocks(prose);
            if (isEmpty(blocks))
                return {x, top};

            Au textStart = first(blocks).pos;
            Au textEnd = last(blocks).pos + last(blocks).width;
            if (side == FloatSide::LEFT ? x + marginWidth <= textStart : textEnd <= x)
                return {x, top};
        }

        return placed.place(side, {marginWidth, 0_au}, proseHeight, container).xy;
    }

    static UsedSpacings _usedSpacings(Tree& tree, Box& box, Vec2Au containingBlock) {
        return {
            .padding = computePaddings(tree, box, containingBlock),
            .borders = computeBorders(tree, box),
            .margin = computeMargins(tree, box, containingBlock),
        };
    }

    // https://www.w3.org/TR/CSS22/visudet.html#float-width
    // If 'width' is computed as 'auto', the used value is the "shrink-to-fit" width.
    static Au _floatBorderBoxWidth(Tree& tree, Box& box, UsedSpacings const& usedSpacings, Vec2Au containingBlock, IntrinsicSize intrinsic) {
        Au borderBoxSpacings = usedSpacings.padding.horizontal() + usedSpacings.borders.horizontal();
        if (auto width = computeSpecifiedBorderBoxWidth(tree, box, box.style->sizing->width, containingBlock, borderBoxSpacings))
            return width.expect();

        if (isMinMaxIntrinsicSize(intrinsic))
            return computeIntrinsicContentSize(tree, box, intrinsic).x + borderBoxSpacings;

        Au availableWidth = containingBlock.x - usedSpacings.margin.horizontal() - borderBoxSpacings;
        return computeFitContentInlineSize(tree, box, availableWidth) + borderBoxSpacings;
    }

    Output run([[maybe_unused]] Tree& tree, Box& box, Input input, [[maybe_unused]] usize startAt, [[maybe_unused]] Opt<usize> stopAt) override {
        auto fragBuilder = FragmentBuilder{tree, box};

        tree.fc.enterMonolithicBox();
        Defer _ = [&] {
            tree.fc.leaveMonolithicBox();
        };

        auto inlineSize = input.knownSize.width.unwrapOrElse([&] {
            if (input.intrinsic == IntrinsicSize::MIN_CONTENT) {
                return 0_au;
            } else if (input.intrinsic == IntrinsicSize::MAX_CONTENT) {
                return Limits<Au>::MAX;
            } else {
                return input.availableSpace.x;
            }
        });

        // NOTE: We are not supposed to get there if the content is not a prose
        auto& prose = box.content.expect<Rc<Gfx::Prose>>();

        Vec<Rc<PlaceholderFragment>> outOfFlowChildren = {};

        for (auto strutCell : prose->cellsWithStruts()) {
            auto boxStrutCell = strutCell->strut(*prose);
            auto& atomicBox = box.children()[boxStrutCell->id];

            // NOSPEC: Floats take no room on their line, except when sizing under a min-content or max-content
            //         constraint where they count like an inline-block, so that shrink-to-fit containers fit them.
            if (atomicBox.isFloating()) {
                boxStrutCell->size = {};
                if (isMinMaxIntrinsicSize(input.intrinsic)) {
                    auto childContainingBlock = Vec2Au{
                        input.knownSize.width.unwrapOr(0_au),
                        input.knownSize.height.unwrapOr(0_au),
                    };
                    auto usedSpacings = _usedSpacings(tree, atomicBox, childContainingBlock);
                    boxStrutCell->size.x = _floatBorderBoxWidth(tree, atomicBox, usedSpacings, childContainingBlock, input.intrinsic) + usedSpacings.margin.horizontal();
                }
                continue;
            }

            if (atomicBox.isRemovedFromFlow())
                continue;

            auto childContainingBlock = Vec2Au{
                input.knownSize.width.unwrapOr(0_au),
                input.knownSize.height.unwrapOr(0_au),
            };

            UsedSpacings usedSpacings{
                .padding = computePaddings(tree, atomicBox, childContainingBlock),
                .borders = computeBorders(tree, atomicBox),
            };

            Input childInput{
                .generateFragment = false,
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

            boxStrutCell->size = atomicBoxOutput.size;
            // FIXME: hard-coding alphabetic alignment, missing alignment-baseline and dominant-baseline
            boxStrutCell->baseline = getUsedBaselineFromBox(atomicBox, atomicBoxOutput).alphabetic;
        }

        if (prose->_struts)
            prose->_blocksMeasured = false;
        auto size = prose->layout(inlineSize);

        auto firstBaselineSet = _computeBaselinePositions(prose->_rootSpan->style.font.metrics(), first(prose->_lines).baseline);
        auto lastBaselineSet = _computeBaselinePositions(prose->_rootSpan->style.font.metrics(), last(prose->_lines).baseline);

        // NOTE: Floats can only be placed once the inline size is known.
        bool placesFloats = input.knownSize.width.has() and not isMinMaxIntrinsicSize(input.intrinsic);
        Floats placedFloats;
        Au contentHeight = size.y;

        // FIXME: Floats in inline content are not fragmented, the part of a float below the fragmentainer's end is lost.
        if (placesFloats) {
            for (auto strutCell : prose->cellsWithStruts()) {
                auto boxStrutCell = strutCell->strut(*prose);
                auto& atomicBox = box.children()[boxStrutCell->id];

                if (not atomicBox.isFloating())
                    continue;

                auto childContainingBlock = Vec2Au{inlineSize, input.knownSize.height.unwrapOr(0_au)};
                auto usedSpacings = _usedSpacings(tree, atomicBox, childContainingBlock);

                Au width = _floatBorderBoxWidth(tree, atomicBox, usedSpacings, childContainingBlock, input.intrinsic);
                Au marginWidth = width + usedSpacings.margin.horizontal();

                auto side = resolveFloatSide(atomicBox.style->float_, box.style->direction);
                auto positionInProse = _floatPositionInProse(*prose, placedFloats, strutCell->runeRange.start, side, marginWidth, inlineSize, size.y);

                Input childInput{
                    .generateFragment = input.generateFragment,
                    .usedSpacings = usedSpacings,
                    .knownSize = {
                        Some(width),
                        computeSpecifiedBorderBoxHeight(
                            tree, atomicBox, atomicBox.style->sizing->height, childContainingBlock,
                            usedSpacings.padding.vertical() + usedSpacings.borders.vertical()
                        ),
                    },
                    .position = input.position + positionInProse + usedSpacings.margin.topStart(),
                    .containingBlock = childContainingBlock,
                    .runningPosition = input.runningPosition,
                    .pageNumber = input.pageNumber,
                };

                auto output = layoutBorderBox(tree, atomicBox, childInput);
                outOfFlowChildren.pushBack(output.outOfFlowStash);
                fragBuilder.addChildIfAny(output.fragment);

                Au marginHeight = output.size.y + usedSpacings.margin.vertical();
                placedFloats.add({side, {positionInProse, {marginWidth, marginHeight}}});
                contentHeight = max(contentHeight, positionInProse.y + marginHeight);
            }
        }

        if (input.generateFragment) {
            for (auto strutCell : prose->cellsWithStruts()) {
                auto runeIdx = strutCell->runeRange.start;
                auto positionInProse = prose->queryPosition(runeIdx);

                auto boxStrutCell = strutCell->strut(*prose);
                auto& atomicBox = box.children()[boxStrutCell->id];

                if (placesFloats and atomicBox.isFloating())
                    continue;

                if (oneOf(atomicBox.style->position, Keywords::ABSOLUTE, Keywords::FIXED)) {
                    if (input.generateFragment) {
                        // https://www.w3.org/TR/css-position-3/#staticpos-rect

                        // TODO:
                        RectAu staticPosRect = {
                            Vec2Au{},
                            Vec2Au{}
                        };

                        auto placeholder = makeRc<PlaceholderFragment>(atomicBox, staticPosRect);

                        fragBuilder.addChildIfAny(Some(placeholder));
                        outOfFlowChildren.pushBack(placeholder);
                    }

                    continue;
                }

                // FIXME:
                // Look for running position should be called at linebox generation.
                // Here it could register multiple time the same box.
                lookForRunningPosition(input, atomicBox);

                auto childContainingBlock = Vec2Au{
                    input.knownSize.width.unwrapOr(0_au),
                    input.knownSize.height.unwrapOr(0_au),
                };

                UsedSpacings usedSpacings{
                    .padding = computePaddings(tree, atomicBox, childContainingBlock),
                    .borders = computeBorders(tree, atomicBox),
                };

                Input childInput{
                    .generateFragment = input.generateFragment,
                    .usedSpacings = usedSpacings,
                    .knownSize = {
                        Some(boxStrutCell->size.x),
                        Some(boxStrutCell->size.y)
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

                if (atomicBox.style->position == Keywords::RELATIVE) {
                    childInput.position += relativePositionOffset(tree, atomicBox, input.containingBlock);
                }

                auto output = layoutBorderBox(tree, atomicBox, childInput);

                outOfFlowChildren.pushBack(output.outOfFlowStash);
                fragBuilder.addChildIfAny(output.fragment);
            }
        }

        if (tree.fc.allowBreak() and
            not tree.fc.acceptsFit(
                input.position.y,
                size.y,
                input.pendingVerticalSizes,
                input.atFragmentainerStart
            )) {
            return {
                .fragment = fragBuilder.buildBoxFromInput(input, {}),
                .size = {},
                .completelyLaidOut = false,
                .breakpoint = Some(Breakpoint::overflow())
            };
        }

        // https://www.w3.org/TR/CSS22/visudet.html#normal-block
        // Only children in the normal flow are taken into account (i.e., floating boxes and absolutely positioned boxes
        // are ignored, and relatively positioned boxes are considered without their offset).
        // NOSPEC: Like in block layout, every box grows to include the floats it places.
        auto outputSize = Vec2Au{
            input.knownSize.width.unwrapOr(size.x),
            input.knownSize.height.unwrapOr(contentHeight),
        };

        return {
            .fragment = fragBuilder.buildBoxFromInput(input, outputSize),
            .size = outputSize,
            .completelyLaidOut = true,
            .breakpoint = Some(Breakpoint::bottomOfMonolithicBox(box)),
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
