#pragma once

#include <vaev-base/color.h>
#include <vaev-base/font.h>
#include <vaev-base/width.h>

#include "base.h"

namespace Vaev::Layout {

Px resolve(Tree &tree, Box &box, Length value);

Px resolve(Tree &tree, Box &box, PercentOr<Length> value, Px relative);

Px resolve(Tree &tree, Box &box, Width value, Px relative);

Px resolve(Tree &tree, Box &box, FontSize value);

} // namespace Vaev::Layout
