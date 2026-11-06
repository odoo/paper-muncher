export module Vaev.Engine:values.keywords;

import Karm.Core;

using namespace Karm;

namespace Vaev {

export template <StrLit K>
struct Keyword {
    void repr(Io::Emit& e) const {
        e("(keyword {})", K);
    }

    template <Karm::StrLit L>
    constexpr bool operator==(Keyword<L> const&) const {
        return K == L;
    }
};

namespace Keywords {

#define KEYWORD(TYPE, VALUE, NAME)      \
    export using TYPE = Keyword<VALUE>; \
    export constexpr inline TYPE NAME{};
#include "defs/keywords.inc"
#undef KEYWORD

} // namespace Keywords

export template <StrLit K>
Keyword<K> toComputed(Keyword<K> const& value, auto const&) {
    return value;
}
export template <StrLit K>
Keyword<K> toSpecified(Keyword<K> const& value) {
    return value;
}

} // namespace Vaev
