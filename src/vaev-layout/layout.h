#pragma once

#include "base.h"

namespace Vaev::Layout {

InsetsPx computeMargins(Tree &t, Box &f, Input input);

InsetsPx computeBorders(Tree &t, Box &f);

Output layout(Tree &t, Box &f, Input input);

} // namespace Vaev::Layout
