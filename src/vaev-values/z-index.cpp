module;

#include <karm-io/emit.h>

export module Vaev.Values:z_index;

import :keywords;
import :primitives;

namespace Vaev {

export using ZIndex = FlatUnion<
    Keywords::Auto,
    Integer>;

} // namespace Vaev
