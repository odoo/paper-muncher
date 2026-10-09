export module Vaev.Engine:layout2.output;

import :layout2.sizing;
// FIXME: Should not be necessary
import :layout2.input;
import :layout2.break_;
import :layout.fragment;

namespace Vaev::Layout2 {

struct Completed {
    PendingMargin pendingMargin;
    BreakBetween finalBreakAfter;
    Opt<BreakOpportunity> bestBreakOpportunity;
};

struct Broke {
    BreakNode tree;
    BreakAppeal appeal;
    bool forced;
};

struct NeedsEarlierBreak {
    BreakOpportunity opportunity;
};

struct Placed {
    Rc<Layout::Fragment> fragment;
    Opt<Au> blockOffset = NONE;
    BreakBetween initialBreakBefore;
    Union<Completed, Broke> breakState;
};

using Abort = Union<NeedsEarlierBreak>;

using Output = Union<Placed, Abort>;

} // namespace Vaev::Layout2
