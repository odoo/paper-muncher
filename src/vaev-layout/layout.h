#pragma once

#include "base.h"
#include "box.h"
#include "frag.h"

namespace Vaev::Layout {

struct Tree {
    Box root;
    Viewport viewport;
};

InsetsPx computeMargins(Tree &t, Box &box, Input input);

InsetsPx computeBorders(Tree &t, Box &f);

Output layout(Tree &t, Box &box, Input input);

} // namespace Vaev::Layout
