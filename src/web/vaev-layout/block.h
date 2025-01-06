#pragma once

#include "base.h"

namespace Vaev::Layout {

Output blockLayout(Tree &tree, Box &box, Input input, usize startAt, Opt<usize> stopAt);

} // namespace Vaev::Layout
