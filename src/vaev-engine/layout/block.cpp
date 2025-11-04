module;

#include <karm-core/macros.h>

export module Vaev.Engine:layout.block;

import Karm.Math;
import Karm.Core;

import :values;
import :layout.base;
import :layout.layout;
import :layout.positioned;

using namespace Karm;

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

// https://www.w3.org/TR/CSS22/visuren.html#normal-flow
struct BlockFormatingContext : FormatingContext {
    Au _computeCapmin(Tree& tree, Box& box, Input input, Au inlineSize) {
        Au capmin{};
        for (auto& c : box.children()) {
            if (c.style->display != Display::TABLE_BOX) {
                auto margin = computeMargins(
                    tree, c,
                    {
                        .containingBlock = {inlineSize, input.knownSize.y.unwrapOr(0_au)},
                    }
                );

                auto minContentContrib = computeIntrinsicSize(
                    tree, c, IntrinsicSize::MIN_CONTENT, input.containingBlock
                );

                capmin = max(
                    capmin,
                    minContentContrib.width + margin.horizontal()
                );
            }
        }

        return capmin;
    }

    Output run2(Tree& tree, Box& box, Input input, usize startAt, Opt<usize> stopAt) override {
        Au blockSize = 0_au;
        Au inlineSize = input.knownSize.width.unwrapOr(0_au);

        if (box.children().len() == 0) {
            return fragmentEmptyBox(tree, input);
        }

        // NOTE: Our parent has no clue about our width but wants us to commit,
        //       we need to compute it first
        if (input.fragment and not input.knownSize.width)
            inlineSize = run(tree, box, input.withFragment(nullptr), startAt, stopAt).width();

        Breakpoint currentBreakpoint;
        BaselinePositionsSet firstBaselineSet, lastBaselineSet;

        usize endChildren = stopAt.unwrapOr(box.children().len());

        bool blockWasCompletelyLaidOut = false;

        Au lastMarginBottom = 0_au;

        for (usize i = startAt; i < endChildren; ++i) {
            auto& c = box.children()[i];
            lookForRunningPosition(input, c);

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
                .fragment = input.fragment,
                .intrinsic = input.intrinsic,
                .availableSpace = {input.availableSpace.x, 0_au},
                .containingBlock = {inlineSize, input.knownSize.y.unwrapOr(0_au)},
                .runningPosition = input.runningPosition,
                .pageNumber = input.pageNumber,
                .breakpointTraverser = input.breakpointTraverser.traverseInsideUsingIthChild(i),
                .pendingVerticalSizes = input.pendingVerticalSizes,
            };

            auto margin = computeMargins(tree, c, childInput);

            Opt<Au> childInlineSize = NONE;
            if (c.style->sizing->width.is<Keywords::Auto>()) {
                childInlineSize = inlineSize - margin.horizontal();
            }

            if (not c.isPositioned() or c.style->position == Keywords::RELATIVE) {
                // TODO: collapsed margins for sibling elements
                blockSize += max(margin.top, lastMarginBottom) - lastMarginBottom;
                if (input.fragment or input.knownSize.x)
                    childInput.knownSize.width = childInlineSize;
            }

            childInput.position = input.position + Vec2Au{margin.start, blockSize};

            // HACK: Table Box mostly behaves like a block box, let's compute its capmin
            //       and avoid duplicating the layout code
            if (c.style->display == Display::Internal::TABLE_BOX) {
                childInput.capmin = _computeCapmin(tree, box, input, inlineSize);
            }

            auto output = layout(
                tree,
                c,
                childInput
            );
            if (not impliesRemovingFromFlow(c.style->position)) {
                blockSize += output.size.y + margin.bottom;
                lastMarginBottom = margin.bottom;
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

            inlineSize = max(inlineSize, output.size.x + margin.horizontal());
        }

        return {
            .size = Vec2Au{inlineSize, blockSize},
            .completelyLaidOut = blockWasCompletelyLaidOut,
            .breakpoint = currentBreakpoint,
            .firstBaselineSet = firstBaselineSet,
            .lastBaselineSet = lastBaselineSet,
        };
    }

    ////////////////////////////////////////////////////////////////////////////
    // NEW CODE
    ////////////////////////////////////////////////////////////////////////////

    struct Item {
        Box const* box;

        Math::Vec2<Opt<Au>> size;
        Math::Insets<Opt<Au>> margins;
        Math::Insets<Au> borders;
        Math::Insets<Au> paddings;
    };

    Vec<Item> _items;

    void _generateItems(Tree& tree, Box& box, Input input, usize startAt, Opt<usize> stopAt) {
        _items.clear();

        auto children = box.children();
        children = mutSub(children, startAt, startAt + stopAt.unwrapOr(box.children().len()));
        for (auto& c : children) {
            Item item;
            item.box = &c;

            item.size.x = computeSpecifiedSize(tree, c, c.style->sizing->width, input.containingBlock, true);
            item.size.y = computeSpecifiedSize(tree, c, c.style->sizing->width, input.containingBlock, false);
            if (input.intrinsic != IntrinsicSize::AUTO) {
                auto intrinsic computeIntrinsicSize(tree, c, input.intrinsic, input.containingBlock)
            }
            item.margins = computeMargins(tree, c, input);
            item.borders = computeBorders(tree, c);
            item.paddings = computePaddings(tree, c, input.containingBlock);
        }
    }

    void _collapseMargins() {
        for (auto& i : _items) {
            // https://www.w3.org/TR/CSS22/visudet.html#normal-block

            // If 'margin-top', or 'margin-bottom' are 'auto', their used value is 0.
            if (i.margins.top == NONE)
                i.margins.top = 0_au;
            if (i.margins.bottom == NONE)
                i.margins.bottom = 0_au;

            // If 'height' is 'auto', the height depends on whether the element has any block-level children and whether it has padding or borders:
        }
    }

    // https://www.w3.org/TR/CSS22/visudet.html#blockwidth
    void _computeWidth(Input const& input) {
        for (auto& i : _items) {
            // 'margin-left' + 'border-left-width' + 'padding-left' + 'width' + 'padding-right' + 'border-right-width' + 'margin-right' = width of containing block
            if (input.intrinsic != IntrinsicSize::AUTO) {
                i.margins.start = i.margins.start.unwrapOr(0_au);
                i.margins.end = i.margins.end.unwrapOr(0_au);
                continue;
            }

            // If 'width' is not 'auto' and 'border-left-width' + 'padding-left' + 'width' + 'padding-right' + 'border-right-width' (plus any of 'margin-left' or 'margin-right' that are not 'auto') is larger than the width of the containing block, then any 'auto' values for 'margin-left' or 'margin-right' are, for the following rules, treated as zero.

            // If all of the above have a computed value other than 'auto', the values are said to be "over-constrained" and one of the used values will have to be different from its computed value. If the 'direction' property of the containing block has the value 'ltr', the specified value of 'margin-right' is ignored and the value is calculated so as to make the equality true. If the value of 'direction' is 'rtl', this happens to 'margin-left' instead.

            // If there is exactly one value specified as 'auto', its used value follows from the equality.

            // If 'width' is set to 'auto', any other 'auto' values become '0' and 'width' follows from the resulting equality.

            // If both 'margin-left' and 'margin-right' are 'auto', their used values are equal. This horizontally centers the element with respect to the edges of the containing block.
        }
    }

    // https://www.w3.org/TR/CSS22/visudet.html#abs-non-replaced-height
    void _computeHeight(Input const& input) {
        for (auto& i : _items) {
            // 'top' + 'margin-top' + 'border-top-width' + 'padding-top' + 'height' + 'padding-bottom' + 'border-bottom-width' + 'margin-bottom' + 'bottom' = height of containing block

            if (input.intrinsic != IntrinsicSize::AUTO) {
                i.margins.top = i.margins.top.unwrapOr(0_au);
                i.margins.bottom = i.margins.top.unwrapOr(0_au);
                continue;
            }

            // 1. The used value of 'height' is determined as for inline replaced elements. If 'margin-top' or 'margin-bottom' is specified as 'auto' its used value is determined by the rules below.

            // 2. If both 'top' and 'bottom' have the value 'auto', replace 'top' with the element's static position.

            // 3. If 'bottom' is 'auto', replace any 'auto' on 'margin-top' or 'margin-bottom' with '0'.

            // 4. If at this point both 'margin-top' and 'margin-bottom' are still 'auto', solve the equation under the extra constraint that the two margins must get equal values.

            // 5. If at this point there is only one 'auto' left, solve the equation for that value.

            // 6. If at this point the values are over-constrained, ignore the value for 'bottom' and solve for that value.
        }
    }

    void _layout(Input const& input) {
    }

    Output run(Tree& tree, Box& box, Input input, usize startAt, Opt<usize> stopAt) {
        _generateItems(tree, box, input, startAt, stopAt);
        _collapseMargins();
        _computeWidth();
        _computeHeight();
        _layout();
    }
};

export Rc<FormatingContext> constructBlockFormatingContext(Box&) {
    return makeRc<BlockFormatingContext>();
}

} // namespace Vaev::Layout
