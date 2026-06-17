module;

#include <karm/macros>

export module Vaev.Engine:layout.block;

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
            std::move(maybeChildBreakpoint.unwrap()),
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
            .breakpoint = currentBreakpoint
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
            .breakpoint = Breakpoint::forced(
                childIndex + 1
            )
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
            .breakpoint = Breakpoint::forced(endAt)
        };
    }

    return Ok(NONE);
}

Output fragmentEmptyBox(Fragmentainer const& fc, FragmentBuilder& fragBuilder, Input input) {
    // put this here instead of in layout.py since we want to know if its the empty box case
    Vec2Au knownSize{input.knownSize.x.unwrapOr(0_au), input.knownSize.y.unwrapOr(0_au)};
    if (fc.isDiscoveryMode()) {
        if (fc.acceptsFit(
                input.position.y,
                knownSize.y,
                input.pendingVerticalSizes
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
                .breakpoint = Breakpoint::overflow()
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

        return computeFitContentInlineSize(tree, wrapper, availableWidth);
    }

    return NONE;
}

void _populateChildSpecifiedSizes(Tree& tree, Box& child, Input& childInput, UsedSpacings const& usedSpacings, Opt<Au> blockInlineSize) {
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
                Au availableWidth = blockInlineSize.unwrap() - usedSpacings.margin.horizontal();
                if (child.style->display == Display::TABLE) {
                    childInput.knownSize.width = _tableWrapperFitContentWidth(tree, child, availableWidth)
                                                     .unwrapOr(availableWidth);
                } else {
                    childInput.knownSize.width = availableWidth;
                }
            }
        } else {
            childInput.knownSize.width = computeSpecifiedBorderBoxWidth(
                tree, child, child.style->sizing->width, childInput.containingBlock,
                usedSpacings.padding.horizontal() + usedSpacings.borders.horizontal(),
                childInput.capmin
            );
        }

        childInput.knownSize.height = computeSpecifiedBorderBoxHeight(
            tree, child, child.style->sizing->height, childInput.containingBlock,
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
    bool startIsAuto = child.style->margin->start.is<Keywords::Auto>();
    bool endIsAuto = child.style->margin->end.is<Keywords::Auto>();

    if (not(startIsAuto or endIsAuto))
        return;

    if (not blockInlineSize)
        return;

    if (not childInput.knownSize.width)
        return;

    // NOTE: 'auto' margins were resolved to zero when computing usedSpacings.
    Au freeSpace = blockInlineSize.unwrap() - childInput.knownSize.width.unwrap() - usedSpacings.margin.horizontal();
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

// https://www.w3.org/TR/CSS22/visuren.html#normal-flow
struct BlockFormatingContext : FormatingContext {
    Au _computeCapmin(Tree& tree, Box& box, Input input, Au inlineSize) {
        Au capmin{};
        for (auto& c : box.children()) {
            if (c.style->display != Display::TABLE_BOX) {
                auto minContentContrib = computeIntrinsicContentSize(
                    tree, c, IntrinsicSize::MIN_CONTENT
                );

                Vec2Au containingBlock = {inlineSize, input.knownSize.y.unwrapOr(0_au)};
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
        BaselinePositionsSet firstBaselineSet, lastBaselineSet;

        usize endChildren = stopAt.unwrapOr(box.children().len());

        bool blockWasCompletelyLaidOut = false;

        Au lastMarginBottom = 0_au;

        for (usize i = startAt; i < endChildren; ++i) {
            auto& c = box.children()[i];
            lookForRunningPosition(input, c);
            if (c.isRunningPositionedBox())
                continue;

            try$(
                processBreakpointsBeforeChild(
                    fragBuilder,
                    i,
                    Vec2Au{inlineSize, blockSize},
                    c.style->break_->before == BreakBetween::PAGE,
                    startAt,
                    input
                )
            );

            // TODO: Implement floating
            // if (c.style->float_ != Float::NONE)
            //     continue;

            auto childContainingBlock = Vec2Au{inlineSize, input.knownSize.y.unwrapOr(0_au)};

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
            };

            if (not c.isRemovedFromFlow()) {
                // TODO: collapsed margins for sibling elements

                Au maxPositive = max(0_au, usedSpacings.margin.top, lastMarginBottom);
                Au minNegative = min(0_au, usedSpacings.margin.top, lastMarginBottom);

                Au collapsedMargin = maxPositive - Math::abs(minNegative);
                blockSize += collapsedMargin - lastMarginBottom;
            }

            // HACK: Table Box mostly behaves like a block box, let's compute its capmin
            //       and avoid duplicating the layout code
            if (c.style->display == Display::Internal::TABLE_BOX) {
                childInput.capmin = _computeCapmin(tree, box, input, inlineSize);
            }

            _populateChildSpecifiedSizes(tree, c, childInput, usedSpacings, input.knownSize.x);

            if (not c.isRemovedFromFlow())
                _resolveAutoHorizontalMargins(c, childInput, usedSpacings, input.knownSize.x);

            childInput.position = input.position + Vec2Au{usedSpacings.margin.start, blockSize};

            auto output = layoutBorderBox(tree, c, childInput);
            if (auto [frag] = output.fragment)
                fragBuilder.addChild(frag);

            if (not c.isRemovedFromFlow()) {
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

            if (i == startAt)
                firstBaselineSet = output.firstBaselineSet.translate(childInput.position.y - input.position.y);
            lastBaselineSet = output.lastBaselineSet.translate(childInput.position.y - input.position.y);

            try$(processBreakpointsAfterChild(
                tree.fc,
                fragBuilder,
                currentBreakpoint,
                box,
                i,
                Vec2Au{inlineSize, blockSize},
                output.completelyLaidOut,
                input
            ));

            if (tree.fc.allowBreak() and i + 1 == endChildren) {
                blockWasCompletelyLaidOut = output.completelyLaidOut and i + 1 == box.children().len();
            }

            inlineSize = max(inlineSize, output.size.x + usedSpacings.margin.horizontal());
        }

        auto size = Vec2Au{
            input.knownSize.x.unwrapOr(inlineSize),
            input.knownSize.y.unwrapOr(blockSize)
        };

        return {
            .fragment = fragBuilder.buildBoxFromInput(input, size),
            .size = size,
            .completelyLaidOut = blockWasCompletelyLaidOut,
            .breakpoint = currentBreakpoint,
            .firstBaselineSet = firstBaselineSet,
            .lastBaselineSet = lastBaselineSet,
        };
    }
};

export Rc<FormatingContext> constructBlockFormatingContext(Box&) {
    return makeRc<BlockFormatingContext>();
}

} // namespace Vaev::Layout
