module;

#include <karm/macros>

export module Vaev.Engine:style.values.table;

import Karm.Core;

import :style.values.length;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev::Style {

export struct BorderSpacing {
    Pixel horizontal = Pixel{0};
    Pixel vertical = Pixel{0};
};

} // namespace Vaev::Style
