export module Vaev.Engine:layout2.block;

import Karm.Logger;

import :layout2.layout;
import :layout2.box;
import :layout2.fragBuilder;
import :layout.layout;

namespace Vaev::Layout2 {

export struct BlockFormattingContext {
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

            // FIXME: Handle auto margins.
            auto childMargins = resolveMargins(tree, child, {Some(0_au), Some(0_au)}).map([](Opt<Au> x) {
                return x.unwrapOr(0_au);
            });

            // https://www.w3.org/TR/CSS22/box.html#collapsing-margins

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
