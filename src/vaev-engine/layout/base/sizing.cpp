export module Vaev.Engine:layout.sizing;

import :layout.input;
import :layout.formating;
import :layout.layout;

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

} // namespace Vaev::Layout
