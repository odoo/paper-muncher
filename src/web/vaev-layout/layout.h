#pragma once

#include "base.h"

namespace Vaev::Layout {

InsetsPx computeMargins(Tree &tree, Box &box, Input input);

InsetsPx computeBorders(Tree &tree, Box &box);

Vec2Px computeIntrinsicSize(Tree &tree, Box &box, IntrinsicSize intrinsic, Vec2Px containingBlock);

Px computeShrinkToFitWidth(Tree &tree, Box &box, Vec2Px containingBlock, Vec2Px availableSpace);

Output layout(Tree &tree, Box &box, Input input);

} // namespace Vaev::Layout
