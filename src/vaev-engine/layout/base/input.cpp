export module Vaev.Engine:layout.input;

import :layout.breaks;
import :layout.fragment;
import :layout.runningPosition;

namespace Vaev::Layout {

// MARK: AvailableSpace --------------------------------------------------------
// https://www.w3.org/TR/css-sizing-3/#available

using AvailableSpaceAxis = Union<Au, Keywords::MinContent, Keywords::MaxContent>;

struct AvailableSpace {
    AvailableSpaceAxis x;
    AvailableSpaceAxis y;
};

// MARK: Input -----------------------------------------------------------------

export enum struct IntrinsicSize {
    AUTO,
    MIN_CONTENT,
    MAX_CONTENT,
    STRETCH_TO_FIT,
};

export bool isMinMaxIntrinsicSize(IntrinsicSize intrinsic) {
    return intrinsic == IntrinsicSize::MIN_CONTENT or
           intrinsic == IntrinsicSize::MAX_CONTENT;
}

struct UsedSpacings {
    InsetsAu padding{};
    InsetsAu borders{};
    InsetsAu margin{};

    void repr(Io::Emit& e) const {
        e("(used spacings paddings: {} borders: {} margin: {})",
          padding, borders, margin);
    }
};

export struct Input {
    /// Parent fragment where the layout will be attached.
    bool generateFragment = false;
    UsedSpacings usedSpacings = {};
    IntrinsicSize intrinsic = IntrinsicSize::AUTO;
    Math::Vec2<Opt<Au>> knownSize = {};
    Vec2Au position = {};
    Vec2Au availableSpace = {};
    Vec2Au containingBlock = {};
    MutCursor<RunningPositionMap> runningPosition = nullptr;
    usize pageNumber = 0;

    BreakpointTraverser breakpointTraverser = {};

    // To be used between table wrapper and table box
    Opt<Au> capmin = NONE;

    // TODO: instead of stringing this around, maybe change this (and check method of fragmentainer) to a
    // "availableSpaceInFragmentainer" parameter
    Au pendingVerticalSizes = {};

    Input withIntrinsic(IntrinsicSize i) const {
        auto copy = *this;
        copy.intrinsic = i;
        return copy;
    }

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

    Input withAvailableSpace(Vec2Au space) const {
        auto copy = *this;
        copy.availableSpace = space;
        return copy;
    }

    Input withContainingBlock(Vec2Au block) const {
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
};

} // namespace Vaev::Layout
