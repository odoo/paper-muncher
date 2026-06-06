module;

#include <karm/macros>
#include <utility>

export module Vaev.Engine:layout.block;

import Karm.Math;
import Karm.Core;

import :values;
import :layout.base;
import :layout.layout;
import :layout.positioned;

namespace Vaev::Layout {

static _Breakpoint _breakBefore(usize childIndex, Style::ComputedValues const& style) {
    if (style.break_->before == BreakBetween::PAGE) {
        return _Breakpoint::before(childIndex, _Breakpoint::PERFECT, true);
    }

    // TODO: Support other fragmentainers.
    if (oneOf(style.break_->before, BreakBetween::AVOID, BreakBetween::AVOID_PAGE)) {
        return _Breakpoint::before(childIndex, _Breakpoint::IGNORING_BREAK_AVOID, false);
    }

    // NOTE: Other unimplemented values are treated as `auto`.
    return _Breakpoint::before(childIndex, _Breakpoint::PERFECT, false);
}

static _Breakpoint _breakInside(usize childIndex, _Breakpoint&& childBreakpoint, Style::ComputedValues const& style) {
    _Breakpoint::Appeal appeal = _Breakpoint::PERFECT;

    // TODO: Support other fragmentainers.
    if (oneOf(style.break_->inside, BreakInside::AVOID, BreakInside::AVOID_PAGE)) {
        appeal = _Breakpoint::IGNORING_BREAK_AVOID;
    }

    appeal = min(appeal, childBreakpoint.appeal);
    return _Breakpoint::inside(childIndex, appeal, childBreakpoint.isForced, makeBox(std::move(childBreakpoint)));
}

static _Breakpoint _breakAfter(usize childIndex, Style::ComputedValues const& style) {
    if (style.break_->after == BreakBetween::PAGE) {
        return _Breakpoint::after(childIndex, _Breakpoint::PERFECT, true);
    }

    // TODO: Support other fragmentainers.
    if (oneOf(style.break_->after, BreakBetween::AVOID, BreakBetween::AVOID_PAGE)) {
        return _Breakpoint::after(childIndex, _Breakpoint::IGNORING_BREAK_AVOID, false);
    }

    // NOTE: Other unimplemented values are treated as `auto`.
    return _Breakpoint::before(childIndex, _Breakpoint::PERFECT, false);
}

// Fragments a monolithic box (no children, known dimensions).
static Output _fragmentMonolithicBox(Fragmentainer const& fc, Input input) {
    Vec2Au knownSize{input.knownSize.x.unwrapOr(0_au), input.knownSize.y.unwrapOr(0_au)};
    if (fc.acceptsFit(
            input.position.y,
            knownSize.y,
            input.pendingVerticalSizes
        )) {
        return Output{
            .size = knownSize,
            .completelyLaidOut = true,
        };
    }

    return Output{
        .size = {},
        .completelyLaidOut = false,
        .breakpoint = _Breakpoint::lastResort(),
    };
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
                childInput.knownSize.width = blockInlineSize.unwrap() - usedSpacings.margin.horizontal();
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

    Output run(Tree& tree, Box& box, Input input) override {
        Au blockSize = 0_au;
        Au inlineSize = input.knownSize.width.unwrapOr(0_au);

        if (box.children().len() == 0) {
            return _fragmentMonolithicBox(tree.fc, input);
        }

        Opt<_Breakpoint> bestBreakpoint = NONE;
        BaselinePositionsSet firstBaselineSet, lastBaselineSet;

        bool blockWasCompletelyLaidOut = false;

        Au lastMarginBottom = 0_au;

        auto st

        for (usize i = startAt; i < box.children().len(); ++i) {
            auto& c = box.children()[i];
            lookForRunningPosition(input, c);
            if (c.isRunningPositionedBox())
                continue;

            // We don't process break-before until some progress has been made to avoid infinite loops.
            if (tree.fc.allowBreak() and i > startAt) {
                auto breakpoint = _breakBefore(i, *c.style);
                if (breakpoint.isForced) {
                    return Output{
                        .size = Vec2Au{inlineSize, blockSize},
                        .completelyLaidOut = false,
                        .breakpoint = breakpoint,
                    };
                }

                if (bestBreakpoint) {
                    bestBreakpoint->overrideIfBetter(std::move(breakpoint));
                } else {
                    *bestBreakpoint = std::move(breakpoint);
                }
            }

            // TODO: Implement floating
            // if (c.style->float_ != Float::NONE)
            //     continue;

            Input childInput = {
                .intrinsic = input.intrinsic,
                .availableSpace = {input.availableSpace.x, 0_au},
                .containingBlock = {inlineSize, input.knownSize.y.unwrapOr(0_au)},
                .runningPosition = input.runningPosition,
                .pageNumber = input.pageNumber,
                .breakpointTraverser = input.breakpointTraverser.traverseInsideUsingIthChild(i),
                .pendingVerticalSizes = input.pendingVerticalSizes,
            };

            UsedSpacings usedSpacings{
                .padding = computePaddings(tree, c, childInput.containingBlock),
                .borders = computeBorders(tree, c),
                .margin = computeMargins(tree, c, childInput)
            };

            if (not c.isRemovedFromFlow()) {
                // TODO: collapsed margins for sibling elements

                Au maxPositive = max(0_au, usedSpacings.margin.top, lastMarginBottom);
                Au minNegative = min(0_au, usedSpacings.margin.top, lastMarginBottom);

                Au collapsedMargin = maxPositive - Math::abs(minNegative);
                blockSize += collapsedMargin - lastMarginBottom;
            }

            childInput.position = input.position + Vec2Au{usedSpacings.margin.start, blockSize};

            // HACK: Table Box mostly behaves like a block box, let's compute its capmin
            //       and avoid duplicating the layout code
            if (c.style->display == Display::Internal::TABLE_BOX) {
                childInput.capmin = _computeCapmin(tree, box, input, inlineSize);
            }

            _populateChildSpecifiedSizes(tree, c, childInput, usedSpacings, input.knownSize.x);

            auto output = input.fragment
                              ? layoutAndCommitBorderBox(tree, c, childInput, *input.fragment, usedSpacings)
                              : layoutBorderBox(tree, c, childInput, usedSpacings);

            if (not c.isRemovedFromFlow()) {
                blockSize += output.size.y + usedSpacings.margin.bottom;
                lastMarginBottom = usedSpacings.margin.bottom;
            }

            if (tree.fc.allowBreak() and output.breakpoint) {
                auto breakpoint = _breakInside(i, output.breakpoint.take(), *c.style);
                if (breakpoint.isForced) {
                    return Output{
                        .size = Vec2Au{inlineSize, blockSize},
                        .completelyLaidOut = false,
                        .breakpoint = breakpoint,
                    };
                }

                if (bestBreakpoint) {
                    bestBreakpoint->overrideIfBetter(std::move(breakpoint));
                } else {
                    *bestBreakpoint = std::move(breakpoint);
                }
            }

            if (i == startAt)
                firstBaselineSet = output.firstBaselineSet.translate(childInput.position.y - input.position.y);
            lastBaselineSet = output.lastBaselineSet.translate(childInput.position.y - input.position.y);

            if (tree.fc.allowBreak() and i < endChildren - 1) {
                auto breakpoint = _breakAfter(i, *c.style);
                if (breakpoint.isForced) {
                    return Output{
                        .size = Vec2Au{inlineSize, blockSize},
                        .completelyLaidOut = false,
                        .breakpoint = breakpoint,
                    };
                }

                if (bestBreakpoint) {
                    bestBreakpoint->overrideIfBetter(std::move(breakpoint));
                } else {
                    *bestBreakpoint = std::move(breakpoint);
                }
            }

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
            .breakpoint = bestBreakpoint,
            .firstBaselineSet = firstBaselineSet,
            .lastBaselineSet = lastBaselineSet,
        };
    }
};

export Rc<FormatingContext> constructBlockFormatingContext(Box&) {
    return makeRc<BlockFormatingContext>();
}

} // namespace Vaev::Layout
