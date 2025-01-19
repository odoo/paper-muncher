#include "grid.h"

#include "block.h"

namespace Vaev::Layout {

Strong<FormatingContext> constructGridFormatingContext(Box& box) {
    return constructBlockFormatingContext(box);
}

} // namespace Vaev::Layout
