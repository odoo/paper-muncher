export module Vaev.Engine:layout2.layout;

import :layout.box;
import :layout.formating;
import :layout2.input;
import :layout2.output;
import :layout2.base;

namespace Vaev::Layout2 {

export Metrics computeMetrics(Layout::Box const& box, LogicalSize<Opt<Au>> const& containingBlock);

IntrinsicSizes intrinsicInlineContentSizes(Layout::Box const& box);
IntrinsicContributions intrinsicInlineContributions(Layout::Box const& box);

export Output layout(
    Layout::Box& box,
    Constraints const& constraints,
    Opt<BreakOpportunity const&> breakOpportunity = NONE,
    Opt<BreakNode const&> breakTree = NONE
    );

} // namespace Vaev::Layout2
