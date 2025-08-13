module;

#include <karm-core/macros.h>
#include <karm-math/au.h>

export module Vaev.Engine:layout.block;

import :values;
import :layout.base;
import :layout.layout;

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

Res<None, Output> processBreakpointsAfterChild(Fragmentainer& fc, Breakpoint& currentBreakpoint, Box& parentBox, usize childIndex, Vec2Au currentBoxSize, bool childCompletelyLaidOut) {
    if (not fc.isDiscoveryMode())
        return Ok(NONE);

    // last child was not completly laid out, we need to abort with our best breakpoint
    if (not childCompletelyLaidOut) {
        return Output{
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
            parentBox.style->break_->inside == BreakInside::AVOID or
            childBox.style->break_->after == BreakBetween::AVOID or
            (not isLastChild and parentBox.children()[childIndex + 1].style->break_->before == BreakBetween::AVOID);

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
            .size = currentBoxSize,
            .completelyLaidOut = false,
            .breakpoint = Breakpoint::forced(
                childIndex + 1
            )
        };
    }

    return Ok(NONE);
}

Res<None, Output> processBreakpointsBeforeChild(usize endAt, Vec2Au currentSize, bool forcedBreakBefore, usize startAt) {
    // FORCED BREAK
    if (forcedBreakBefore and not(startAt == endAt)) {
        return Output{
            .size = currentSize,
            .completelyLaidOut = false,
            .breakpoint = Breakpoint::forced(endAt)
        };
    }

    return Ok(NONE);
}

Output fragmentEmptyBox(Tree& tree, Input input) {
    // put this here instead of in layout.py since we want to know if its the empty box case
    Vec2Au knownSize{input.knownSize.x.unwrapOr(0_au), input.knownSize.y.unwrapOr(0_au)};
    if (tree.fc.isDiscoveryMode()) {
        if (tree.fc.acceptsFit(
                input.position.y,
                knownSize.y,
                input.pendingVerticalSizes
            )) {
            return Output{
                .size = knownSize,
                .completelyLaidOut = true,
            };
        } else {
            return Output{
                .size = {},
                .completelyLaidOut = false,
                .breakpoint = Breakpoint::overflow()
            };
        }
    } else {
        // FIXME: we should be breaking empty boxes using pixels or percentages, this behaviour is not compliant
        Au verticalSpaceLeft = tree.fc.leftVerticalSpace(
            input.position.y, input.pendingVerticalSizes
        );
        return Output{
            .size = {knownSize.x, min(knownSize.y, verticalSpaceLeft)},
            .completelyLaidOut = verticalSpaceLeft >= knownSize.y,
        };
    }
}

void _populateChildSpecifiedSizes(Tree& tree, Box& child, Input& childInput, Au horizontalMargins, Opt<Au> blockInlineSize) {
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
                childInput.knownSize.width = blockInlineSize.unwrap() - horizontalMargins;
            }
        } else {
            childInput.knownSize.width = computeSpecifiedWidth(
                tree, child, child.style->sizing->width, childInput.containingBlock
            );
        }

        childInput.knownSize.height = computeSpecifiedHeight(
            tree, child, child.style->sizing->height, childInput.containingBlock
        );
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
                    .margin = computeMargins(tree, c, {.containingBlock = containingBlock})
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
        Au blockSize = 0_au;
        Au inlineSize = input.knownSize.width.unwrapOr(0_au);

        if (box.children().len() == 0) {
            return fragmentEmptyBox(tree, input);
        }

        Breakpoint currentBreakpoint;
        BaselinePositionsSet firstBaselineSet, lastBaselineSet;

        usize endChildren = stopAt.unwrapOr(box.children().len());

        bool blockWasCompletelyLaidOut = false;

        Au lastMarginBottom = 0_au;
        for (usize i = startAt; i < endChildren; ++i) {
            auto& c = box.children()[i];

            try$(
                processBreakpointsBeforeChild(
                    i,
                    Vec2Au{inlineSize, blockSize},
                    c.style->break_->before == BreakBetween::PAGE,
                    startAt
                )
            );

            // TODO: Implement floating
            // if (c.style->float_ != Float::NONE)
            //     continue;

            Input childInput = {
                .intrinsic = input.intrinsic,
                .availableSpace = {input.availableSpace.x, 0_au},
                .containingBlock = {inlineSize, input.knownSize.y.unwrapOr(0_au)},
                .breakpointTraverser = input.breakpointTraverser.traverseInsideUsingIthChild(i),
                .pendingVerticalSizes = input.pendingVerticalSizes,
            };

            UsedSpacings usedSpacings{
                .padding = computePaddings(tree, c, childInput.containingBlock),
                .borders = computeBorders(tree, c),
                .margin = computeMargins(tree, c, childInput)
            };

            if (not impliesRemovingFromFlow(c.style->position)) {
                // TODO: collapsed margins for sibling elements
                blockSize += max(usedSpacings.margin.top, lastMarginBottom) - lastMarginBottom;
            }

            childInput.position = input.position + Vec2Au{usedSpacings.margin.start, blockSize};

            // HACK: Table Box mostly behaves like a block box, let's compute its capmin
            //       and avoid duplicating the layout code
            if (c.style->display == Display::Internal::TABLE_BOX) {
                childInput.capmin = _computeCapmin(tree, box, input, inlineSize);
            }

            _populateChildSpecifiedSizes(tree, c, childInput, usedSpacings.margin.horizontal(), input.knownSize.x);

            auto output = input.fragment
                              ? layoutAndCommitBorderBox(tree, c, childInput, *input.fragment, usedSpacings)
                              : layoutBorderBox(tree, c, childInput, usedSpacings);

            if (not impliesRemovingFromFlow(c.style->position)) {
                blockSize += output.size.y + usedSpacings.margin.bottom;
                lastMarginBottom = usedSpacings.margin.bottom;
            }

            maybeProcessChildBreakpoint(
                tree.fc,
                currentBreakpoint,
                i,
                box.style->break_->inside == BreakInside::AVOID,
                output.breakpoint
            );

            if (i == startAt)
                firstBaselineSet = output.firstBaselineSet.translate(childInput.position.y - input.position.y);
            lastBaselineSet = output.lastBaselineSet.translate(childInput.position.y - input.position.y);

            try$(processBreakpointsAfterChild(
                tree.fc,
                currentBreakpoint,
                box,
                i,
                Vec2Au{inlineSize, blockSize},
                output.completelyLaidOut
            ));

            if (tree.fc.allowBreak() and i + 1 == endChildren) {
                blockWasCompletelyLaidOut = output.completelyLaidOut and i + 1 == box.children().len();
            }

            inlineSize = max(inlineSize, output.size.x + usedSpacings.margin.horizontal());
        }

        return {
            .size = Vec2Au{
                input.knownSize.x.unwrapOr(inlineSize),
                input.knownSize.y.unwrapOr(blockSize)
            },
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
