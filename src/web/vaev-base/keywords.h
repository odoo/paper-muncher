#pragma once

#include <karm-base/string.h>
#include <karm-io/emit.h>

namespace Vaev {

template <Karm::StrLit K>
struct Keyword {
    void repr(Io::Emit& e) const {
        e("(keyword {})", K);
    }
};

namespace Keywords {
using Auto = Keyword<"auto">;
using None = Keyword<"none">;
using MaxContent = Keyword<"max-content">;
using MinContent = Keyword<"min-content">;
using Content = Keyword<"content">;
} // namespace Keywords

} // namespace Vaev
