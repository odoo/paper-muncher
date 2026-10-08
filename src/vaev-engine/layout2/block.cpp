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
        auto const& [constraints, metrics] = input;

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

        if (not constraints.collapseMargins or box.establishesFc or metrics.paddings.blockStart != 0_au or metrics.borders.blockStart != 0_au) {
            blockOffset = Some(pendingMargin.sum());
            pendingMargin = PendingMargin{};
        }

        // FIXME: Express in logical units instead
        Vec2Au cursor = {
            metrics.borders.inlineStart + metrics.paddings.inlineStart,
            metrics.borders.blockStart + metrics.paddings.blockStart
        };

        Opt<BreakOpportunity> bestBreakOpportunity = NONE;

        for (usize i = 0; i < box.children().len(); i++) {
            auto& child = box.children()[i];

            if (child.isRemovedFromFlow()) {
                continue;
            }

            auto childMargins = _computeChildMargins(tree, child, constraints.containingBlock);

            Opt<Fragmentainer> childFragmentainer = NONE;

            if (auto [fragmentainer] = constraints.fragmentainer) {
                childFragmentainer = Some(fragmentainer.at(blockOffset.unwrapOr(0_au) + cursor.y));
            }

            auto childConstraints = Constraints{
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

            auto placeChild = [&](Placed& placed) {
                if (auto [childOffset] = placed.blockOffset) {
                    // fragmentainerBudget -= childOffset;
                    if (not blockOffset) {
                        blockOffset = Some(childOffset);
                    } else {
                        cursor.y += childOffset;
                    }
                }

                if (auto [completed] = placed.breakState.is<Completed>()) {
                    pendingMargin = completed.pendingMargin;
                    fragBuilder.addChild(placed.fragment, cursor + Vec2Au{childMargins.inlineStart, 0_au});

                    // TODO: Extract this to an helper
                    if (auto [breakOpportunity] = completed.bestBreakOpportunity) {
                        if (not bestBreakOpportunity or breakOpportunity.appeal >= bestBreakOpportunity->appeal) {
                            bestBreakOpportunity = std::move(completed.bestBreakOpportunity);
                        }
                    }
                }

                auto childSize = LogicalSize<Au>::fromPhysical(placed.fragment->borderBox().size(), box.style->writingMode);

                cursor.y += childSize.block;

                // FIXME:
                // fragmentainerBudget -= childSize.block;
            };

            if (auto [placed] = childOutput.is<Placed>()) {
                placeChild(placed);
            } else if (auto [abort] = childOutput.is<Abort>()) {
                // NOTE: As of now, its the only abort reason.
                auto it = abort.is<NeedsEarlierBreak>().expect();

                childConstraints.replayedBreak = Some(std::move(it.opportunity));
                childOutput = layout(tree, child, childConstraints);

                // NOTE: This acts as an assertion that the relayout must produce something.
                placeChild(childOutput.is<Placed>().take());
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
            .breakState = Completed{
                .pendingMargin = pendingMargin,
                .bestBreakOpportunity = std::move(bestBreakOpportunity),
            }
        };
    }
};

} // namespace Vaev::Layout2
