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
export RectAu _computeInsetModifiedContainingBlock(Tree& tree, Box& box, RectAu containingBlock, RectAu staticPositionRect) {
    auto const& style = *box.style;

    auto relativeTo = style.position == Keywords::FIXED
                          ? tree.viewport.small
                          : containingBlock;

    InsetsAu resolvedInsets = {};

    using InsetLength = CalcValue<PercentOr<Length>>;

    bool startIsAuto = style.insets->start.is<Keywords::Auto>();
    bool endIsAuto = style.insets->end.is<Keywords::Auto>();

    if (startIsAuto and endIsAuto) {
        // -> for self-start alignment or its equivalent
        if (oneOf(style.aligns.justifySelf, Align::SELF_START, Align::AUTO, Align::NORMAL, Align::STRETCH)) {
            // Set its start-edge inset property to the static position, and its end-edge inset property to zero.
            resolvedInsets.start = staticPositionRect.start() - containingBlock.start();
        }

        // -> for self-end alignment or its equivalent
        else if (style.aligns.justifySelf == Align::SELF_END) {
            // Set its end-edge inset property to the static position, and its start-edge inset property to zero.
            resolvedInsets.end = containingBlock.end() - staticPositionRect.end();

        }

        // -> for center alignment
        else if (style.aligns.justifySelf == Align::CENTER) {
            // Let start distance be the distance from the center of its static-position rectangle to the start edge of its containing block,
            // and end distance be the distance from the center of its static-position rectangle to the end edge of its containing block.
            Au staticPositionCenter = staticPositionRect.start() + Au{f64{staticPositionRect.width} / 2.0};
            Au startDistance = Math::abs(staticPositionCenter - containingBlock.start());
            Au endDistance = Math::abs(staticPositionCenter - containingBlock.end());

            // If start distance is less than or equal to end distance,
            if (startDistance <= endDistance) {
                // then set the start-edge inset property to zero, and set the end-edge inset property
                // to (containing block size - 2 × |start distance|);
                resolvedInsets.end = containingBlock.width - (2_au * startDistance);
            } else {
                //	otherwise, set the end-edge inset property to zero and the start-edge inset property
                //	to (containing block size - 2 × |end distance|).
                resolvedInsets.start = containingBlock.width - (2_au * endDistance);
            }
        } else {
            // TODO
            logError("unsuported align on absolute positioned box");
        }

        if (resolvedInsets.start < 0_au or resolvedInsets.end < 0_au) {
            // TODO: Implement spec for this
            logError("auto for start and end produces negative insets");
        }
    } else {
        if (not startIsAuto) {
            resolvedInsets.start = resolve(tree, box, *style.insets->start.is<InsetLength>(), relativeTo.width);
        }
        if (not endIsAuto) {
            resolvedInsets.end = resolve(tree, box, *style.insets->end.is<InsetLength>(), relativeTo.width);
        }
    }

    bool topIsAuto = style.insets->top.is<Keywords::Auto>();
    bool bottomIsAuto = style.insets->bottom.is<Keywords::Auto>();

    if (topIsAuto and bottomIsAuto) {
        // -> for self-start alignment or its equivalent
        if (oneOf(style.aligns.alignSelf, Align::SELF_START, Align::AUTO, Align::NORMAL, Align::STRETCH)) {
            // Set its start-edge inset property to the static position, and its end-edge inset property to zero.
            resolvedInsets.top = staticPositionRect.top() - containingBlock.top();
        }

        // -> for self-end alignment or its equivalent
        else if (style.aligns.alignSelf == Align::SELF_END) {
            // Set its end-edge inset property to the static position, and its start-edge inset property to zero.
            resolvedInsets.bottom = containingBlock.bottom() - staticPositionRect.bottom();

        }

        // -> for center alignment
        else if (style.aligns.alignSelf == Align::CENTER) {
            // Let start distance be the distance from the center of its static-position rectangle to the start edge of its containing block,
            // and end distance be the distance from the center of its static-position rectangle to the end edge of its containing block.
            Au staticPositionCenter = staticPositionRect.top() + Au{f64{staticPositionRect.height} / 2.0};
            Au startDistance = Math::abs(staticPositionCenter - containingBlock.top());
            Au endDistance = Math::abs(staticPositionCenter - containingBlock.bottom());

            // If start distance is less than or equal to end distance,
            if (startDistance <= endDistance) {
                // then set the start-edge inset property to zero, and set the end-edge inset property
                // to (containing block size - 2 × |start distance|);
                resolvedInsets.bottom = containingBlock.height - (2_au * startDistance);
            } else {
                //	otherwise, set the end-edge inset property to zero and the start-edge inset property
                //	to (containing block size - 2 × |end distance|).
                resolvedInsets.top = containingBlock.height - (2_au * endDistance);
            }
        } else {
            // TODO
            logError("unsuported align on absolute positioned box");
        }

        if (resolvedInsets.top < 0_au or resolvedInsets.bottom < 0_au) {
            // TODO: Implement spec for this
            logError("auto for start and end produces negative insets");
        }
    } else {
        if (not topIsAuto) {
            resolvedInsets.top = resolve(tree, box, *style.insets->top.is<InsetLength>(), relativeTo.height);
        }
        if (not bottomIsAuto) {
            resolvedInsets.bottom = resolve(tree, box, *style.insets->bottom.is<InsetLength>(), relativeTo.height);
        }
    }

    return containingBlock.shrink(resolvedInsets);
}

export bool isAbsoluteOrFixedPositionedContainingBlock(Box const& box) {
    // https://www.w3.org/TR/css-transforms-1/#transform-rendering
    // For elements whose layout is governed by the CSS box model, any value other than none for the transform property
    // also causes the element to establish a containing block for all descendants. Its padding box will be used to layout
    // for all of its absolute-position descendants, fixed-position descendants, and descendant fixed background attachments.
    if (box.style->transform->transform != Keywords::NONE) {
        return true;
    }

    // TODO: Implement interactions with filter-effects-* and css-transforms-2 when those will be implemented.
    return false;
}

// https://www.w3.org/TR/css-position-3/#absolute-positioning-containing-block
export bool isAbsolutePositionedContainingBlock(Box const& box) {
    // NOTE: There is a helpful but non-normative article on MDN
    // https://developer.mozilla.org/en-US/docs/Web/CSS/Guides/Display/Containing_block#identifying_the_containing_block

    if (box.style->position != Keywords::STATIC) {
        return true;
    }

    if (isAbsoluteOrFixedPositionedContainingBlock(box)) {
        return true;
    }

    return false;
}

// https://www.w3.org/TR/css-position-3/#relpos-insets
export Vec2Au resolveRelativePositionedOffset(Tree& tree, Box& box, Vec2Au containingBlock) {
    auto const& style = *box.style;

    Vec2Au resolvedOffset = {};

    bool leftIsAuto = style.insets->start.is<Keywords::Auto>();
    bool rightIsAuto = style.insets->end.is<Keywords::Auto>();

    if (not leftIsAuto and not rightIsAuto) {
        resolvedOffset.x = resolve(tree, box, *style.insets->start.is<CalcValue<PercentOr<Length>>>(), containingBlock.width);
    } else {
        if (not leftIsAuto) {
            resolvedOffset.x = resolve(tree, box, *style.insets->start.is<CalcValue<PercentOr<Length>>>(), containingBlock.width);
        }
        if (not rightIsAuto) {
            resolvedOffset.x = -resolve(tree, box, *style.insets->end.is<CalcValue<PercentOr<Length>>>(), containingBlock.width);
        }
    }

    bool topIsAuto = style.insets->top.is<Keywords::Auto>();
    bool bottomIsAuto = style.insets->bottom.is<Keywords::Auto>();

    if (not topIsAuto and not bottomIsAuto) {
        resolvedOffset.y = resolve(tree, box, *style.insets->top.is<CalcValue<PercentOr<Length>>>(), containingBlock.height);
    } else {
        if (not topIsAuto) {
            resolvedOffset.y = resolve(tree, box, *style.insets->top.is<CalcValue<PercentOr<Length>>>(), containingBlock.height);
        }
        if (not bottomIsAuto) {
            resolvedOffset.y = -resolve(tree, box, *style.insets->bottom.is<CalcValue<PercentOr<Length>>>(), containingBlock.height);
        }
    }

    return resolvedOffset;
}

// https://www.w3.org/TR/css-position-3/#abspos-layout
export Tuple<Vec2Au, RectAu, UsedSpacings> layoutAbsolutePositioned(Tree& tree, Box& box, RectAu containingBlock, RectAu staticPositionRect) {
    auto const& style = *box.style;

    // Absolute positioning not only takes a box out of flow, but also lays it out
    // in its containing block (after the final size of the containing block has been determined)
    // according to the absolute positioning layout model:

    // 1. First, its inset-modified containing block is calculated, defining its available space.
    auto availableSpace = _computeInsetModifiedContainingBlock(tree, box, containingBlock, staticPositionRect);

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
    )
                     .unwrapOr(availableSpace.width);

    auto height = computeSpecifiedBorderBoxHeight(
                      tree, box, style.sizing->height, containingBlock.size(),
                      usedSpacings.padding.vertical() + usedSpacings.borders.vertical()
    )
                      .unwrapOr(availableSpace.height);

    yap("{}", usedSpacings);
    yap("Width {}, Height {}", width, height);

    // TODO
    // 3. Then, the value of any auto margins are calculated.

    // TODO
    // 4. Lastly, its margin box is aligned within the inset-modified containing block.

    return {{width, height}, availableSpace, usedSpacings};
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
