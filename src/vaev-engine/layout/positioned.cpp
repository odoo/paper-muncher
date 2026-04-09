export module Vaev.Engine:layout.positioned;

import Karm.Math;

import :values;
import :layout.layout;
import :layout.values;

namespace Vaev::Layout {

Vec2Au _resolveOrigin(Vec2Au fragPosition, Vec2Au containingBlockOrigin, Position position) {
    if (position == Keywords::RELATIVE) {
        return fragPosition;
    } else if (position == Keywords::ABSOLUTE) {
        return containingBlockOrigin;
    } else {
        return {0_au, 0_au};
    }
}

// https://www.w3.org/TR/css-position-3/#resolving-insets
export RectAu _computeInsetModifiedContainingBlock(Tree& tree, Box& box, RectAu containingBlock) {
    auto const& style = *box.style;

    auto relativeTo = style.position == Keywords::FIXED
                          ? tree.viewport.small
                          : containingBlock;

    InsetsAu resolvedInsets = {};

    using InsetLength = CalcValue<PercentOr<Length>>;

    bool topIsAuto = style.insets->top.is<Keywords::Auto>();
    bool bottomIsAuto = style.insets->bottom.is<Keywords::Auto>();

    if (topIsAuto and bottomIsAuto) {
        // TODO
        logError("auto for top and bottom is not implemented");
    } else {
        if (not topIsAuto) {
            resolvedInsets.top = resolve(tree, box, *style.insets->top.is<InsetLength>(), relativeTo.height);
        }
        if (not bottomIsAuto) {
            resolvedInsets.bottom = resolve(tree, box, *style.insets->bottom.is<InsetLength>(), relativeTo.height);
        }
    }

    bool startIsAuto = style.insets->start.is<Keywords::Auto>();
    bool endIsAuto = style.insets->end.is<Keywords::Auto>();

    if (startIsAuto and endIsAuto) {
        // TODO
        logError("auto for start and end is not implemented");
    } else {
        if (not startIsAuto) {
            resolvedInsets.start = resolve(tree, box, *style.insets->start.is<InsetLength>(), relativeTo.width);
        }
        if (not endIsAuto) {
            resolvedInsets.end = resolve(tree, box, *style.insets->end.is<InsetLength>(), relativeTo.width);
        }
    }

    return containingBlock.shrink(resolvedInsets);
}

// https://www.w3.org/TR/css-position-3/#abspos-layout
export void _layoutAbsolutePositioned(Tree& tree, Box& box, RectAu containingBlock) {
    auto const& style = *box.style;

    // Absolute positioning not only takes a box out of flow, but also lays it out
    // in its containing block (after the final size of the containing block has been determined)
    // according to the absolute positioning layout model:

    // 1. First, its inset-modified containing block is calculated, defining its available space.
    auto availableSpace = _computeInsetModifiedContainingBlock(tree, box, containingBlock);

    // 2. Next, its width and height are resolved against this definite available space,
    //    as its preferred size capped by its maximum size (if any), floored by its minimum size.
    //    Percentages, however, are resolved against the original containing block size.

    UsedSpacings usedSpacings{
        .padding = computePaddings(tree, box, containingBlock.size()),
        .borders = computeBorders(tree, box),
        .margin = computeMargins(tree, box, containingBlock.size())
    };

    auto width = computeSpecifiedBorderBoxWidth(
        tree, box, style.sizing->width, containingBlock.size(), availableSpace.width,
        usedSpacings.padding.horizontal() + usedSpacings.borders.horizontal()
    );
}

export void layoutPositioned(Tree& tree, Frag& frag, RectAu containingBlock) {
    auto& style = frag.style();

    auto& metrics = frag.metrics;

    if (impliesRemovingFromFlow(style.position) or style.position == Keywords::RELATIVE) {
        auto origin = _resolveOrigin(metrics.position, containingBlock.topStart(), style.position);
        auto relativeTo = style.position == Keywords::FIXED
                              ? tree.viewport.small
                              : containingBlock;

        auto top = metrics.position.y;
        auto start = metrics.position.x;

        auto topOffset = style.insets->top;
        if (auto topOffsetCalc = topOffset.is<CalcValue<PercentOr<Length>>>()) {
            top = origin.y + resolve(tree, *frag.box, *topOffsetCalc, relativeTo.height);
        }

        auto startOffset = style.insets->start;
        if (auto startOffsetCalc = startOffset.is<CalcValue<PercentOr<Length>>>()) {
            start = origin.x + resolve(tree, *frag.box, *startOffsetCalc, relativeTo.width);
        }

        auto endOffset = frag.style().insets->end;
        if (auto endOffsetCalc = endOffset.is<CalcValue<PercentOr<Length>>>()) {
            start = (origin.x + relativeTo.width) - resolve(tree, *frag.box, *endOffsetCalc, relativeTo.width) - metrics.borderSize.width;
        }

        Vec2Au newPositionOffset = Vec2Au{start, top} - metrics.position;
        frag.offset(newPositionOffset);

        containingBlock = metrics.contentBox();
    }

    for (auto& c : frag.children()) {
        layoutPositioned(tree, c, containingBlock);
    }
}

export void lookForRunningPosition(Input& input, Box& box) {
    if (not input.runningPosition)
        return;

    if (box.style->position.is<RunningPosition>()) {
        auto& runningMap = input.runningPosition.peek();
        runningMap.add(input.pageNumber, box);
    }
}

} // namespace Vaev::Layout
