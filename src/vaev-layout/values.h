#pragma once

#include <vaev-base/color.h>
#include <vaev-base/font.h>
#include <vaev-base/width.h>

#include "base.h"

namespace Vaev::Layout {

Px resolve(Tree const &tree, Box const &box, Length value);

Px resolve(Tree const &tree, Box const &box, PercentOr<Length> value, Px relative);

Px resolve(Tree const &tree, Box const &box, Width value, Px relative);

Px resolve(Tree const &tree, Box const &box, FontSize value);

} // namespace Vaev::Layout
