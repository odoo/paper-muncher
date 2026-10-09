module;

#include <karm/macros>

export module Vaev.Engine:layout2.block;

import Karm.Logger;

import :layout2.layout;
import :layout2.box;
import :layout2.fragBuilder;
import :layout.layout;

namespace Vaev::Layout2 {

export struct BlockFormattingContext {
    static LogicalInsets<Au> _computeChildMargins(Layout::Tree& tree, Layout::Box& child, LogicalSize<Opt<Au>> const& containingBlock) {
        auto maybeMargins = resolveMargins(tree, child, {Some(0_au), Some(0_au)});

        auto margins = LogicalInsets<Au>{};

        // FIXME: Follow spec
        margins.blockStart = maybeMargins.blockStart.unwrapOr(0_au);
        margins.blockEnd = maybeMargins.blockEnd.unwrapOr(0_au);

        // FIXME: Writing mode
        // If 'width' is set to 'auto', any other 'auto' values become '0' and 'width' follows from the resulting equality.
        if (child.style->sizing->width.is<Keywords::Auto>()) {
            margins.inlineStart = maybeMargins.inlineStart.unwrapOr(0_au);
            margins.inlineEnd = maybeMargins.inlineEnd.unwrapOr(0_au);
            return margins;
        }

        // FIXME: Pass gud cb.
        auto childMetrics = computeMetrics(tree, child, {Some(0_au), Some(0_au)});
        auto inlineSize = childMetrics.size.inline_.unwrapOr(0_au);

        // If both 'margin-left' and 'margin-right' are 'auto', their used values are equal.
        // This horizontally centers the element with respect to the edges of the containing block.
        if (not maybeMargins.inlineStart and not maybeMargins.inlineEnd) {
            auto inlineMargin = containingBlock.inline_.unwrapOr(0_au) / 2 - (inlineSize / 2);
            margins.inlineStart = inlineMargin;
            margins.inlineEnd = inlineMargin;
        } else if (maybeMargins.inlineStart and not maybeMargins.inlineEnd) {
            margins.inlineStart = *maybeMargins.inlineStart;
            // FIXME: Should be clamped.
            margins.inlineEnd = containingBlock.inline_.unwrapOr(0_au) - inlineSize - *maybeMargins.inlineStart;
        } else if (not maybeMargins.inlineStart and maybeMargins.inlineEnd) {
            // FIXME: Should be clamped.
            margins.inlineStart = containingBlock.inline_.unwrapOr(0_au) - inlineSize - *maybeMargins.inlineEnd;
            margins.inlineEnd = *maybeMargins.inlineEnd;
        } else {
            margins.inlineStart = *maybeMargins.inlineStart;
            margins.inlineEnd = *maybeMargins.inlineEnd;
        }

        return margins;
    }

    Output run(Layout::Tree& tree, Layout::Box& box, Input const& input) {
        auto const& [constraints, metrics, _, _] = input;

        auto fragBuilder = FragBuilder{tree, box};

        auto inlineSizeBehavior = input.constraints.inlineAutoSizeBehavior;

        Au autoInlineSize = 0_au;
        if (inlineSizeBehavior == AutoSizeBehavior::STRETCH_FIT and constraints.containingBlock.inline_) {
            auto [inlineStartMargin, inlineEndMargin] = resolveInlineMargins(tree, box, constraints.containingBlock);
            auto inlineMargins = inlineStartMargin.unwrapOr(0_au) + inlineEndMargin.unwrapOr(0_au);
            autoInlineSize = max(metrics.borders.inlineSum() + metrics.paddings.inlineSum(), *constraints.containingBlock.inline_ - inlineMargins);
        } else {
            autoInlineSize = metrics.borders.inlineSum() + metrics.paddings.inlineSum();
        }

        PendingMargin pendingMargin = constraints.pendingMargin;
        pendingMargin.add(constraints.margins.blockStart);

        Opt<Au> blockOffset = NONE;

        auto initialBreakBefore = box.style->break_->before;

        Opt<BreakOpportunity> bestBreakOpportunity = NONE;

        auto overrideBestBreakOpportunityIfBetter = [&](BreakOpportunity&& other) {
            if (auto& [old] = bestBreakOpportunity) {
                if (other.appeal >= old.appeal) {
                    bestBreakOpportunity = Some(std::move(other));
                }
            } else {
                bestBreakOpportunity = Some(std::move(other));
            }
        };

        if (not constraints.collapseMargins or box.establishesFc or metrics.paddings.blockStart != 0_au or metrics.borders.blockStart != 0_au) {
            blockOffset = Some(pendingMargin.sum());
            pendingMargin = PendingMargin{};

            if (constraints.fragmentainer) {
                auto appeal = BreakAppeal::PERFECT;
                if (box.style->break_->after == BreakBetween::AVOID)
                    appeal = BreakAppeal::DROPS_AVOID;

                overrideBestBreakOpportunityIfBetter(BreakOpportunity{
                    .inner = NONE,
                    .index = 0,
                    .consumedBlockSize = *blockOffset,
                    .appeal = appeal,
                });
            }
        }

        // FIXME: Express in logical units instead
        Vec2Au cursor = {
            metrics.borders.inlineStart + metrics.paddings.inlineStart,
            metrics.borders.blockStart + metrics.paddings.blockStart
        };

        BreakBetween previousBreakAfter = BreakBetween::AUTO;

        for (usize i = 0; i < box.children().len(); i++) {
            auto& child = box.children()[i];

            if (child.isRemovedFromFlow()) {
                continue;
            }

            auto childMargins = _computeChildMargins(tree, child, constraints.containingBlock);

            Opt<Fragmentainer> childFragmentainer = NONE;

            if (auto const& [fragmentainer] = constraints.fragmentainer) {
                childFragmentainer = Some(fragmentainer.at(blockOffset.unwrapOr(0_au) + cursor.y));
            }

            auto const childConstraints = Constraints{
                .containingBlock = {
                    Some(input.metrics.size.inline_.unwrapOr(autoInlineSize) - metrics.borders.inlineSum() - metrics.paddings.inlineSum()),
                    NONE,
                },
                .pendingMargin = pendingMargin,
                .margins = childMargins,
                .fragmentainer = childFragmentainer,
            };

            auto childOutput = layout(tree, child, childConstraints);

            Opt<BreakNode> breakTree = NONE;

            if (auto [placed] = childOutput.is<Placed>()) {
                if (isEmpty(fragBuilder._children)) {
                    initialBreakBefore = max(initialBreakBefore, placed.initialBreakBefore);
                }

                if (constraints.fragmentainer and not isEmpty(fragBuilder._children) and (placed.initialBreakBefore == BreakBetween::PAGE or previousBreakAfter == BreakBetween::PAGE)) {
                    return Placed{
                        .fragment = fragBuilder.buildBox(input, {autoInlineSize, cursor.height}),
                        .blockOffset = blockOffset,
                        .initialBreakBefore = initialBreakBefore,
                        .breakState = Broke{
                            .tree = BreakNode{
                                // https://www.w3.org/TR/css-break-3/#break-margins
                                // When a forced break occurs there, adjoining margins before the break are truncated,
                                // but margins after the break are preserved.
                                .preservedMargin = true,
                                ._inner = BreakNode::BlockResumeData{},
                                ._children = {},
                            },
                            .appeal = BreakAppeal::PERFECT,
                            .forced = true,
                        },
                    };
                }

                if (auto [childOffset] = placed.blockOffset) {
                    // fragmentainerBudget -= childOffset;
                    if (not blockOffset) {
                        blockOffset = Some(childOffset);
                    } else {
                        cursor.y += childOffset;
                    }
                }

                fragBuilder.addChild(placed.fragment, cursor + Vec2Au{childMargins.inlineStart, 0_au});

                if (auto [completed] = placed.breakState.is<Completed>()) {
                    previousBreakAfter = completed.finalBreakAfter;
                    pendingMargin = completed.pendingMargin;

                    if (completed.bestBreakOpportunity)
                        overrideBestBreakOpportunityIfBetter(completed.bestBreakOpportunity.take());
                }

                if (auto broke = placed.breakState.is<Broke>()) {
                    if (broke->forced) {
                        auto b = std::move(broke.take().tree);
                        auto c = Vec<BreakNode>{};
                        c.pushBack(std::move(b));

                        return Placed{
                            .fragment = fragBuilder.buildBox(input, {autoInlineSize, cursor.height}),
                            .blockOffset = blockOffset,
                            .initialBreakBefore = initialBreakBefore,
                            .breakState = Broke{
                                .tree = BreakNode{
                                    // https://www.w3.org/TR/css-break-3/#break-margins
                                    // When a forced break occurs there, adjoining margins before the break are truncated,
                                    // but margins after the break are preserved.
                                    .preservedMargin = true,
                                    ._inner = BreakNode::BlockResumeData{},
                                    ._children = std::move(c),
                                },
                                .appeal = BreakAppeal::PERFECT,
                                .forced = true,
                            },
                        };

                    }
                }

                auto childSize = LogicalSize<Au>::fromPhysical(placed.fragment->borderBox().size(), box.style->writingMode);

                cursor.y += childSize.block;

                // FIXME:
                // fragmentainerBudget -= childSize.block;
            } else if (auto [abort] = childOutput.is<Abort>()) {
                // NOTE: As of now, its the only abort reason.
                auto it = abort.is<NeedsEarlierBreak>();

                childOutput = layout(tree, child, childConstraints, Some(it->opportunity));

                // NOTE: This acts as an assertion that the relayout must produce something.
                if (auto [childOffset] = placed.blockOffset) {
                    // fragmentainerBudget -= childOffset;
                    if (not blockOffset) {
                        blockOffset = Some(childOffset);
                    } else {
                        cursor.y += childOffset;
                    }
                }

                fragBuilder.addChild(placed.fragment, cursor + Vec2Au{childMargins.inlineStart, 0_au});

                if (auto [completed] = placed.breakState.is<Completed>()) {
                    previousBreakAfter = completed.finalBreakAfter;
                    pendingMargin = completed.pendingMargin;

                    if (completed.bestBreakOpportunity)
                        overrideBestBreakOpportunityIfBetter(completed.bestBreakOpportunity.take());
                }

                if (auto broke = placed.breakState.is<Broke>()) {
                    if (broke->forced) {
                        auto b = std::move(broke.take().tree);
                        auto c = Vec<BreakNode>{};
                        c.pushBack(std::move(b));

                        return Placed{
                            .fragment = fragBuilder.buildBox(input, {autoInlineSize, cursor.height}),
                            .blockOffset = blockOffset,
                            .initialBreakBefore = initialBreakBefore,
                            .breakState = Broke{
                                .tree = BreakNode{
                                    // https://www.w3.org/TR/css-break-3/#break-margins
                                    // When a forced break occurs there, adjoining margins before the break are truncated,
                                    // but margins after the break are preserved.
                                    .preservedMargin = true,
                                    ._inner = BreakNode::BlockResumeData{},
                                    ._children = std::move(c),
                                },
                                .appeal = BreakAppeal::PERFECT,
                                .forced = true,
                            },
                        };

                    }
                }

                auto childSize = LogicalSize<Au>::fromPhysical(placed.fragment->borderBox().size(), box.style->writingMode);

                cursor.y += childSize.block;

                // FIXME:
                // fragmentainerBudget -= childSize.block;
            } else {
                unreachable();
            }

            // FIXME: Rework and bring back in.
            // if (inlineSizeBehavior == AutoSizeBehavior::FIT_CONTENT or not constraints.containingBlock.inline_) {
            //     autoInlineSize = max(autoInlineSize, childOutput.size.inline_);
            // }
        }

        // FIXME: Check for fixed height too.
        if (blockOffset and (box.establishesFc or metrics.paddings.blockEnd != 0_au or metrics.borders.blockEnd != 0_au)) {
            // FIXME
            // fragmentainerBudget = fragmentainerBudget - pendingMargin.sum();
            cursor.y += pendingMargin.sum();
            pendingMargin = PendingMargin{};
        }

        auto finalBreakAfter = max(previousBreakAfter, box.style->break_->after);

        // FIXME
        // fragmentainerBudget -= metrics.paddings.blockEnd + metrics.borders.blockEnd;

        auto size = LogicalSize{
            input.metrics.size.inline_.unwrapOr(autoInlineSize + metrics.paddings.inlineEnd + metrics.borders.inlineEnd),
            input.metrics.size.block.unwrapOr(cursor.height + metrics.paddings.blockEnd + metrics.borders.blockEnd),
        };

        if (not blockOffset and size.block > 0_au) {
            blockOffset = Some(pendingMargin.sum());
            pendingMargin = PendingMargin{};
        }

        pendingMargin.add(constraints.margins.blockEnd);

        return Placed{
            .fragment = fragBuilder.buildBox(input, size),
            .blockOffset = blockOffset,
            .initialBreakBefore = initialBreakBefore,
            .breakState = Completed{
                .pendingMargin = pendingMargin,
                .finalBreakAfter = finalBreakAfter,
                .bestBreakOpportunity = std::move(bestBreakOpportunity),
            }
        };
    }
};

} // namespace Vaev::Layout2
