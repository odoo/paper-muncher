export module Vaev.Engine:layout2.layout;

import :layout.box;
import :layout.formating;
import :layout2.input;
import :layout2.output;
import :layout2.base;

namespace Vaev::Layout2 {

export Metrics computeMetrics(Layout::Box const& box, LogicalSize<Opt<Au>> const& containingBlock);

// http://w3.org/TR/css-layout-api-1/#intrinsicsizes
struct IntrinsicSizes {
    Au minContentSize;
    Au maxContentSize;
};

// https://www.w3.org/TR/css-layout-api-1/#intrinsic-sizes
IntrinsicSizes intrinsicSizes(Layout::Box const& box);

export Output layout(
    Layout::Box& box,
    Constraints const& constraints,
    Opt<BreakOpportunity const&> breakOpportunity = NONE,
    Opt<BreakNode const&> breakTree = NONE
);

} // namespace Vaev::Layout2
