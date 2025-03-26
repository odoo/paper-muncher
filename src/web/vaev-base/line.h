#pragma once

#include <karm-base/union.h>

#include "keywords.h"
#include "length.h"
#include "numbers.h"
#include "percent.h"

namespace Vaev {

using LineHeight = Union<Keywords::Normal, PercentOr<Length>, Number>;

LineHeight resolveToComputedValue(LineHeight const& lh) {
    if (lh == Keywords::NORMAL)
        return Number{1.2};
    return lh;
}

} // namespace Vaev
