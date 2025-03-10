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
static constexpr Auto AUTO{};

using Medium = Keyword<"medium">;
static constexpr Medium MEDIUM{};

using Thick = Keyword<"thick">;
static constexpr Thick THICK{};

using Thin = Keyword<"thin">;
static constexpr Thin THIN{};

using CurrentColor = Keyword<"currentcolor">;
static constexpr CurrentColor CURRENT_COLOR{};

} // namespace Keywords

} // namespace Vaev
