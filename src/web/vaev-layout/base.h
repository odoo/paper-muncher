#pragma once

#include <karm-image/picture.h>
#include <karm-text/prose.h>
#include <vaev-base/length.h>
#include <vaev-base/resolution.h>
#include <vaev-base/writing.h>
#include <vaev-style/computer.h>

namespace Vaev::Layout {

// MARK: Fragmentainer ---------------------------------------------------------

// https://www.w3.org/TR/css-break-3/#fragmentainer
// https://www.w3.org/TR/css-break-3/#fragmentation-context
struct Fragmentainer {
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
struct Breakpoint {
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
             (prevIteration->advance == Breakpoint::Advance::WITH_CHILDREN and i == prevIteration->endIdx)
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
        return prevIteration->endIdx - (prevIteration->advance == Breakpoint::Advance::DONT);
    }

    Opt<usize> getEnd() {
        if (currIteration == nullptr)
            return NONE;
        return currIteration->endIdx;
    }
};

// MARK: Box -------------------------------------------------------------------

struct FormatingContext;
struct Box;

using Content = Union<
    None,
    Vec<Box>,
    Rc<Text::Prose>,
    Karm::Image::Picture>;

struct Attrs {
    usize span = 1;
    usize rowSpan = 1;
    usize colSpan = 1;

    void repr(Io::Emit& e) const {
        e("(attrs span: {} rowSpan: {} colSpan: {})", span, rowSpan, colSpan);
    }
};

struct Box : public Meta::NoCopy {
    Rc<Style::Computed> style;
    Rc<Text::Fontface> fontFace;
    Content content = NONE;
    Attrs attrs;
    Opt<Rc<FormatingContext>> formatingContext = NONE;

    Box(Rc<Style::Computed> style, Rc<Karm::Text::Fontface> fontFace);

    Box(Rc<Style::Computed> style, Rc<Karm::Text::Fontface> fontFace, Content content);

    Slice<Box> children() const;

    MutSlice<Box> children();

    void add(Box&& box);

    void repr(Io::Emit& e) const;
};

struct Viewport {
    Resolution dpi = Resolution::fromDpi(96);
    // https://drafts.csswg.org/css-values/#small-viewport-size
    RectAu small;
    // https://drafts.csswg.org/css-values/#large-viewport-size
    RectAu large = small;
    // https://drafts.csswg.org/css-values/#dynamic-viewport-size
    RectAu dynamic = small;
};

struct Tree {
    Box root;
    Viewport viewport = {};
    Fragmentainer fc = {};
};

// MARK: Fragment --------------------------------------------------------------

struct Metrics {
    InsetsAu padding{};
    InsetsAu borders{};
    Vec2Au position; //< Position relative to the content box of the containing block
    Vec2Au borderSize;
    InsetsAu margin{};
    RadiiAu radii{};

    void repr(Io::Emit& e) const {
        e("(layout paddings: {} borders: {} position: {} borderSize: {} margin: {} radii: {})",
          padding, borders, position, borderSize, margin, radii);
    }

    RectAu borderBox() const {
        return RectAu{position, borderSize};
    }

    RectAu paddingBox() const {
        return borderBox().shrink(borders);
    }

    RectAu contentBox() const {
        return paddingBox().shrink(padding);
    }

    RectAu marginBox() const {
        return borderBox().grow(margin);
    }
};

struct Frag {
    MutCursor<Box> box;
    Metrics metrics;
    Vec<Frag> children;

    Frag(MutCursor<Box> box) : box{std::move(box)} {}

    Frag() : box{nullptr} {}

    Style::Computed const& style() const {
        return *box->style;
    }

    /// Offset the position of this fragment and its subtree.
    void offset(Vec2Au d) {
        metrics.position = metrics.position + d;
        for (auto& c : children)
            c.offset(d);
    }

    /// Add a child fragment.
    void add(Frag&& frag) {
        children.pushBack(std::move(frag));
    }
};

// MARK: Input & Output --------------------------------------------------------

enum struct IntrinsicSize {
    AUTO,
    MIN_CONTENT,
    MAX_CONTENT,
    STRETCH_TO_FIT,
};

static inline bool isMinMaxIntrinsicSize(IntrinsicSize intrinsic) {
    return intrinsic == IntrinsicSize::MIN_CONTENT or
           intrinsic == IntrinsicSize::MAX_CONTENT;
}

struct Input {
    /// Parent fragment where the layout will be attached.
    MutCursor<Frag> fragment = nullptr;
    IntrinsicSize intrinsic = IntrinsicSize::AUTO;

    InsetsAu computedBorders{}, computedPadding{};

    Math::Vec2<Opt<Au>> knownBorderBoxSize = {};
    Math::Vec2<Opt<Au>> knownContentBoxSize = {};
    Vec2Au borderBoxPosition = {};
    Vec2Au borderBoxAvailableSpace = {};
    Vec2Au containingBlock = {};

    BreakpointTraverser breakpointTraverser = {};

    // To be used between table wrapper and table box
    Opt<Au> capmin = NONE;

    // TODO: instead of stringing this around, maybe change this (and check method of fragmentainer) to a
    // "availableSpaceInFragmentainer" parameter
    Au borderBoxPendingVerticalSizes = {};

    Math::Vec2<Opt<Au>> contentBoxSize() const {
        Math::Vec2<Opt<Au>> contentBoxSize;

        if (not knownContentBoxSize.width) {
            contentBoxSize.width = knownBorderBoxSize.width.map([=, this](auto s) {
                return max(0_au, s - computedPadding.horizontal() - computedBorders.horizontal());
            });
        } else
            contentBoxSize.width = knownContentBoxSize.width;

        if (not knownContentBoxSize.height) {
            contentBoxSize.height = knownBorderBoxSize.height.map([&](auto s) {
                return max(0_au, s - computedPadding.vertical() - computedBorders.vertical());
            });
        } else
            contentBoxSize.height = knownContentBoxSize.height;

        return contentBoxSize;
    }

    Vec2Au contentBoxPosition() const {
        return borderBoxPosition + computedBorders.topStart() + computedPadding.topStart();
    }

    Vec2Au contentBoxAvailableSpace() const {
        return (borderBoxAvailableSpace - computedPadding.all() - computedBorders.all()).map([](auto x) {
            return max(0_au, x);
        });
    }

    Au contentBoxPendingVerticalSizes() const {
        return borderBoxPendingVerticalSizes - computedPadding.bottom - computedBorders.bottom;
    }

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

    Input withKnownBorderBoxSize(Math::Vec2<Opt<Au>> size) const {
        auto copy = *this;
        copy.knownBorderBoxSize = size;
        return copy;
    }

    Input withBorderBoxPosition(Vec2Au pos) const {
        auto copy = *this;
        copy.borderBoxPosition = pos;
        return copy;
    }

    Input withBorderBoxAvailableSpace(Vec2Au space) const {
        auto copy = *this;
        copy.borderBoxAvailableSpace = space;
        return copy;
    }

    Input withBorderBoxContainingBlock(Vec2Au block) const {
        auto copy = *this;
        copy.containingBlock = block;
        return copy;
    }

    Input withBreakpointTraverser(BreakpointTraverser bpt) const {
        auto copy = *this;
        copy.breakpointTraverser = bpt;
        return copy;
    }

    Input addBorderBoxPendingVerticalSize(Au newPendingVerticalSize) const {
        auto copy = *this;
        copy.borderBoxPendingVerticalSizes += newPendingVerticalSize;
        return copy;
    }
};

struct Output {
    // size of subtree maximizing displayed content while respecting
    // - endchild constraint or
    // - not overflowing fragmentainer or
    // - forced break
    Vec2Au size;

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

    static Output fromSize(Vec2Au size) {
        return {
            .size = size,
            .completelyLaidOut = true
        };
    }

    Au width() const {
        return size.x;
    }

    Au height() const {
        return size.y;
    }
};

// MARK: Formating Context -----------------------------------------------------

struct FormatingContext {
    virtual ~FormatingContext() = default;

    virtual void build(Tree&, Box&) {};
    virtual Output run(Tree& tree, Box& box, Input input, usize startAt, Opt<usize> stopAt) = 0;
};

} // namespace Vaev::Layout
