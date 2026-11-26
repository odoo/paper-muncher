export module Vaev.Engine:layout2.output;

// FIXME: Should not be necessary
import :layout2.input;
import :layout2.break_;
import :layout.fragment;

namespace Vaev::Layout2 {

struct Completed {
    PendingMargin pendingMargin;
    Style::BreakBetween finalBreakAfter;
    Opt<BreakOpportunity> bestBreakOpportunity;
};

struct Broke {
    BreakNode tree;
    BreakAppeal appeal;
    Style::BreakBetween originatingBreak;
    bool forced;
};

struct NeedsEarlierBreak {
    BreakOpportunity opportunity;
};

struct Placed {
    Rc<Layout::Fragment> fragment;
    Opt<Au> blockOffset = NONE;
    Style::BreakBetween initialBreakBefore;
    Union<Completed, Broke> breakState;
};

using Output = Union<Placed, NeedsEarlierBreak>;

} // namespace Vaev::Layout2
