module;

#include <karm-image/picture.h>
#include <karm-text/font.h>
#include <karm-text/prose.h>
#include <vaev-style/computer.h>

export module Vaev.Layout:base;

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

export struct BreakpointTraverser {
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

export struct FormatingContext;
export struct Box;

export struct InlineBox {
    /* NOTE:
    This is a sketch implementation of the data model for InlineBox. We should be able to:
        -   add different inline elements to it, from different types (Prose, Image, inline-block)
        -   retrieve the added data to be displayed in the same Inline Formatting Context (break lines and display
            into line boxes)
        -   respect different styling for the same line (font, fontsize, color, etc)
    */
    Rc<Text::Prose> prose;
    Vec<::Box<Box>> atomicBoxes;

    InlineBox(Text::ProseStyle style) : prose(makeRc<Text::Prose>(style)) {}

    InlineBox(Rc<Text::Prose> prose) : prose(prose) {}

    void startInlineBox(Text::ProseStyle proseStyle) {
        // FIXME: ugly workaround while we dont fix the Prose data structure
        prose->pushSpan();
        if (proseStyle.color)
            prose->spanColor(proseStyle.color.unwrap());
    }

    void endInlineBox() {
        prose->popSpan();
    }

    void add(Box&& b);

    bool active() {
        return prose->_runes.len() or atomicBoxes.len();
    }

    void repr(Io::Emit& e) const {
        e("(inline box {}", prose->_runes);
        e.indentNewline();
        for (auto& c : atomicBoxes) {
            e("{}", c);
            e.newline();
        }
        e.deindent();
        e(")");
    }

    static InlineBox fromInterruptedInlineBox(InlineBox const& inlineBox) {
        auto oldProse = inlineBox.prose;

        auto newInlineBox = InlineBox{oldProse->_style};
        newInlineBox.prose->overrideSpanStackWith(*oldProse);

        return newInlineBox;
    }
};

export using Content = Union<
    None,
    Vec<Box>,
    InlineBox,
    Karm::Image::Picture>;

export struct Attrs {
    usize span = 1;
    usize rowSpan = 1;
    usize colSpan = 1;

    void repr(Io::Emit& e) const {
        e("(attrs span: {} rowSpan: {} colSpan: {})", span, rowSpan, colSpan);
    }
};

struct Box : Meta::NoCopy {
    Rc<Style::ComputedStyle> style;
    Rc<Text::Fontface> fontFace;
    Content content = NONE;
    Attrs attrs;
    Opt<Rc<FormatingContext>> formatingContext = NONE;
    Gc::Ptr<Dom::Element> origin;

    Box(Rc<Style::ComputedStyle> style, Rc<Text::Fontface> font, Gc::Ptr<Dom::Element> og)
        : style{std::move(style)}, fontFace{font}, origin{og} {}

    Box(Rc<Style::ComputedStyle> style, Rc<Text::Fontface> font, Content content, Gc::Ptr<Dom::Element> og)
        : style{std::move(style)}, fontFace{font}, content{std::move(content)}, origin{og} {}

    Slice<Box> children() const {
        if (auto children = content.is<Vec<Box>>())
            return *children;
        return {};
    }

    MutSlice<Box> children() {
        if (auto children = content.is<Vec<Box>>()) {
            return *children;
        }
        return {};
    }

    void add(Box&& box) {
        if (content.is<None>())
            content = Vec<Box>{};

        if (auto children = content.is<Vec<Box>>()) {
            children->pushBack(std::move(box));
        }
    }

    void repr(Io::Emit& e) const {
        if (children()) {
            e("(box {} {} {}", attrs, style->display, style->position);
            e.indentNewline();
            for (auto& c : children()) {
                c.repr(e);
                e.newline();
            }
            e.deindent();
            e(")");
        } else if (content.is<InlineBox>()) {
            e("(box {} {} {}", attrs, style->display, style->position);
            e.indentNewline();
            e("{}", content.unwrap<InlineBox>());
            e.deindent();
            e(")");
        } else {
            e("(box {} {} {})", attrs, style->display, style->position);
        }
    }
};

void InlineBox::add(Box&& b) {
    prose->append(Text::Prose::StrutCell{atomicBoxes.len()});
    atomicBoxes.pushBack(makeBox<Box>(std::move(b)));
}

export struct Viewport {
    Resolution dpi = Resolution::fromDpi(96);
    // https://drafts.csswg.org/css-values/#small-viewport-size
    RectAu small;
    // https://drafts.csswg.org/css-values/#large-viewport-size
    RectAu large = small;
    // https://drafts.csswg.org/css-values/#dynamic-viewport-size
    RectAu dynamic = small;
};

export struct Tree {
    Box root;
    Viewport viewport = {};
    Fragmentainer fc = {};
};

// MARK: Fragment --------------------------------------------------------------

export struct Metrics {
    InsetsAu padding{};
    InsetsAu borders{};
    Au outlineOffset{};
    Au outlineWidth{};
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

export struct Frag {
    MutCursor<Box> box;
    Metrics metrics;
    Vec<Frag> children;

    Frag(MutCursor<Box> box) : box{std::move(box)} {}

    Frag() : box{nullptr} {}

    Style::ComputedStyle const& style() const {
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

// https://drafts.csswg.org/css-align-3/#baseline-set
// https://drafts.csswg.org/css-writing-modes-3/#baseline
// https://www.w3.org/TR/css-inline-3/#baseline-types
// https://www.w3.org/TR/css-inline-3/#dominant-baseline-property
// NOTE: positions are relative to box top, not absolute
export struct BaselinePositionsSet {
    Au alphabetic;
    Au xHeight;
    Au xMiddle;
    Au capHeight;

    BaselinePositionsSet translate(Au delta) const {
        return {
            alphabetic + delta,
            xHeight + delta,
            xMiddle + delta,
            capHeight + delta,
        };
    }

    void repr(Io::Emit& e) const {
        e("(baselineset ");
        e(" alphabetic {}", alphabetic);
        e(" xHeight {}", xHeight);
        e(" xMiddle {}", xMiddle);
        e(" capHeight {}", capHeight);
        e(")\n");
    }
};

export struct Output {
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

    BaselinePositionsSet const firstBaselineSet = {};
    BaselinePositionsSet const lastBaselineSet = {};

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
