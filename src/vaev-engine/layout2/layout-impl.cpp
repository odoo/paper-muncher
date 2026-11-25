module Vaev.Engine;

import :layout.box;

import :layout2.block;
import :layout.values;

namespace Vaev::Layout2 {

Metrics computeMetrics(Layout::Box const& box, LogicalSize<Opt<Au>> const& containingBlock) {
    auto paddingLeft = Layout::resolve(
        box.style->padding->start,
        containingBlock.inline_.unwrapOr(0_au)
    );

    auto paddingRight = Layout::resolve(
        box.style->padding->end,
        containingBlock.inline_.unwrapOr(0_au)
    );

    auto paddingTop = Layout::resolve(
        box.style->padding->top,
        containingBlock.inline_.unwrapOr(0_au)
    );

    auto paddingBottom = Layout::resolve(
        box.style->padding->bottom,
        containingBlock.inline_.unwrapOr(0_au)
    );

    // FIXME: Implement writing-mode translation
    auto paddings = LogicalInsets{
        .inlineStart = paddingLeft,
        .inlineEnd = paddingRight,
        .blockStart = paddingTop,
        .blockEnd = paddingBottom,
    };

    // FIXME: Move the resolution of borders for computed values, this should just be a cast to Au.
    auto physicalBorders = Layout::computeBorders(box);

    // FIXME: Implement writing-mode translation
    auto borders = LogicalInsets{
        .inlineStart = physicalBorders.start,
        .inlineEnd = physicalBorders.end,
        .blockStart = physicalBorders.top,
        .blockEnd = physicalBorders.bottom,
    };

    auto const _resolveSize = [&](auto const& inline_, auto const& block) {
        LogicalSize<Opt<Au>> size = {NONE, NONE};

        // FIXME: Intrinsic sizing.
        if (auto [calc] = inline_.template is<Style::PxPercent>()) {
            if (auto const [resolved] = Layout::resolve(calc, NONE)) {
                if (box.style->writingMode == Keywords::HORIZONTAL_TB) {
                    size.inline_ = Some(resolved);
                } else {
                    size.block = Some(resolved);
                }
                if (box.style->writingMode == Keywords::HORIZONTAL_TB) {
                    if (box.style->boxSizing == Keywords::CONTENT_BOX) {
                        size.inline_ = Some(resolved + paddings.inlineSum() + borders.inlineSum());
                    } else {
                        size.inline_ = Some(max(resolved, paddings.inlineSum() + borders.inlineSum()));
                    }
                } else {
                    if (box.style->boxSizing == Keywords::CONTENT_BOX) {
                        size.block = Some(resolved + paddings.blockSum() + borders.blockSum());
                    } else {
                        size.block = Some(max(resolved, paddings.blockSum() + borders.blockSum()));
                    }
                }
            }
        }

        // FIXME: Intrinsic sizing.
        if (auto [calc] = block.template is<Style::PxPercent>()) {
            if (auto const [resolved] = Layout::resolve(calc, NONE)) {
                if (box.style->writingMode == Keywords::HORIZONTAL_TB) {
                    if (box.style->boxSizing == Keywords::CONTENT_BOX) {
                        size.block = Some(resolved + paddings.blockSum() + borders.blockSum());
                    } else {
                        size.block = Some(max(resolved, paddings.blockSum() + borders.blockSum()));
                    }
                } else {
                    if (box.style->boxSizing == Keywords::CONTENT_BOX) {
                        size.inline_ = Some(resolved + paddings.inlineSum() + borders.inlineSum());
                    } else {
                        size.inline_ = Some(max(resolved, paddings.inlineSum() + borders.inlineSum()));
                    }
                }
            }
        }

        return size;
    };

    // FIXME: Implement writing-mode translation
    LogicalSize<Opt<Au>> size = _resolveSize(box.style->sizing->width, box.style->sizing->height);
    LogicalSize<Opt<Au>> minSize = _resolveSize(box.style->sizing->minWidth, box.style->sizing->minHeight);
    LogicalSize<Opt<Au>> maxSize = _resolveSize(box.style->sizing->maxWidth, box.style->sizing->maxHeight);

    return Metrics{
        .borders = borders,
        .paddings = paddings,
        .size = size,
        .minSize = minSize,
        .maxSize = maxSize,
    };
}


Output layout(Layout::Box& box, Constraints const& constraints, Opt<BreakOpportunity const&> breakOpportunity, Opt<BreakNode const&> breakTree) {
    auto input = Input{
        .constraints = constraints,
        .metrics = computeMetrics(box, constraints.containingBlock),
        .breakOpportunity = breakOpportunity,
        .breakTree = breakTree,
    };

    if (box.style->display == Style::Display::BLOCK) {
        return BlockFormattingContext{}.run(box, input);
    } else {
        logWarn("falling back to bfc");
        return BlockFormattingContext{}.run(box, input);
    }
}

} // namespace Vaev::Layout2
