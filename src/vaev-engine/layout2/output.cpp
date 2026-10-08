export module Vaev.Engine:layout2.output;

import :layout2.sizing;
// FIXME: Should not be necessary
import :layout2.input;
import :layout2.break_;
import :layout.fragment;

namespace Vaev::Layout2 {

struct Completed {
    PendingMargin pendingMargin;
    Opt<BreakOpportunity> bestBreakOpportunity;
};

struct Broke {
    BreakAppeal appeal;
    BreakNode tree;
};

struct NeedsEarlierBreak {
    BreakOpportunity opportunity;
};

struct Placed {
    Rc<Layout::Fragment> fragment;
    Opt<Au> blockOffset = NONE;
    Union<Completed, Broke> breakState;
};

using Abort = Union<NeedsEarlierBreak>;

using Output = Union<Placed, Abort>;

} // namespace Vaev::Layout2
