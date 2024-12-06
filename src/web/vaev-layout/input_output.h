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

    Vec<Opt<Breakpoint>> children = {};

    enum struct ADVANCE_CASE {
        NOT_ADVANCE, // keeping children
        ADVANCE_WITH_CHILDREN,
        ADVANCE_WITHOUT_CHILDREN
    } advanceCase;

    void overrideIfBetter(Breakpoint &&BPWithMoreContent) {
        if (appeal == 0 and BPWithMoreContent.appeal == 0)
            return;

        if (appeal <= BPWithMoreContent.appeal)
            *this = std::move(BPWithMoreContent);
    }

    void repr(Io::Emit &e) const {
        e("(end: {} appeal: {} advance case: {}", endIdx, appeal, advanceCase);
        if (children.len() == 0)
            e("; no child)");
        else
            e("; children : {})", children);
    }

    static Breakpoint buildForced(usize endIdx) {
        return Breakpoint{
            .endIdx = endIdx,
            // since this is a FORCED break, it will have maximum appeal
            .appeal = Limits<usize>::MAX,
            .advanceCase = ADVANCE_CASE::ADVANCE_WITHOUT_CHILDREN,
        };
    }

    // NOTE: a bit inconsistent with the rest of the API
    void applyAvoid() {
        appeal = min(appeal, AVOID_APPEAL);
    }

    static Breakpoint buildFromChild(Breakpoint &&childBreakpoint, usize endIdx, bool isAvoid) {
        Breakpoint b{
            .endIdx = endIdx,
            .appeal = childBreakpoint.appeal,
            .children = {std::move(childBreakpoint)},
            .advanceCase = ADVANCE_CASE::NOT_ADVANCE,
        };

        if (isAvoid)
            b.appeal = min(b.appeal, AVOID_APPEAL);

        return b;
    }

    static Breakpoint buildFromChildren(Vec<Opt<Breakpoint>> childrenBreakpoints, usize endIdx, bool isAvoid, bool advance) {
        usize appeal = Limits<usize>::MAX;
        for (auto &breakpoint : childrenBreakpoints) {
            if (not breakpoint)
                continue;
            appeal = min(appeal, breakpoint.unwrap().appeal);
        }

        if (appeal == Limits<usize>::MAX)
            panic("");

        Breakpoint b{
            .endIdx = endIdx,
            .appeal = appeal,
            .children = {std::move(childrenBreakpoints)},
            .advanceCase = advance ? ADVANCE_CASE::ADVANCE_WITH_CHILDREN : ADVANCE_CASE::NOT_ADVANCE
        };

        if (isAvoid)
            b.appeal = min(b.appeal, AVOID_APPEAL);

        return b;
    }

    static Breakpoint buildClassB(usize endIdx, bool isAvoid) {
        Breakpoint b{
            .endIdx = endIdx,
            .appeal = isAvoid ? AVOID_APPEAL : CLASS_B_APPEAL,
            .advanceCase = ADVANCE_CASE::ADVANCE_WITHOUT_CHILDREN
        };

        return b;
    }

    static Breakpoint buildOverflow() {
        // this is a placeholder breakpoint and should be overriden
        return {
            .endIdx = 0,
            .appeal = 0,
            .advanceCase = ADVANCE_CASE::NOT_ADVANCE
        };
    }
};

struct BreakpointTraverser {
    MutCursor<Breakpoint> prevIteration, currIteration;

    BreakpointTraverser(
        MutCursor<Breakpoint> prev = nullptr,
        MutCursor<Breakpoint> curr = nullptr
    ) : prevIteration(prev), currIteration(curr) {}

    bool isDeactivated() {
        return prevIteration == nullptr and currIteration == nullptr;
    }

    MutCursor<Breakpoint> traversePrev(usize i, usize j) {
        if (prevIteration and prevIteration->children.len() > 0 and
            (i + 1 == prevIteration->endIdx or
             (prevIteration->advanceCase == Breakpoint::ADVANCE_CASE::ADVANCE_WITH_CHILDREN and i == prevIteration->endIdx)
            )) {
            if (prevIteration->children[j])
                return &prevIteration->children[j].unwrap();
        }
        return nullptr;
    }

    MutCursor<Breakpoint> traverseCurr(usize i, usize j) {
        if (currIteration and currIteration->children.len() > 0 and i + 1 == currIteration->endIdx) {
            if (currIteration->children[j])
                return &currIteration->children[j].unwrap();
        }
        return nullptr;
    }

    BreakpointTraverser traverseInsideUsingIthChildToJthParallelFlow(usize i, usize j) {
        BreakpointTraverser deeperBPT;
        deeperBPT.prevIteration = traversePrev(i, j);
        deeperBPT.currIteration = traverseCurr(i, j);
        return deeperBPT;
    }

    BreakpointTraverser traverseInsideUsingIthChild(usize i) {
        return traverseInsideUsingIthChildToJthParallelFlow(i, 0);
    }

    Opt<usize> getStart() {
        if (prevIteration == nullptr)
            return NONE;
        return prevIteration->endIdx - (prevIteration->advanceCase == Breakpoint::ADVANCE_CASE::NOT_ADVANCE);
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

    Input addPendingVerticalSize(Px newPendingVerticalSize) const {
        auto copy = *this;
        copy.pendingVerticalSizes += newPendingVerticalSize;
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
