#pragma once

#include "base.h"
#include "frag.h"

namespace Vaev::Layout {

// NOTE: all these comments might be erased once we are secure on the strucutre and have proper documentation

// TODO: consider adding classification for breakpoints, what would make appeal computing easier and less error prone
struct Breakpoint {
    static usize const AVOID_APPEAL = 1;
    static usize const CLASS_B_APPEAL = 2;

    usize endIdx = 0;

    // appeal = 0: an overflow occured; we want the earliest breakpoint possible
    // appeal > 0: no overflow occured: we want the latest breakpoint possible with biggest appeal
    // appeal = 1: breakpoints that have an avoid related to it
    // appeal = 2: breakpoints without avoids
    // this ranking system should be extended to more values (3 values is prolly a simplification) and the value
    // attribution could should be encapsulated in this class, instead of exposed to code
    usize appeal = 0;

    Vec<Breakpoint> child = {};

    void overrideIfBetter(Breakpoint &&BPWithMoreContent) {
        if (appeal == 0 and BPWithMoreContent.appeal == 0)
            return;

        if (appeal <= BPWithMoreContent.appeal)
            *this = std::move(BPWithMoreContent);
    }

    void repr(Io::Emit &e) const {
        e("(end: {} appeal: {}", endIdx, appeal);
        if (child.len() == 0)
            e("; no child)");
        else
            e("; child : {})", child[0]);
    }

    static Breakpoint buildForced(usize endIdx) {
        return Breakpoint{
            .endIdx = endIdx,
            // since this is a FORCED break, it will have maximum appeal
            .appeal = Limits<usize>::MAX
        };
    }

    static Breakpoint buildFromChild(Breakpoint &&childBreakpoint, usize endIdx, bool isAvoid) {
        Breakpoint b{
            .endIdx = endIdx,
            .appeal = childBreakpoint.appeal,
            .child = {std::move(childBreakpoint)}
        };

        if (isAvoid)
            b.appeal = min(b.appeal, AVOID_APPEAL);

        return b;
    }

    static Breakpoint buildClassB(usize endIdx, bool isAvoid) {
        Breakpoint b{
            .endIdx = endIdx,
            .appeal = isAvoid ? AVOID_APPEAL : CLASS_B_APPEAL
        };

        return b;
    }

    static Breakpoint buildOverflow() {
        // this is a placeholder breakpoint and should be overriden
        return {
            .endIdx = 0,
            .appeal = 0
        };
    }
};

struct BreakpointTraverser {
    MutCursor<Breakpoint> prevIteration, currIteration;

    BreakpointTraverser(
        MutCursor<Breakpoint> prev = nullptr,
        MutCursor<Breakpoint> curr = nullptr
    ) : prevIteration(prev), currIteration(curr) {}

    BreakpointTraverser traverseInsideUsingIthChild(usize i) {
        BreakpointTraverser deeperBPT;
        if (prevIteration and prevIteration->child.len() > 0 and i + 1 == prevIteration->endIdx) {
            deeperBPT.prevIteration = &prevIteration->child[0];
        }

        if (currIteration and currIteration->child.len() > 0 and i + 1 == currIteration->endIdx) {
            deeperBPT.currIteration = &currIteration->child[0];
        }

        return deeperBPT;
    }

    Opt<usize> getStart() {
        if (prevIteration == nullptr)
            return NONE;
        return prevIteration->endIdx - (prevIteration->child.len() ? 1 : 0);
    }

    Opt<usize> getEnd() {
        if (currIteration == nullptr)
            return NONE;
        return currIteration->endIdx;
    }
};

/// Input to the layout algorithm.
struct Input {
    /// Parent fragment where the layout will be attached.
    MutCursor<Frag> fragment = nullptr;
    IntrinsicSize intrinsic = IntrinsicSize::AUTO;
    Math::Vec2<Opt<Px>> knownSize = {};
    Vec2Px position = {};
    Vec2Px availableSpace = {};
    Vec2Px containingBlock = {};

    BreakpointTraverser breakpointTraverser = {};

    // To be used between table wrapper and table box
    Opt<Px> capmin = NONE;

    // TODO: instead of stringing this around, maybe change this (and check method of fragmentainer) to a
    // "availableSpaceInFragmentainer" parameter
    Px pendingVerticalSizes = {};

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

    Input withKnownSize(Math::Vec2<Opt<Px>> size) const {
        auto copy = *this;
        copy.knownSize = size;
        return copy;
    }

    Input withPosition(Vec2Px pos) const {
        auto copy = *this;
        copy.position = pos;
        return copy;
    }

    Input withAvailableSpace(Vec2Px space) const {
        auto copy = *this;
        copy.availableSpace = space;
        return copy;
    }

    Input withContainingBlock(Vec2Px block) const {
        auto copy = *this;
        copy.containingBlock = block;
        return copy;
    }

    Input withBreakpointTraverser(BreakpointTraverser bpt) const {
        auto copy = *this;
        copy.breakpointTraverser = bpt;
        return copy;
    }
};

struct Output {
    // size of subtree maximizing displayed content while respecting
    // - endchild constraint or
    // - not overflowing fragmentainer or
    // - forced break
    Vec2Px size;

    // was the box subtree laid out until the end?
    // - discovery mode: until the very end of the box
    // - non discovery mode: all subtrees until endChildren-1 were completly laid out
    // useful for:
    // - discovery mode: knowing if a child was complete so we can break after it
    //      (if not fully laid out, we need to stop the block formatting context)
    // - non-discovery mode: knowing if we can finish rendering
    bool completelyLaidOut;

    // only to be used in discovery mode
    Opt<Breakpoint> breakpoint = NONE;

    static Output fromSize(Vec2Px size) {
        return {
            .size = size,
            .completelyLaidOut = true
        };
    }

    Px width() const {
        return size.x;
    }

    Px height() const {
        return size.y;
    }
};

} // namespace Vaev::Layout
