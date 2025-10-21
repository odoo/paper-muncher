export module Vaev.Engine:layout.input;

import :layout.breaks;
import :layout.fragment;
import :layout.runningPosition;

namespace Vaev::Layout {

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

export struct Input {
    /// Parent fragment where the layout will be attached.
    MutCursor<Frag> fragment = nullptr;
    IntrinsicSize intrinsic = IntrinsicSize::AUTO;
    Math::Vec2<Opt<Au>> knownSize = {};
    Vec2Au position = {};
    Vec2Au availableSpace = {};
    Vec2Au containingBlock = {};
    MutCursor<Runnings> runningPosition = nullptr;
    usize pageIndex = 0;

    BreakpointTraverser breakpointTraverser = {};

    // To be used between table wrapper and table box
    Opt<Au> capmin = NONE;

    // TODO: instead of stringing this around, maybe change this (and check method of fragmentainer) to a
    // "availableSpaceInFragmentainer" parameter
    Au pendingVerticalSizes = {};

    Input withFragment(MutCursor<Frag> f) const {
        auto copy = *this;
        copy.fragment = f;
        return copy;
    }

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
