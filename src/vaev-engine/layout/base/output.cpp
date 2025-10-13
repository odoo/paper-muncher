export module Vaev.Engine:layout.output;

import Karm.Math;
import Karm.Core;
import :values;

import :layout.breaks;

namespace Vaev::Layout {

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

    static BaselinePositionsSet fromSinglePosition(Au pos) {
        return {
            pos,
            pos,
            pos,
            pos,
        };
    }

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

} // namespace Vaev::Layout
