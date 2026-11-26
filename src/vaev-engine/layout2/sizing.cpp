export module Vaev.Engine:layout2.sizing;

import :style;
import :layout2.layout;

namespace Vaev::Layout2 {

IntrinsicContributions intrinsicInlineContributions(Layout::Box const& box) {
    // https://www.w3.org/TR/css-sizing-3/#min-percentage-contribution
    // FIXME: Writing mode translation.

    auto paddingLeft = Layout::resolve(box.style->padding->start, NONE).unwrapOr(0_au);

    auto paddingRight = Layout::resolve(box.style->padding->end, NONE).unwrapOr(0_au);

    auto paddingInline = paddingLeft + paddingRight;

    auto marginVisitor = Visitor{
        [](Keywords::Auto) {
            return 0_au;
        },
        [](Style::PxPercent const& calc) {
            return Layout::resolve(calc, NONE).unwrapOr(0_au);
        }
    };

    auto borders = Layout::computeBorders(box);

    auto borderInline = borders.horizontal();

    auto marginLeft = box.style->margin->start.visit(marginVisitor);
    auto marginRight = box.style->margin->end.visit(marginVisitor);
    auto marginInline = marginLeft + marginRight;

    Opt<Au> minInlineSize = Some(0_au);
    if (auto [calc] = box.style->sizing->minWidth.is<Style::PxPercent>()) {
        if (auto const [resolved] = Layout::resolve(calc, NONE)) {
            if (box.style->boxSizing == Keywords::CONTENT_BOX) {
                minInlineSize = Some(resolved + paddingInline + borderInline);
            } else {
                minInlineSize = Some(resolved);
            }
        } else {
            minInlineSize = NONE;
        }
    }

    Opt<Au> maxInlineSize = Some(Limits<Au>::MAX);
    if (auto [calc] = box.style->sizing->maxWidth.is<Style::PxPercent>()) {
        if (auto const [resolved] = Layout::resolve(calc, NONE)) {
            if (box.style->boxSizing == Keywords::CONTENT_BOX) {
                maxInlineSize = Some(resolved + paddingInline + borderInline);
            } else {
                maxInlineSize = Some(resolved);
            }
        } else {
            maxInlineSize = NONE;
        }
    }

    auto contributions = IntrinsicContributions{};

    if (auto [calc] = box.style->sizing->width.is<Style::PxPercent>()) {
        if (auto const [resolved] = Layout::resolve(calc, NONE)) {
            if (box.style->boxSizing == Keywords::CONTENT_BOX) {
                contributions.min = resolved + paddingInline + borderInline;
                contributions.max = contributions.min;
            } else {
                contributions.min = resolved;
                contributions.max = contributions.min;
            }
        } else {
            IntrinsicSizes intrinsicSizes = intrinsicInlineContentSizes(box);
            if (box.isReplaced()) {
                contributions.min = 0_au;
            } else {
                contributions.min = intrinsicSizes.minContentSize;
            }
            contributions.max = intrinsicSizes.maxContentSize;
        }
    } else {
        IntrinsicSizes intrinsicSizes = intrinsicInlineContentSizes(box);
        contributions.min = intrinsicSizes.minContentSize;
        contributions.max = intrinsicSizes.maxContentSize;
    }

    contributions.min = clamp(
        contributions.min,
        minInlineSize.unwrapOr(0_au),
        maxInlineSize.unwrapOr(box.isReplaced() ? 0_au : Limits<Au>::MAX)
    );

    contributions.max = clamp(
        contributions.max,
        minInlineSize.unwrapOr(0_au),
        maxInlineSize.unwrapOr(Limits<Au>::MAX)
    );

    contributions.min += marginInline;
    contributions.max += marginInline;

    return contributions;
}

export Style::Size const& logicalWidth(Style::ComputedValues const& style) {
    if (style.writingMode == Keywords::HORIZONTAL_TB) {
        return style.sizing->width;
    } else {
        return style.sizing->height;
    }
}

export Style::Size const& logicalHeight(Style::ComputedValues const& style) {
    if (style.writingMode == Keywords::HORIZONTAL_TB) {
        return style.sizing->height;
    } else {
        return style.sizing->width;
    }
}

} // namespace Vaev::Layout2
