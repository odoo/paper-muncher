export module Vaev.Engine:layout.breaks;

import Karm.Core;
import Karm.Math;

using namespace Karm;

namespace Vaev::Layout {

// MARK: Fragmentainer ---------------------------------------------------------

// https://www.w3.org/TR/css-break-3/#fragmentainer
// https://www.w3.org/TR/css-break-3/#fragmentation-context
export struct Fragmentainer {
    Vec2Au _size;
    bool _isDiscoveryMode = false; //< Are we looking for suitable breakpoints?
    usize _monolithicCount = 0;    //< How deep we are in a monolithic box

    Fragmentainer(Vec2Au currSize = Vec2Au::MAX) : _size(currSize) {}

    Vec2Au size() const { return _size; }

    void enterDiscovery() { _isDiscoveryMode = true; }

    void leaveDiscovery() { _isDiscoveryMode = false; }

    bool isDiscoveryMode() {
        return allowBreak() and _isDiscoveryMode;
    }

    bool isMonolithicBox() {
        return _monolithicCount == 0;
    }

    void enterMonolithicBox() {
        _monolithicCount++;
    }

    void leaveMonolithicBox() {
        _monolithicCount--;
    }

    bool hasInfiniteDimensions() {
        return _size == Vec2Au::MAX;
    }

    bool allowBreak() {
        return not hasInfiniteDimensions() and _monolithicCount == 0;
    }

    bool acceptsFit(Au verticalPosition, Au verticalSize, Au pendingVerticalSizes) {
        // TODO: consider apply this check only when in discovery mode
        return verticalPosition + verticalSize + pendingVerticalSizes <= _size.y;
    }

    Au leftVerticalSpace(Au verticalPosition, Au pendingVerticalSizes) {
        return _size.y - verticalPosition - pendingVerticalSizes;
    }
};

// MARK: Breakpoint ------------------------------------------------------------

// TODO: consider adding classification for breakpoints, what would make appeal computing easier and less error prone
export struct Breakpoint {
    enum struct Appeal : u8 {
        EMPTY = 0,
        OVERFLOW = 1,
        AVOID = 2,
        CLASS_B = 3,
        FORCED = 4,
        MAX = Limits<u8>::MAX
    };

    enum struct Advance {
        DONT, // keeping children
        WITH_CHILDREN,
        WITHOUT_CHILDREN
    };

    usize endIdx = 0;
    Appeal appeal = Appeal::EMPTY;
    Vec<Opt<Breakpoint>> children = {};
    Advance advance;

    static Breakpoint const START_OF_DOCUMENT;
    static Breakpoint const END_OF_DOCUMENT;

    static Breakpoint forced(usize endIdx) {
        return {
            .endIdx = endIdx,
            // since this is a FORCED break, it will have maximum appeal
            .appeal = Appeal::FORCED,
            .advance = Advance::WITHOUT_CHILDREN,
        };
    }

    static Breakpoint fromChild(Breakpoint&& childBreakpoint, usize endIdx, bool isAvoid) {
        Breakpoint b{
            .endIdx = endIdx,
            .appeal = childBreakpoint.appeal,
            .children = {std::move(childBreakpoint)},
            .advance = Advance::DONT,
        };

        if (isAvoid)
            b.appeal = Appeal::AVOID;

        return b;
    }

    static Breakpoint fromChildren(Vec<Opt<Breakpoint>> childrenBreakpoints, usize endIdx, bool isAvoid, bool advance) {
        Appeal appeal = Appeal::MAX;
        for (auto& breakpoint : childrenBreakpoints) {
            if (not breakpoint)
                continue;
            appeal = min(appeal, breakpoint.unwrap().appeal);
        }

        if (appeal == Appeal::MAX)
            panic("cannot build breakpoint from children when no children have a breakpoint");

        Breakpoint b{
            .endIdx = endIdx,
            .appeal = appeal,
            .children = {std::move(childrenBreakpoints)},
            .advance = advance ? Advance::WITH_CHILDREN : Advance::DONT
        };

        if (isAvoid)
            b.appeal = Appeal::AVOID;

        return b;
    }

    static Breakpoint classB(usize endIdx, bool isAvoid) {
        Breakpoint b{
            .endIdx = endIdx,
            .appeal = isAvoid ? Appeal::AVOID : Appeal::CLASS_B,
            .advance = Advance::WITHOUT_CHILDREN
        };

        return b;
    }

    static Breakpoint overflow() {
        // this is a placeholder breakpoint and should be overriden
        return {
            .endIdx = 0,
            .appeal = Appeal::OVERFLOW,
            .advance = Advance::DONT
        };
    }

    Breakpoint& withAppeal(Appeal a) {
        appeal = a;
        return *this;
    }

    void overrideIfBetter(Breakpoint&& other) {
        // in case of overflows, we need the earliest breakpoint possible
        if (appeal == Appeal::OVERFLOW and other.appeal == Appeal::OVERFLOW)
            return;

        if (appeal <= other.appeal)
            *this = std::move(other);
    }

    void repr(Io::Emit& e) const {
        e("(end: {} appeal: {} advance case: {}", endIdx, appeal, advance);
        if (children.len() == 0)
            e("; no child)");
        else
            e("; children : {})", children);
    }
};

Breakpoint const Breakpoint::START_OF_DOCUMENT = {
    .endIdx = 0,
    .advance = Advance::WITHOUT_CHILDREN
};

Breakpoint const Breakpoint::END_OF_DOCUMENT = {
    .endIdx = 1,
    .advance = Advance::WITHOUT_CHILDREN
};

export struct BreakpointTraverser {
    Cursor<Breakpoint> prevIteration, currIteration;

    BreakpointTraverser(
        Cursor<Breakpoint> prev = nullptr,
        Cursor<Breakpoint> curr = nullptr
    ) : prevIteration(prev), currIteration(curr) {}

    bool isDeactivated() {
        return prevIteration == nullptr and currIteration == nullptr;
    }

    Cursor<Breakpoint> traversePrev(usize i, usize j) {
        if (prevIteration and prevIteration->children.len() > 0 and
            (i + 1 == prevIteration->endIdx or
             (prevIteration->advance == Breakpoint::Advance::WITH_CHILDREN and i == prevIteration->endIdx))) {
            if (prevIteration->children[j])
                return &prevIteration->children[j].unwrap();
        }
        return nullptr;
    }

    Cursor<Breakpoint> traverseCurr(usize i, usize j) {
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
        return prevIteration->endIdx - (prevIteration->advance == Breakpoint::Advance::DONT);
    }

    Opt<usize> getEnd() {
        if (currIteration == nullptr)
            return NONE;
        return currIteration->endIdx;
    }
};

} // namespace Vaev::Layout
