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
constexpr static inline auto AUTO = Auto{};

using None = Keyword<"none">;
constexpr static inline auto NONE = None{};

using MaxContent = Keyword<"max-content">;
constexpr static inline auto MAX_CONTENT = None{};

using MinContent = Keyword<"min-content">;
constexpr static inline auto MIN_CONTENT = None{};

using Content = Keyword<"content">;
constexpr static inline auto CONTENT = None{};

using CurrentColor = Keyword<"currentcolor">;
constexpr static inline auto CURRENT_COLOR = CurrentColor{};

using Normal = Keyword<"normal">;
constexpr static inline auto NORMAL = Normal{};

} // namespace Keywords

} // namespace Vaev
