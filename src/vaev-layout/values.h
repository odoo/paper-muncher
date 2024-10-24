#pragma once

#include <vaev-base/color.h>
#include <vaev-base/font.h>
#include <vaev-base/width.h>

#include "base.h"

namespace Vaev::Layout {

Px resolve(Tree &t, Box &f, Length l);

Px resolve(Tree &t, Box &f, PercentOr<Length> p, Px relative);

Px resolve(Tree &t, Box &f, Width w, Px relative);

Px resolve(Tree &t, Box &f, FontSize fs);

} // namespace Vaev::Layout
