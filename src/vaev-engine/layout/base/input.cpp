export module Vaev.Engine:layout.input;

import :layout.breaks;
import :layout.fragment;
import :layout.runningPosition;
import :values;

namespace Vaev::Layout {

// MARK: AvailableSpace --------------------------------------------------------
// https://www.w3.org/TR/css-sizing-3/#available

export struct MinContent {
    bool operator==(MinContent const&) const = default;
};

export constexpr MinContent MIN_CONTENT;

export struct MaxContent {
    bool operator==(MaxContent const&) const = default;
};

export constexpr MaxContent MAX_CONTENT;

export using AvailableSpaceAxis = Union<Au, MinContent, MaxContent>;

export bool isIntrinsic(AvailableSpaceAxis const& axis) {
    return axis.is<MinContent>() or axis.is<MaxContent>();
}

// The definite size in a slot, or zero for a sizing constraint.
// NOTE: Reproduces the old behavior where intrinsic layouts were given a
//       zero available space.
// FIXME: Callers should handle constraints instead of treating them as zero.
export Au definiteOrZero(AvailableSpaceAxis const& axis) {
    if (auto space = axis.is<Au>())
        return *space;
    return 0_au;
}

// The definite size in each axis, or zero where it is indefinite.
// FIXME: Callers should handle indefinite sizes instead of treating them as
//        zero, e.g. percentages resolving against an indefinite size should
//        behave as auto.
//        https://www.w3.org/TR/css-sizing-3/#percentage-sizing
export Vec2Au definiteOrZero(Math::Vec2<Opt<Au>> size) {
    return {size.x.unwrapOr(0_au), size.y.unwrapOr(0_au)};
}

// FIXME: Assumes horizontal-tb, inline is x and block is y.
export struct AvailableSpace {
    AvailableSpaceAxis inline_ = 0_au;
    AvailableSpaceAxis block = 0_au;

    AvailableSpace() = default;

    AvailableSpace(AvailableSpaceAxis inline_, AvailableSpaceAxis block)
        : inline_(inline_), block(block) {}

    AvailableSpace(Vec2Au v)
        : inline_(v.x), block(v.y) {}

    bool operator==(AvailableSpace const&) const = default;
};

// MARK: Input -----------------------------------------------------------------

struct UsedSpacings {
    InsetsAu padding{};
    InsetsAu borders{};
    InsetsAu margin{};

    void repr(Io::Emit& e) const {
        e("(used spacings paddings: {} borders: {} margin: {})",
          padding, borders, margin);
    }
};

export enum struct LayoutMode {
    MEASURE, //< Pure measurement.
    COMMIT,  //< Generate fragments.
};

export enum struct SizingMode {
    // https://www.w3.org/TR/css-sizing-3/#auto-box-sizes
    SIZE,
    // https://www.w3.org/TR/css-sizing-3/#contributions
    CONTRIBUTION,
};

export struct Input {
    LayoutMode mode = LayoutMode::MEASURE;
    UsedSpacings usedSpacings = {};
    Math::Vec2<Opt<Au>> knownSize = {};
    Vec2Au position = {};
    // https://www.w3.org/TR/css-sizing-3/#available
    AvailableSpace availableSpace = {MAX_CONTENT, MAX_CONTENT};
    // NONE means the containing block size is indefinite in that axis.
    Math::Vec2<Opt<Au>> containingBlock = {};
    MutCursor<RunningPositionMap> runningPosition = nullptr;
    usize pageNumber = 0;

    BreakpointTraverser breakpointTraverser = {};

    // To be used between table wrapper and table box
    Opt<Au> capmin = NONE;

    // TODO: instead of stringing this around, maybe change this (and check method of fragmentainer) to a
    // "availableSpaceInFragmentainer" parameter
    Au pendingVerticalSizes = {};

    Input withKnownSize(Math::Vec2<Opt<Au>> size) const {
        auto copy = *this;
        copy.knownSize = size;
        return copy;
    }

    Input withPosition(Vec2Au pos) const {
        auto copy = *this;
        copy.position = pos;
        return copy;
    }

    Input withAvailableSpace(AvailableSpace space) const {
        auto copy = *this;
        copy.availableSpace = space;
        return copy;
    }

    Input withContainingBlock(Math::Vec2<Opt<Au>> block) const {
        auto copy = *this;
        copy.containingBlock = block;
        return copy;
    }

    Input withBreakpointTraverser(BreakpointTraverser bpt) const {
        auto copy = *this;
        copy.breakpointTraverser = bpt;
        return copy;
    }

    Input addPendingVerticalSize(Au newPendingVerticalSize) const {
        auto copy = *this;
        copy.pendingVerticalSizes += newPendingVerticalSize;
        return copy;
    }

    Input withMode(LayoutMode mode) const {
        auto copy = *this;
        copy.mode = mode;
        return copy;
    }
};

} // namespace Vaev::Layout
