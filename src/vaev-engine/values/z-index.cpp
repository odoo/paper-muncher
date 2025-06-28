module;

#include <karm-io/emit.h>

export module Vaev.Engine:values.z_index;

import :values.keywords;
import :values.primitives;

namespace Vaev {

export using ZIndex = FlatUnion<
    Keywords::Auto,
    Integer>;

} // namespace Vaev
