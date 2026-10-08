export module Vaev.Engine:layout2.input;

import :layout2.break_;
import :layout2.sizing;

namespace Vaev::Layout2 {

export enum struct MinContent { MIN_CONTENT };
export using enum MinContent;

export enum struct MaxContent { MAX_CONTENT };
export using enum MaxContent;

using AvailableSpace = Union<Au, MinContent, MaxContent>;

enum struct AutoSizeBehavior {
    STRETCH_FIT,
    FIT_CONTENT,
    _LEN,
};

struct PendingMargin {
    Au positive = 0_au;
    Au negative = 0_au;

    void add(Au value) {
        if (value > 0_au) {
            positive = max(value, positive);
        } else {
            negative = min(value, negative);
        }
    }

    Au sum() const {
        return positive + negative;
    }

    void repr(Io::Emit& e) const {
        e("(pendingMargin pos={} neg={})", positive, negative);
    }
};

export struct Constraints {
    LogicalSize<Opt<Au>> knownSize = {NONE, NONE};
    LogicalSize<Opt<Au>> containingBlock = {NONE, NONE};
    LogicalSize<AvailableSpace> availableSpace = {MAX_CONTENT, MAX_CONTENT};

    AutoSizeBehavior inlineAutoSizeBehavior = AutoSizeBehavior::STRETCH_FIT;
    AutoSizeBehavior blockAutoSizeBehavior = AutoSizeBehavior::FIT_CONTENT;

    bool collapseMargins = true;
    PendingMargin pendingMargin = PendingMargin{};

    LogicalInsets<Au> margins;

    Opt<Fragmentainer> fragmentainer = NONE;

    Opt<BreakOpportunity> replayedBreak = NONE;
};

export struct Metrics {
    LogicalInsets<Au> borders;
    LogicalInsets<Au> paddings;
    LogicalSize<Opt<Au>> size;

    void repr(Io::Emit& e) const {
        e.ln("(metrics");
        e.indent();
        e.ln("borders={}", borders);
        e.ln("paddings={}", paddings);
        e.ln("size={}", size);
        e.deindent();
        e(")");
    }
};

export struct Input {
    Constraints constraints;
    Metrics metrics;
};

} // namespace Vaev::Layout2
