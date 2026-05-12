export module Vaev.Engine:values.computed.font;

import Karm.Core;

import :values.common;
import :values.computed.length;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Experimental {

struct FontFamily {
    Symbol name;

    FontFamily(Symbol name)
        : name(name) {
    }

    bool operator==(FontFamily const&) const = default;
    auto operator<=>(FontFamily const&) const = default;
};

export struct FontProps {
    Vec<FontFamily> families;
    Number weight;
    Percentage width;
    Px size;
};

} // namespace Vaev::Experimental
