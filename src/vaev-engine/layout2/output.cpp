export module Vaev.Engine:layout2.output;

import :layout2.sizing;
// FIXME: Should not be necessary
import :layout2.input;
import :layout.fragment;

namespace Vaev::Layout2 {

export struct Output {
    PendingMargin pendingMargin;

    LogicalSize<Au> size;
    Opt<Au> blockOffset;

    Opt<Rc<Layout::Fragment>> fragment;
};

} // namespace Vaev::Layout2
