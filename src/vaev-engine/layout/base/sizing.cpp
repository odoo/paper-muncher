export module Vaev.Engine:layout.sizing;

import :layout.input;
import :layout.formating;
import :layout.layout;
import :layout.values;

namespace Vaev::Layout {

// MARK: Fit content size ------------------------------------------------------
// https://www.w3.org/TR/css-sizing-3/#fit-content-size
// NOTE: This is called the “shrink-to-fit” width in CSS2

export Vec2Au computeFitContentSize(Tree& tree, Box& box, AvailableSpace availableSpace) {
    auto minSize = computeIntrinsicContentSize(tree, box, IntrinsicSize::MIN_CONTENT);
    auto maxSize = computeIntrinsicContentSize(tree, box, IntrinsicSize::MAX_CONTENT);
    Vec2Au size = {INDEFINITE, INDEFINITE};

    // If the available space in a given axis is definite,
    // equal to clamp(min-content size, stretch-fit size, max-content size)
    // (i.e. max(min-content size, min(max-content size, stretch-fit size))).
    // When sizing under a min-content constraint, equal to the min-content size.
    // Otherwise, equal to the max-content size in that axis.
    if (availableSpace.x == Keywords::MIN_CONTENT) {
        size.x = minSize.width;
    } else if (availableSpace.x == Keywords::MAX_CONTENT) {
        size.x = maxSize.width;
    } else if (availableSpace.x != INDEFINITE) {
        size.x = clamp(availableSpace.x.unwrap<Au>(), minSize.width, maxSize.width);
    }

    if (availableSpace.y == Keywords::MIN_CONTENT) {
        size.y = minSize.height;
    } else if (availableSpace.y == Keywords::MAX_CONTENT) {
        size.y = maxSize.height;
    } else if (availableSpace.y != INDEFINITE) {
        size.y = clamp(availableSpace.y.unwrap<Au>(), minSize.height, maxSize.height);
    }

    return size;
}

export Au computeFitContentInlineSize(Tree& tree, Box& box, AvailableSpaceAxis availableSpace) {
    return computeFitContentSize(tree, box, {availableSpace, INDEFINITE}).x;
}

export Au computeFitContentBlockSize(Tree& tree, Box& box, AvailableSpaceAxis availableSpace) {
    return computeFitContentSize(tree, box, {INDEFINITE, availableSpace}).y;
}

// https://www.w3.org/TR/css-sizing-3/#preferred-size-properties
// FIXME: Values other than <length-percentage> are currently treated as 'auto'.
export Math::Vec2<Opt<Au>> resolvePreferredSize(Tree const& tree, Box const& box, Vec2Au containingBlock) {
    auto const& style = *box.style;

    Opt<Au> width = NONE;
    Opt<Au> height = NONE;

    if (auto calc = style.sizing->width.is<CalcValue<PercentOr<Length>>>()) {
        width = resolve(tree, box, *calc, containingBlock.width);
    }

    if (auto calc = style.sizing->height.is<CalcValue<PercentOr<Length>>>()) {
        height = resolve(tree, box, *calc, containingBlock.height);
    }

    return {width, height};
}

// https://www.w3.org/TR/CSS22/visudet.html#min-max-widths
// FIXME: Values other than <length-percentage> are currently treated as 'auto'.
export Vec2Au applyMinMaxSizeConstraints(Tree const& tree, Box const& box, Vec2Au tentative, Vec2Au containingBlock) {
    auto const& style = *box.style;

    Au minWidth = 0_au;
    Au minHeight = 0_au;

    Au maxWidth = Limits<Au>::MAX;
    Au maxHeight = Limits<Au>::MAX;

    if (auto calc = style.sizing->minWidth.is<CalcValue<PercentOr<Length>>>())
        minWidth = resolve(tree, box, *calc, containingBlock.width);
    if (auto calc = style.sizing->maxWidth.is<CalcValue<PercentOr<Length>>>())
        maxWidth = resolve(tree, box, *calc, containingBlock.width);
    if (auto calc = style.sizing->minHeight.is<CalcValue<PercentOr<Length>>>())
        minHeight = resolve(tree, box, *calc, containingBlock.height);
    if (auto calc = style.sizing->maxHeight.is<CalcValue<PercentOr<Length>>>())
        maxHeight = resolve(tree, box, *calc, containingBlock.height);

    maxWidth = max(maxWidth, minWidth);
    maxHeight = max(maxHeight, minHeight);

    auto w = tentative.width;
    auto h = tentative.height;

    // Fallback for degenerate tentative.
    if (w == 0_au or h == 0_au)
        return {clamp(w, minWidth, maxWidth), clamp(h, minHeight, maxHeight)};

    // NOTE: Constraint violation checks are done in the reverse order of the table in the spec
    //       because it needs to go from the most constrained case to the least constrained case.

    if (w > maxWidth and h < minHeight) {
        return {maxWidth, minHeight};
    }

    if (w < minWidth and h > maxHeight) {
        return {minWidth, maxHeight};
    }

    if (w < minWidth and h < minHeight) {
        if (minWidth / w > minHeight / h) {
            return {minWidth, min(maxHeight, minWidth * (h/w))};
        } else {
            return {min(maxWidth, minHeight * (w/h)), minHeight};
        }
    }

    if (w > maxWidth and h > maxHeight) {
        if (maxWidth / w > maxHeight / h) {
            return {max(minWidth, maxHeight * (w / h)), maxHeight};
        } else {
            return {maxWidth, max(minHeight, maxWidth * (h / w))};
        }
    }

    if (h < minHeight) {
        return {min(minHeight * (w / h), maxWidth), minHeight};
    }

    if (h > maxHeight) {
        return {max(maxHeight * (w / h), minWidth), maxHeight};
    }

    if (w < minWidth) {
        return {minWidth, min(minWidth * (h / w), maxHeight)};
    }

    if (w > maxWidth) {
        return {maxWidth, max(maxWidth * (h / w), minHeight)};
    }

    return {w, h};
}

// https://www.w3.org/TR/css-images-3/#natural-dimensions
export struct ObjectNaturalDimensions {
    Math::Vec2<Opt<Au>> size;
    Opt<f64> aspectRatio;
};

// https://www.w3.org/TR/css-images-3/#default-sizing
export Vec2Au resolveObjectDefaultSizing(ObjectNaturalDimensions naturalDimensions, Math::Vec2<Opt<Au>> specifiedSize) {
    auto resolvePartialSpecifiedSize = [](ObjectNaturalDimensions naturalDimensions, Math::Vec2<Opt<Au>> specifiedSize) -> Opt<Vec2Au> {
        // - If the specified size is a definite width and height, the concrete object size is given that width and height.
        if (specifiedSize.width and specifiedSize.height) {
            return Vec2Au{*specifiedSize.width, *specifiedSize.height};
        }

        // - If the specified size is only a width or height (but not both)
        //   then the concrete object size is given that specified width or height.
        //   The other dimension is calculated as follows:

        if (specifiedSize.width and not specifiedSize.height) {
            Au width = *specifiedSize.width;

            // 1. If the object has a natural aspect ratio,
            if (naturalDimensions.aspectRatio) {
                // the missing dimension of the concrete object size is calculated using that aspect ratio and the present dimension.
                return Vec2Au{width, width / *naturalDimensions.aspectRatio};
            }

            // 2. Otherwise, if the missing dimension is present in the object’s natural dimensions,
            if (naturalDimensions.size.height) {
                // the missing dimension is taken from the object’s natural dimensions.
                return Vec2Au{width, *naturalDimensions.size.height};
            }

            // 3. Otherwise, the missing dimension of the concrete object size is taken from the default object size.
            return Vec2Au{width, 150_au};
        }

        if (specifiedSize.height and not specifiedSize.width) {
            Au height = *specifiedSize.height;

            // 1. If the object has a natural aspect ratio,
            if (naturalDimensions.aspectRatio) {
                // the missing dimension of the concrete object size is calculated using that aspect ratio and the present dimension.
                return Vec2Au{height * *naturalDimensions.aspectRatio, height};
            }

            // 2. Otherwise, if the missing dimension is present in the object’s natural dimensions,
            if (naturalDimensions.size.width) {
                // the missing dimension is taken from the object’s natural dimensions.
                return Vec2Au{*naturalDimensions.size.width, height};
            }

            // 3. Otherwise, the missing dimension of the concrete object size is taken from the default object size.
            return Vec2Au{300_au, height};
        }

        return NONE;
    };

    if (auto const [size] = resolvePartialSpecifiedSize(naturalDimensions, specifiedSize))
        return size;

    // - If the specified size has no constraints:

    // 1. If the object has a natural height or width, its size is resolved as if its natural dimensions were given as the specified size.
    if (auto const [size] = resolvePartialSpecifiedSize(naturalDimensions, naturalDimensions.size))
        return size;

    // 2. Otherwise, its size is resolved as a contain constraint against the default object size.
    if (auto const [aspectRatio] = naturalDimensions.aspectRatio) {
        if (300_au / 150_au > aspectRatio) {
            return {150_au * aspectRatio, 150_au};
        } else {
            return {300_au, 300_au / aspectRatio};
        }
    }

    return {300_au, 150_au};
}

} // namespace Vaev::Layout
