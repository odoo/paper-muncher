export module Vaev.Engine:layout.breaks;

import Karm.Core;
import Karm.Math;

import :values.length;
import :layout.box;
import :style;

using namespace Karm;

namespace Vaev::Layout {

// MARK: Fragmentainer ---------------------------------------------------------

// https://www.w3.org/TR/css-break-3/#fragmentainer
// https://www.w3.org/TR/css-break-3/#fragmentation-context
export struct Fragmentainer {
    Vec2Au _size;
    usize _monolithicCount = 0; //< How deep we are in a monolithic box

    Fragmentainer(Vec2Au currSize = Vec2Au::MAX) : _size(currSize) {}

    Vec2Au size() const { return _size; }

    bool isMonolithicBox() {
        return _monolithicCount != 0;
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
        return not hasInfiniteDimensions() and not isMonolithicBox();
    }

    bool acceptsFit(Au verticalPosition, Au verticalSize, Au pendingVerticalSizes) const {
        // TODO: consider apply this check only when in discovery mode
        return verticalPosition + verticalSize + pendingVerticalSizes <= _size.y;
    }

    Au leftVerticalSpace(Au verticalPosition, Au pendingVerticalSizes) {
        return _size.y - verticalPosition - pendingVerticalSizes;
    }
};

// MARK: Breakpoint ------------------------------------------------------------

export struct _Breakpoint {
    enum struct Type : u8 {
        BEFORE,
        INSIDE,
        LINE,
        _LEN,
    };

    enum struct Appeal : u8 {
        LAST_RESORT = 0,
        IGNORING_BREAK_AVOID = 1,
        IGNORING_ORPHANS_AND_WIDOWS = 2,
        PERFECT = 3,
        _LEN,
    };

    using enum Appeal;

    usize index; // Child index or line index.
    Opt<Karm::Box<_Breakpoint>> bestChildBreakpoint;
    Type type;
    Appeal appeal;
    bool isForced;

    _Breakpoint(Type type, usize index, Appeal appeal, bool isForced, Opt<Karm::Box<_Breakpoint>> const& bestChildBreakpoint)
        : index(index),
          bestChildBreakpoint(std::move(bestChildBreakpoint)),
          type(type),
          appeal(appeal),
          isForced(isForced) {}

    static _Breakpoint before(usize childIndex, Appeal appeal, bool isForced) {
        return _Breakpoint(Type::BEFORE, childIndex, appeal, isForced, NONE);
    }

    static _Breakpoint after(usize childIndex, Appeal appeal, bool isForced) {
        return _Breakpoint(Type::BEFORE, childIndex + 1, appeal, isForced, NONE);
    }

    static _Breakpoint inside(usize childIndex, Appeal appeal, bool isForced, Karm::Box<_Breakpoint>&& bestChildBreakpoint) {
        return _Breakpoint(Type::INSIDE, childIndex, appeal, isForced, std::move(bestChildBreakpoint));
    }

    static _Breakpoint line(usize lineIndex, bool isForced, Appeal appeal) {
        return _Breakpoint(Type::LINE, lineIndex, appeal, isForced, NONE);
    }

    static _Breakpoint lastResort() {
        return before(0, LAST_RESORT, false);
    }

    void overrideIfBetter(_Breakpoint&& other) {
        if (appeal <= other.appeal)
            *this = std::move(other);
    }

    void repr(Io::Emit& e) const {
        e("(breakpoint appeal= {}", appeal);
    }
};

export struct BreakpointReplayer {
    Opt<_Breakpoint const&> current;

    bool isActive() const {
        return current.has();
    }

    bool shouldRecurseInto(usize childIndex) {
        return current and
               current->type == _Breakpoint::Type::BEFORE and
               current->index == childIndex;
    }
};

export struct BreakpointTraverser {
    _Breakpoint const* current = nullptr;

    BreakpointTraverser() = default;
    BreakpointTraverser(_Breakpoint const* bp) : current(bp) {}

    bool isActive() const { return current != nullptr; }

    // Are we supposed to break before child[i] ?
    bool breakBefore(usize i) const {
        return current and
               current->type == _Breakpoint::Type::BEFORE and
               current->index == i;
    }

    // Should we recurse into child[i] ?
    bool recurseInto(usize i) const {
        return current and
               current->type == _Breakpoint::Type::INSIDE and
               current->index == i;
    }

    // Get the traverser for the inside of child[i]
    BreakpointTraverser traverseInside(usize i) const {
        if (recurseInto(i) and current->bestChildBreakpoint)
            return {current->bestChildBreakpoint};
        return {};
    }

    // Start index: skip children before the breakpoint
    usize startIndex() const {
        if (not current) return 0;
        return current->index - (current->type == _Breakpoint::Type::BEFORE ? 0 : 0);
    }
};

} // namespace Vaev::Layout
