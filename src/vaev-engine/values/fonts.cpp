module;

#include <karm/macros>

export module Vaev.Engine:values.fonts;

import Karm.Core;

using namespace Karm;

namespace Vaev {

export struct FontFamily {
    Symbol name;

    FontFamily(Symbol name)
        : name(name) {
    }

    void repr(Io::Emit& e) const {
        e("(FontFamily name:{})", name);
    }

    bool operator==(FontFamily const&) const = default;
    auto operator<=>(FontFamily const&) const = default;
};

} // namespace Vaev
