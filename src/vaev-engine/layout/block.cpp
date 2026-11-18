module;

#include <karm/macros>

export module Vaev.Engine:layout.block;

import Karm.Gfx;
import Karm.Math;
import Karm.Core;

import :values;
import :layout.base;
import :layout.layout;
import :layout.positioned;

namespace Vaev::Layout {

void maybeProcessChildBreakpoint(Fragmentainer& fc, Breakpoint& currentBreakpoint, usize childIndex, bool currBoxIsBreakAvoid, Opt<Breakpoint> maybeChildBreakpoint) {
    if (not fc.isDiscoveryMode())
        return;

    // if we are in a monolitic context, we might not have breakpoints
    if (not maybeChildBreakpoint)
        return;

    // breakpoint inside child (from this blocks perspective)
    // BREAK CLASS X (recursive case)
    currentBreakpoint.overrideIfBetter(
        Breakpoint::fromChild(
            std::move(maybeChildBreakpoint.expect()),
            childIndex + 1,
            currBoxIsBreakAvoid
        )
    );
}

Res<None, Output> processBreakpointsAfterChild(Fragmentainer const& fc, FragmentBuilder& fragBuilder, Breakpoint& currentBreakpoint, Box& parentBox, usize childIndex, Vec2Au currentBoxSize, bool childCompletelyLaidOut, Input input) {
    if (not fc.isDiscoveryMode())
        return Ok(NONE);

    // last child was not completly laid out, we need to abort with our best breakpoint
    if (not childCompletelyLaidOut) {
        return Output{
            .fragment = fragBuilder.buildBoxFromInput(input, currentBoxSize),
            .size = currentBoxSize,
            .completelyLaidOut = false,
            .breakpoint = Some(currentBreakpoint)
        };
    }

    Box& childBox = parentBox.children()[childIndex];
    bool isLastChild = childIndex + 1 == parentBox.children().len();

    // breakpoint right after child
    // this can only happen IF child was fully laid out and its not last child (not class C)
    // BREAK CLASS A
    if (not isLastChild) {
        bool breakIsAvoided =
            oneOf(parentBox.style->break_->inside, BreakInside::AVOID, BreakInside::AVOID_PAGE) or
            oneOf(childBox.style->break_->after, BreakBetween::AVOID, BreakBetween::AVOID_PAGE) or
            (not isLastChild and oneOf(parentBox.children()[childIndex + 1].style->break_->before, BreakBetween::AVOID, BreakBetween::AVOID_PAGE));

        currentBreakpoint.overrideIfBetter(
            Breakpoint::classB(
                childIndex + 1,
                breakIsAvoided
            )
        );
    }

    // FORCED BREAK
    if (childBox.style->break_->after == BreakBetween::PAGE) {
        return Output{
            .fragment = fragBuilder.buildBoxFromInput(input, currentBoxSize),
            .size = currentBoxSize,
            .completelyLaidOut = false,
            .breakpoint = Some(Breakpoint::forced(childIndex + 1))
        };
    }

    return Ok(NONE);
}

Res<None, Output> processBreakpointsBeforeChild(FragmentBuilder& fragBuilder, usize endAt, Vec2Au currentSize, bool forcedBreakBefore, usize startAt, Input input) {
    // FORCED BREAK
    if (forcedBreakBefore and not(startAt == endAt)) {
        return Output{
            .fragment = fragBuilder.buildBoxFromInput(input, currentSize),
            .size = currentSize,
            .completelyLaidOut = false,
            .breakpoint = Some(Breakpoint::forced(endAt))
        };
    }

    return Ok(NONE);
}

Output fragmentEmptyBox(Fragmentainer const& fc, FragmentBuilder& fragBuilder, Input input) {
    // put this here instead of in layout.py since we want to know if its the empty box case
    Vec2Au knownSize{
        input.knownSize.width.unwrapOr(0_au),
        input.knownSize.height.unwrapOr(0_au),
    };
    if (fc.isDiscoveryMode()) {
        // FIXME: The rest of the box should continue on the next fragmentainer instead of being dropped.
        bool fitsInEmptyFragmentainer = knownSize.y + input.pendingVerticalSizes <= fc.size().y;
        if (not fitsInEmptyFragmentainer or
            fc.acceptsFit(
                input.position.y,
                knownSize.y,
                input.pendingVerticalSizes,
                input.atFragmentainerStart
            )) {

            return Output{
                .fragment = fragBuilder.buildBoxFromInput(input, knownSize),
                .size = knownSize,
                .completelyLaidOut = true,
            };
        } else {
            return Output{
                .fragment = fragBuilder.buildBoxFromInput(input, {}),
                .size = {},
                .completelyLaidOut = false,
                .breakpoint = Some(Breakpoint::overflow())
            };
        }
    } else {
        // FIXME: we should be breaking empty boxes using pixels or percentages, this behaviour is not compliant
        Au verticalSpaceLeft = fc.leftVerticalSpace(
            input.position.y, input.pendingVerticalSizes
        );

        Vec2Au size = {
            knownSize.x,
            min(knownSize.y, verticalSpaceLeft),
        };

        return Output{
            .fragment = fragBuilder.buildBoxFromInput(input, size),
            .size = size,
            .completelyLaidOut = verticalSpaceLeft >= knownSize.y,
        };
    }
}

// https://www.w3.org/TR/CSS22/tables.html#model
// The table wrapper box inline size is the table grid box inline size, computed
// here from its intrinsic sizes since tables with 'width: auto' size to
// fit-content instead of stretching like other block-level boxes.
Opt<Au> _tableWrapperFitContentWidth(Tree& tree, Box& wrapper, Au availableWidth) {
    for (auto& gridBox : wrapper.children()) {
        if (gridBox.style->display != Display::Internal::TABLE_BOX)
            continue;

        // TODO: Specified widths live on the table grid box and may be percentages,
        //       which we cannot resolve during intrinsic sizing; bail out for now.
        if (not gridBox.style->sizing->width.is<Keywords::Auto>())
            return NONE;

        return Some(computeFitContentInlineSize(tree, wrapper, availableWidth));
    }

    return NONE;
}

void _populateChildSpecifiedSizes(Tree& tree, Box& child, Input& parentInput, Input& childInput, UsedSpacings const& usedSpacings, Opt<Au> blockInlineSize) {
    // https://drafts.csswg.org/css-tables-3/#fixup-algorithm
    // Percentages which would depend on the width and height on the table-wrapper box’s size are relative to the table-wrapper
    // box’s containing block instead, not the table-wrapper box itself.
    auto containingBlock = child.style->display == Display::TABLE_BOX ? parentInput.containingBlock : childInput.containingBlock;

    if (childInput.intrinsic == IntrinsicSize::AUTO or child.style->display != Display::INLINE) {
        if (child.style->sizing->width.is<Keywords::Auto>()) {
            // https://www.w3.org/TR/css-tables-3/#layout-principles
            // Unlike other block-level boxes, tables do not fill their containing block by default.
            // When their width computes to auto, they behave as if they had fit-content specified instead.
            // This is different from most block-level boxes, which behave as if they had stretch instead.
            if (child.style->display == Display::TABLE_BOX) {
                // Do nothing. 'fit-content' is kinda intrinsic size, when we don't populate knownSize.
            } else if (blockInlineSize) {
                // When the inline size is not known, we cannot enforce it to the child. (?)
                Au availableWidth = blockInlineSize.expect() - usedSpacings.margin.horizontal();
                if (child.style->display == Display::TABLE) {
                    childInput.knownSize.width = Some(_tableWrapperFitContentWidth(tree, child, availableWidth)
                                                          .unwrapOr(availableWidth));
                } else {
                    childInput.knownSize.width = Some(availableWidth);
                }
            }
        } else {
            childInput.knownSize.width = computeSpecifiedBorderBoxWidth(
                tree, child, child.style->sizing->width, containingBlock,
                usedSpacings.padding.horizontal() + usedSpacings.borders.horizontal(),
                childInput.capmin
            );
        }

        childInput.knownSize.height = computeSpecifiedBorderBoxHeight(
            tree, child, child.style->sizing->height, containingBlock,
            usedSpacings.padding.vertical() + usedSpacings.borders.vertical()
        );
    }
}

// https://www.w3.org/TR/CSS22/visudet.html#blockwidth
// If both 'margin-left' and 'margin-right' are 'auto', their used values are equal,
// horizontally centering the box within its containing block. If only one of them is
// 'auto', it absorbs the remaining free space. This only applies when both the
// containing block inline size and the box inline size are known; otherwise 'auto'
// margins resolve to zero.
void _resolveAutoHorizontalMargins(Box& child, Input& childInput, UsedSpacings& usedSpacings, Opt<Au> blockInlineSize) {
    bool startIsAuto = child.style->margin->start.is<Keywords::Auto>().has();
    bool endIsAuto = child.style->margin->end.is<Keywords::Auto>().has();

    if (not(startIsAuto or endIsAuto))
        return;

    if (not blockInlineSize)
        return;

    if (not childInput.knownSize.width)
        return;

    // NOTE: 'auto' margins were resolved to zero when computing usedSpacings.
    Au freeSpace = blockInlineSize.expect() - childInput.knownSize.width.expect() - usedSpacings.margin.horizontal();
    if (freeSpace <= 0_au)
        return;

    if (startIsAuto and endIsAuto) {
        usedSpacings.margin.start = freeSpace / 2;
        usedSpacings.margin.end = freeSpace - usedSpacings.margin.start;
    } else if (startIsAuto) {
        usedSpacings.margin.start = freeSpace;
    } else {
        usedSpacings.margin.end = freeSpace;
    }
}

// NOTE: Measures the whole box, without recording breakpoints nor running elements.
Output _layoutMonolithic(Tree& tree, Box& box, Input input) {
    input.generateFragment = false;
    input.runningPosition = nullptr;
    tree.fc.enterMonolithicBox();
    auto output = layoutBorderBox(tree, box, input);
    tree.fc.leaveMonolithicBox();
    return output;
}

PlacedFloat _placeFloat(Tree& tree, Box& child, Direction direction, Input const& input, Input& childInput, UsedSpacings const& usedSpacings, Floats const& floats, Au blockSize, Au inlineSize) {
    // NOTE: The float is sized as a whole, not only the part that fits in the current fragmentainer.
    tree.fc.enterMonolithicBox();
    Defer _ = [&] {
        tree.fc.leaveMonolithicBox();
    };

    Au borderBoxSpacings = usedSpacings.padding.horizontal() + usedSpacings.borders.horizontal();
    Au availableWidth = inlineSize - usedSpacings.margin.horizontal() - borderBoxSpacings;

    if (child.style->display == Display::TABLE) {
        // NOTE: Table wrappers got their fit-content width from _populateChildSpecifiedSizes, unless the table has a
        //       specified width, which lives on the table grid box: then the wrapper shrinks to the grid box.
        bool gridBoxWidthIsAuto = iter(child.children()) | Any([](Box const& gridBox) {
                                      return gridBox.style->display == Display::Internal::TABLE_BOX and
                                             gridBox.style->sizing->width.is<Keywords::Auto>();
                                  });
        if (not gridBoxWidthIsAuto)
            childInput.knownSize.width = NONE;
    } else if (child.style->sizing->width.is<Keywords::Auto>()) {
        // https://www.w3.org/TR/CSS22/visudet.html#float-width
        // If 'width' is computed as 'auto', the used value is the "shrink-to-fit" width.
        childInput.knownSize.width = Some(computeFitContentInlineSize(tree, child, availableWidth) + borderBoxSpacings);
    }

    auto measured = _layoutMonolithic(tree, child, childInput);

    Au minTop = input.position.y + blockSize;

    // https://www.w3.org/TR/CSS22/visuren.html#flow-control
    // The top outer edge of the float must be below the bottom outer edge of all earlier left-floating boxes (in the
    // case of 'clear: left'), or all earlier right-floating boxes (in the case of 'clear: right'), or both ('clear:
    // both').
    if (auto clearance = floats.clearance(child.style->clear, direction))
        minTop = max(minTop, clearance.expect());

    auto side = resolveFloatSide(child.style->float_, direction);
    auto marginBox = floats.place(
        side,
        measured.size + usedSpacings.margin.all(),
        minTop,
        {input.position.x, inlineSize}
    );

    childInput.knownSize.width = Some(measured.size.x);
    childInput.position = marginBox.xy + usedSpacings.margin.topStart();
    return {side, marginBox};
}

// https://www.w3.org/TR/CSS22/visuren.html#floats
// The border box of a table, a block-level replaced element, or an element in the normal flow that establishes a new
// block formatting context (such as an element with 'overflow' other than 'visible') must not overlap the margin box of
// any floats in the same block formatting context as the element itself. If necessary, implementations should clear
// the said element by placing it below any preceding floats, but may place it adjacent to such floats if there is
// sufficient space. They may even make the border box of said element narrower than defined by section 10.3.3. CSS2
// does not define when a UA may put said element next to the float or by how much said element may become narrower.
// However, the current and subsequent line boxes created next to the float are shortened as necessary to make room for
// the margin box of the float.
// NOSPEC: Line boxes can't be shortened around floats, so blocks with inline content avoid floats the same way. A box is
//         put next to the floats only if it ends before the floats it is next to do, otherwise it goes below them.
Opt<Range<Au>> _avoidFloats(Tree& tree, Box& child, Input const& input, Input& childInput, UsedSpacings& usedSpacings, Floats const& floats, Au& blockSize, Au inlineSize) {
    Au top = input.position.y + blockSize;
    if (not floats.nextBottom(top, 0_au))
        return NONE;

    // NOTE: The box is measured as a whole, not only the part that fits in the current fragmentainer.
    tree.fc.enterMonolithicBox();
    Defer _ = [&] {
        tree.fc.leaveMonolithicBox();
    };

    Range<Au> container = {input.position.x, inlineSize};

    // NOSPEC: 'auto' margins are resolved again against the space next to the floats.
    UsedSpacings spacings = usedSpacings;
    spacings.margin = computeMargins(tree, child, childInput.containingBlock);
    Au margins = spacings.margin.horizontal();
    Au borderBoxSpacings = spacings.padding.horizontal() + spacings.borders.horizontal();
    Au minContentWidth = computeIntrinsicContentSize(tree, child, IntrinsicSize::MIN_CONTENT).x + borderBoxSpacings + margins;
    bool stretches = child.style->sizing->width.is<Keywords::Auto>() and child.style->display != Display::TABLE;

    while (auto floatsBottom = floats.nextBottom(top, 0_au)) {
        auto band = floats.freeBand(top, 0_au, container);
        Au width = stretches ? band.size - margins : childInput.knownSize.width.unwrapOr(band.size - margins);

        if (band.size >= max(minContentWidth, width + margins)) {
            auto measured = _layoutMonolithic(tree, child, childInput.withKnownSize({Some(width), childInput.knownSize.height}));
            if (top + measured.size.y <= floatsBottom.expect()) {
                childInput.knownSize.width = Some(width);
                _resolveAutoHorizontalMargins(child, childInput, spacings, Some(band.size));
                childInput.position = {band.start + spacings.margin.start, top};
                usedSpacings = spacings;
                blockSize = top - input.position.y;
                return Some(band);
            }
        }

        top = floatsBottom.expect();
        childInput.position.y = top;
    }

    blockSize = top - input.position.y;
    return NONE;
}

// https://www.w3.org/TR/CSS22/visuren.html#normal-flow
struct BlockFormatingContext : FormatingContext {
    Au _computeCapmin(Tree& tree, Box& box, Input input, Au inlineSize) {
        Au capmin{};
        for (auto& c : box.children()) {
            if (c.style->display != Display::TABLE_BOX) {
                auto minContentContrib = computeIntrinsicContentSize(
                    tree, c, IntrinsicSize::MIN_CONTENT
                );

                Vec2Au containingBlock = {inlineSize, input.knownSize.height.unwrapOr(0_au)};
                UsedSpacings usedSpacings{
                    .padding = computePaddings(tree, c, containingBlock),
                    .borders = computeBorders(tree, c),
                    .margin = computeMargins(tree, c, containingBlock)
                };

                capmin = max(
                    capmin,
                    minContentContrib.width + usedSpacings.margin.horizontal() +
                        usedSpacings.padding.horizontal() + usedSpacings.borders.horizontal()
                );
            }
        }

        return capmin;
    }

    Output run(Tree& tree, Box& box, Input input, usize startAt, Opt<usize> stopAt) override {
        auto fragBuilder = FragmentBuilder{tree, box};

        Au blockSize = 0_au;
        Au inlineSize = input.knownSize.width.unwrapOr(0_au);

        if (box.children().len() == 0) {
            return fragmentEmptyBox(tree.fc, fragBuilder, input);
        }

        Breakpoint currentBreakpoint;
        Opt<BaselinePositionsSet> firstBaselineSet = NONE;
        BaselinePositionsSet lastBaselineSet;

        // NOTE: Floats can only be placed once the inline size is known.
        bool placesFloats = input.knownSize.width.has() and not isMinMaxIntrinsicSize(input.intrinsic);
        Floats floats = input.floats;
        Opt<Au> placedFloatsBottom = NONE;

        // https://www.w3.org/TR/CSS22/visudet.html#normal-block
        // Only children in the normal flow are taken into account (i.e., floating boxes and absolutely positioned boxes
        // are ignored, and relatively positioned boxes are considered without their offset).
        // NOSPEC: Every box grows to include the floats it places, not only block formatting context roots, so a float
        //         never overlaps the content that follows its parent.
        auto contentBlockSize = [&] {
            if (not placedFloatsBottom)
                return blockSize;
            return max(blockSize, placedFloatsBottom.expect() - input.position.y);
        };

        usize endChildren = stopAt.unwrapOr(box.children().len());

        bool blockWasCompletelyLaidOut = false;

        Au lastMarginBottom = 0_au;

        // NOSPEC: In max-content sizing, the floats are placed beside the content that avoids them, so their widths add
        //         up with it instead of competing with it.
        Au floatsBesideInlineSize = 0_au;

        Vec<Rc<PlaceholderFragment>> outOfFlowChildren;

        bool atFragmentainerStart = input.atFragmentainerStart;

        for (usize i = startAt; i < endChildren; ++i) {
            auto& c = box.children()[i];
            lookForRunningPosition(input, c);
            if (c.isRunningPositionedBox())
                continue;

            if (oneOf(c.style->position, Keywords::ABSOLUTE, Keywords::FIXED)) {
                if (input.generateFragment) {
                    // https://www.w3.org/TR/css-position-3/#staticpos-rect
                    RectAu staticPosRect = {
                        Vec2Au{input.position.x, input.position.y + blockSize},
                        Vec2Au{input.knownSize.width.unwrapOr(0_au), 0_au}
                    };

                    auto placeholder = makeRc<PlaceholderFragment>(c, staticPosRect);

                    fragBuilder.addChildIfAny(Some(placeholder));
                    outOfFlowChildren.pushBack(placeholder);
                }

                if (i + 1 == box.children().len()) {
                    blockWasCompletelyLaidOut = true;
                }

                continue;
            }

            try$(
                processBreakpointsBeforeChild(
                    fragBuilder,
                    i,
                    Vec2Au{inlineSize, contentBlockSize()},
                    c.style->break_->before == BreakBetween::PAGE,
                    startAt,
                    input
                )
            );

            auto childContainingBlock = Vec2Au{inlineSize, input.knownSize.height.unwrapOr(0_au)};

            auto usedSpacings = UsedSpacings{
                .padding = computePaddings(tree, c, childContainingBlock),
                .borders = computeBorders(tree, c),
                .margin = computeMargins(tree, c, childContainingBlock)
            };

            Input childInput = {
                .generateFragment = input.generateFragment,
                .usedSpacings = usedSpacings,
                .intrinsic = input.intrinsic,
                .availableSpace = {input.availableSpace.x, 0_au},
                .containingBlock = childContainingBlock,
                .runningPosition = input.runningPosition,
                .pageNumber = input.pageNumber,
                .breakpointTraverser = input.breakpointTraverser.traverseInsideUsingIthChild(i),
                .pendingVerticalSizes = input.pendingVerticalSizes,
                .atFragmentainerStart = atFragmentainerStart,
            };

            if (not c.isRemovedFromFlow()) {
                // TODO: collapsed margins for sibling elements

                Au maxPositive = max(0_au, usedSpacings.margin.top, lastMarginBottom);
                Au minNegative = min(0_au, usedSpacings.margin.top, lastMarginBottom);

                Au collapsedMargin = maxPositive - Math::abs(minNegative);
                blockSize += collapsedMargin - lastMarginBottom;

                auto clearance = placesFloats ? floats.clearance(c.style->clear, box.style->direction) : NONE;
                if (clearance)
                    blockSize = max(blockSize, clearance.expect() - input.position.y);
            }

            // HACK: Table Box mostly behaves like a block box, let's compute its capmin
            //       and avoid duplicating the layout code
            if (c.style->display == Display::Internal::TABLE_BOX)
                childInput.capmin = Some(_computeCapmin(tree, box, input, inlineSize));

            _populateChildSpecifiedSizes(tree, c, input, childInput, usedSpacings, input.knownSize.width);

            if (not c.isRemovedFromFlow())
                _resolveAutoHorizontalMargins(c, childInput, usedSpacings, input.knownSize.width);

            childInput.position = input.position + Vec2Au{usedSpacings.margin.start, blockSize};

            Opt<PlacedFloat> placedFloat = NONE;
            if (placesFloats and c.isFloating())
                placedFloat = Some(_placeFloat(tree, c, box.style->direction, input, childInput, usedSpacings, floats, blockSize, inlineSize));

            if (placesFloats and not c.establishesBlockFormatingContext())
                childInput.floats = floats;

            Opt<Range<Au>> bandNextToFloats = NONE;
            bool avoidsFloats = c.content.is<Rc<Gfx::Prose>>() or c.establishesBlockFormatingContext();
            if (placesFloats and floats.len() and avoidsFloats and not c.isRemovedFromFlow() and not c.isPseudoElement(Dom::PseudoElement::MARKER))
                bandNextToFloats = _avoidFloats(tree, c, input, childInput, usedSpacings, floats, blockSize, inlineSize);

            if (box.style->text->align == TextAlign::BLOCK_CENTER and not placedFloat) {
                Au availableWidth = bandNextToFloats ? bandNextToFloats.expect().size : inlineSize;
                childInput.position.x += availableWidth / 2 - layoutBorderBox(tree, c, childInput.withGenerateFragment(false)).width() / 2;
            }

            if (c.isPseudoElement(Dom::PseudoElement::MARKER)) {
                // NOSPEC: The spec doesn't define where the marker should be placed.
                auto width = computeIntrinsicContentSize(tree, c, IntrinsicSize::MAX_CONTENT).x;
                childInput.position.x -= width * 2.5;
                childInput.knownSize.width = Some(width);
            }

            if (c.style->position == Keywords::RELATIVE) {
                childInput.position += relativePositionOffset(tree, c, input.containingBlock);
            }

            auto output = layoutBorderBox(tree, c, childInput);
            fragBuilder.addChildIfAny(output.fragment);

            if (placedFloat) {
                placedFloat->marginBox.height = output.size.y + usedSpacings.margin.vertical();
                floats.add(*placedFloat);
                Au bottom = placedFloat->marginBox.bottom();
                placedFloatsBottom = Some(max(placedFloatsBottom.unwrapOr(bottom), bottom));
            }

            outOfFlowChildren.pushBack(output.outOfFlowStash);

            if (output.size.y > 0_au and not c.isPseudoElement(Dom::PseudoElement::MARKER))
                atFragmentainerStart = false;

            if (not(c.isRemovedFromFlow() or c.isPseudoElement(Dom::PseudoElement::MARKER))) {
                blockSize += output.size.y + usedSpacings.margin.bottom;
                lastMarginBottom = usedSpacings.margin.bottom;
            }

            maybeProcessChildBreakpoint(
                tree.fc,
                currentBreakpoint,
                i,
                oneOf(box.style->break_->inside, BreakInside::AVOID, BreakInside::AVOID_PAGE),
                output.breakpoint
            );

            // https://www.w3.org/TR/css-align-3/#baseline-export
            // The first/last baseline set of a block container is taken from the first/last in-flow line box in the
            // block container or the first/last in-flow block-level child in the block container that contributes a
            // set of first/last baselines, whichever comes first/last.
            if (not c.isFloating()) {
                if (not firstBaselineSet)
                    firstBaselineSet = Some(output.firstBaselineSet.translate(childInput.position.y - input.position.y));
                lastBaselineSet = output.lastBaselineSet.translate(childInput.position.y - input.position.y);
            }

            try$(processBreakpointsAfterChild(
                tree.fc,
                fragBuilder,
                currentBreakpoint,
                box,
                i,
                Vec2Au{inlineSize, contentBlockSize()},
                output.completelyLaidOut,
                input
            ));

            if (tree.fc.allowBreak() and i + 1 == endChildren) {
                blockWasCompletelyLaidOut = output.completelyLaidOut and i + 1 == box.children().len();
            }

            Au childInlineSize = output.size.x + usedSpacings.margin.horizontal();
            if (input.intrinsic == IntrinsicSize::MAX_CONTENT) {
                if (c.style->clear != Clear::NONE)
                    floatsBesideInlineSize = 0_au;

                if (c.isFloating()) {
                    floatsBesideInlineSize += childInlineSize;
                    childInlineSize = floatsBesideInlineSize;
                } else {
                    if (avoidsFloats)
                        childInlineSize += floatsBesideInlineSize;
                    floatsBesideInlineSize = 0_au;
                }
            }

            inlineSize = max(inlineSize, childInlineSize);
        }

        auto size = Vec2Au{
            input.knownSize.width.unwrapOr(inlineSize),
            input.knownSize.height.unwrapOr(contentBlockSize())
        };

        return {
            .fragment = fragBuilder.buildBoxFromInput(input, size),
            .size = size,
            .completelyLaidOut = blockWasCompletelyLaidOut,
            .breakpoint = Some(currentBreakpoint),
            .firstBaselineSet = firstBaselineSet.unwrapOr({}),
            .lastBaselineSet = lastBaselineSet,
            .outOfFlowStash = std::move(outOfFlowChildren),
        };
    }
};

export Rc<FormatingContext> constructBlockFormatingContext(Box&) {
    return makeRc<BlockFormatingContext>();
}

} // namespace Vaev::Layout
