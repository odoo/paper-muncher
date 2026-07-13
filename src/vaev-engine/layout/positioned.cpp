export module Vaev.Engine:layout.positioned;

import Karm.Math;

import :values;
import :layout.layout;
import :layout.values;

namespace Vaev::Layout {

// https://www.w3.org/TR/css-position-3/#resolving-insets
static RectAu _computeInsetModifiedContainingBlock(Tree const& tree, Box& box, RectAu containingBlock, RectAu staticPositionRect) {
    auto const& style = *box.style;

    auto relativeTo = style.position == Keywords::FIXED
                          ? tree.viewport.small
                          : containingBlock;

    InsetsAu resolvedInsets = {};

    using InsetLength = CalcValue<PercentOr<Length>>;

    bool startIsAuto = style.insets->start.is<Keywords::Auto>();
    bool endIsAuto = style.insets->end.is<Keywords::Auto>();

    // If only one inset property in a given axis is auto, it is set to zero. If both inset properties in a given axis are auto, then,
    // depending on the box’s self-alignment property in the relevant axis:
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
                resolvedInsets.end = containingBlock.width - (startDistance * 2);
            } else {
                //	otherwise, set the end-edge inset property to zero and the start-edge inset property
                //	to (containing block size - 2 × |end distance|).
                resolvedInsets.start = containingBlock.width - (endDistance * 2);
            }
        } else {
            // TODO
            logError("unsupported align on absolute positioned box: {}", style.aligns.justifySelf);
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

    // If only one inset property in a given axis is auto, it is set to zero. If both inset properties in a given axis are auto, then,
    // depending on the box’s self-alignment property in the relevant axis:
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
                resolvedInsets.bottom = containingBlock.height - (startDistance * 2);
            } else {
                //	otherwise, set the end-edge inset property to zero and the start-edge inset property
                //	to (containing block size - 2 × |end distance|).
                resolvedInsets.top = containingBlock.height - (endDistance * 2);
            }
        } else {
            // TODO
            logError("unsupported align on absolute positioned box");
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

bool _isAbsoluteAndFixedPositioningContainingBlock(Style::ComputedValues const& style) {
    // https://www.w3.org/TR/css-transforms-1/#transform-rendering
    // For elements whose layout is governed by the CSS box model, any value other than none for the transform property
    // also causes the element to establish a containing block for all descendants. Its padding box will be used to layout
    // for all of its absolute-position descendants, fixed-position descendants, and descendant fixed background attachments.
    if (style.transform->transform != Keywords::NONE) {
        return true;
    }

    // TODO: Implement interactions with filter-effects-* and css-transforms-2 when those will be implemented.
    return false;
}

// https://www.w3.org/TR/css-position-3/#absolute-positioning-containing-block
export bool isAbsolutePositioningContainingBlock(Style::ComputedValues const& style) {
    // NOTE: There is a helpful but non-normative article on MDN
    // https://developer.mozilla.org/en-US/docs/Web/CSS/Guides/Display/Containing_block#identifying_the_containing_block

    if (style.position != Keywords::STATIC) {
        return true;
    }

    if (_isAbsoluteAndFixedPositioningContainingBlock(style)) {
        return true;
    }

    return false;
}

// https://www.w3.org/TR/css-position-3/#fixed-positioning-containing-block
export bool isFixedPositioningContainingBlock(Style::ComputedValues const& style) {
    // NOTE: There is a helpful but non-normative article on MDN
    // https://developer.mozilla.org/en-US/docs/Web/CSS/Guides/Display/Containing_block#identifying_the_containing_block

    return _isAbsoluteAndFixedPositioningContainingBlock(style);
}

// https://www.w3.org/TR/css-position-3/#abspos-layout
export Output layoutAbsolutePositioned(Tree& tree, Box& box, RectAu containingBlock, RectAu staticPositionRect, usize pageNumber) {
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
        // FIXME: Auto margins should not always resolve to zero (see step 3).
        .margin = computeMargins(tree, box, containingBlock.size())
    };

    auto width = computeSpecifiedBorderBoxWidth(
                     tree, box, style.sizing->width, containingBlock.size(),
                     usedSpacings.padding.horizontal() + usedSpacings.borders.horizontal()
    );

    auto height = computeSpecifiedBorderBoxHeight(
                      tree, box, style.sizing->height, containingBlock.size(),
                      usedSpacings.padding.vertical() + usedSpacings.borders.vertical()
    );

    // TODO
    // 3. Then, the value of any auto margins are calculated.

    // TODO
    // 4. Lastly, its margin box is aligned within the inset-modified containing block.

    Input childInput = {
        .generateFragment = true,
        .usedSpacings = usedSpacings,
        .knownSize = {width, height},
        .position = availableSpace.topStart() + usedSpacings.margin.topStart(),
        .availableSpace = availableSpace.size(),
        .containingBlock = containingBlock.size(),
        .pageNumber = pageNumber,
    };

    return layoutBorderBox(tree, box, childInput);
}

// https://www.w3.org/TR/css-position-3/#relpos-insets
Au _negotiateInsetsForRelativePositioning(Tree& tree, Box const& box, Size const& start, Size const& end, Au relativeTo) {
    bool startIsAuto = start.is<Keywords::Auto>();
    bool endIsAuto = end.is<Keywords::Auto>();

    // - If opposing inset properties in an axis both compute to auto (their initial values), their used values are zero
    //   (i.e., the boxes stay in their original position in that axis).
    if (startIsAuto and endIsAuto) {
        return 0_au;
    }

    // - If only one is auto, its used value becomes the negation of the other, and the box is shifted by the specified amount.
    if (not startIsAuto and endIsAuto) {
        return resolve(tree, box, *start.is<CalcValue<PercentOr<Length>>>(), relativeTo);
    }
    if (startIsAuto and not endIsAuto) {
        return -resolve(tree, box, *end.is<CalcValue<PercentOr<Length>>>(), relativeTo);
    }

    // - If neither is auto, the position is over-constrained; (with respect to the writing mode of its containing block)
    //   the computed end side value is ignored, and its used value becomes the negation of the start side.
    // FIXME: Honor writing mode.
    return resolve(tree, box, *start.is<CalcValue<PercentOr<Length>>>(), relativeTo);
}

// https://www.w3.org/TR/css-position-3/#relpos-insets
export Vec2Au relativePositionOffset(Tree& tree, Box const& box, RectAu containingBlock) {
    auto const& s = *box.style;

    return Vec2Au {
        _negotiateInsetsForRelativePositioning(tree, box, s.insets->start, s.insets->end, containingBlock.width),
        _negotiateInsetsForRelativePositioning(tree, box, s.insets->top, s.insets->bottom, containingBlock.height),
    };
}

export void lookForRunningPosition(Input& input, Box& box) {
    if (not input.runningPosition)
        return;

    if (box.isRunningPositionedBox()) {
        auto& runningMap = input.runningPosition.peek();
        runningMap.add(input.pageNumber, box);
    }
}

} // namespace Vaev::Layout
