module;

#include <karm-base/rc.h>

export module Vaev.Engine:layout.grid;

import :layout.block;

namespace Vaev::Layout {

export Rc<FormatingContext> constructGridFormatingContext(Box& box) {
    return constructBlockFormatingContext(box);
}

} // namespace Vaev::Layout
