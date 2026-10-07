export module Vaev.Engine:layout2.block;

import Karm.Logger;

import :layout2.layout;
import :layout2.box;
import :layout2.fragBuilder;
import :layout.layout;

namespace Vaev::Layout2 {

export struct BlockFormattingContext {
    static LogicalInsets<Au> _computeChildMargins(Layout::Tree& tree, Layout::Box& child, LogicalSize<Opt<Au>> const& containingBlock) {
        auto childMargins = resolveMargins(tree, child, {Some(0_au), Some(0_au)});

        auto childMarginsResolved = LogicalInsets<Au>{};

        // FIXME: Follow spec
        childMarginsResolved.blockStart = childMargins.blockStart.unwrapOr(0_au);
        childMarginsResolved.blockEnd = childMargins.blockEnd.unwrapOr(0_au);

        // FIXME: Writing mode
        // If 'width' is set to 'auto', any other 'auto' values become '0' and 'width' follows from the resulting equality.
        if (child.style->sizing->width.is<Keywords::Auto>()) {
            childMarginsResolved.inlineStart = childMargins.inlineStart.unwrapOr(0_au);
            childMarginsResolved.inlineEnd = childMargins.inlineEnd.unwrapOr(0_au);
            return childMarginsResolved;
        }

        auto childMetrics = computeMetrics(tree, child, {Some(0_au), Some(0_au)});

        // If both 'margin-left' and 'margin-right' are 'auto', their used values are equal.
        // This horizontally centers the element with respect to the edges of the containing block.
        if (not childMargins.inlineStart and not childMargins.inlineEnd) {
            // FIXME: Pass gud cb.

            auto inlineSize = childMetrics.size.inline_.unwrapOr(0_au);
            auto inlineMargin = containingBlock.inline_.unwrapOr(0_au) / 2 - (inlineSize / 2);

            childMarginsResolved.inlineStart = inlineMargin;
            childMarginsResolved.inlineEnd = inlineMargin;
        }

        return childMarginsResolved;
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

        if (box.establishesFc or metrics.paddings.blockStart != 0_au or metrics.borders.blockStart != 0_au) {
            blockOffset = Some(pendingMargin.sum());
            pendingMargin = PendingMargin{};
        }

        // FIXME: Express in logical units instead
        Vec2Au cursor = {
            metrics.borders.inlineStart + metrics.paddings.inlineStart,
            metrics.borders.blockStart + metrics.paddings.blockStart
        };

        for (usize i = 0; i < box.children().len(); i++) {
            auto& child = box.children()[i];

            if (child.isRemovedFromFlow()) {
                continue;
            }

            auto childMargins = _computeChildMargins(tree, child, constraints.containingBlock);

            auto childConstraints = Constraints{
                .containingBlock = {
                    Some(input.metrics.size.inline_.unwrapOr(autoInlineSize) - metrics.borders.inlineSum() - metrics.paddings.inlineSum()),
                    NONE,
                },
                .pendingMargin = pendingMargin,
                .margins = childMargins,
            };

            auto childOutput = layout(tree, child, childConstraints);

            if (auto [childOffset] = childOutput.blockOffset) {
                if (not blockOffset) {
                    blockOffset = Some(childOffset);
                } else {
                    cursor.y += childOffset;
                }
            }

            fragBuilder.addChildIfAny(childOutput.fragment, cursor + Vec2Au{childMargins.inlineStart, 0_au});

            cursor.y += childOutput.size.block;

            pendingMargin = childOutput.pendingMargin;

            if (inlineSizeBehavior == AutoSizeBehavior::FIT_CONTENT or not constraints.containingBlock.inline_) {
                autoInlineSize = max(autoInlineSize, childOutput.size.inline_);
            }
        }

        auto size = LogicalSize{
            input.metrics.size.inline_.unwrapOr(autoInlineSize + metrics.paddings.inlineEnd + metrics.borders.inlineEnd),
            input.metrics.size.block.unwrapOr(cursor.height + metrics.paddings.blockEnd + metrics.borders.blockEnd),
        };

        // FIXME: Check for fixed height too.
        if (blockOffset and (box.establishesFc or metrics.paddings.blockEnd != 0_au or metrics.borders.blockEnd != 0_au)) {
            cursor.y += pendingMargin.sum();
            pendingMargin = PendingMargin{};
        }

        if (not blockOffset and size.block > 0_au) {
            blockOffset = Some(pendingMargin.sum());
            pendingMargin = PendingMargin{};
        }

        pendingMargin.add(constraints.margins.blockEnd);

        return Output{
            .pendingMargin = pendingMargin,
            .size = size,
            .blockOffset = blockOffset,
            .fragment = Some(fragBuilder.buildBox(input, size))
        };
    }
};

} // namespace Vaev::Layout2
