export module Vaev.Engine:values.common.keywords;

import Karm.Core;

using namespace Karm;

namespace Vaev::Experimental {

namespace Keywords {

export template <StrLit K>
struct Keyword {
    void repr(Io::Emit& e) const {
        e("(keyword {})", K);
    }

    template <StrLit L>
    constexpr bool operator==(Keyword<L> const&) const {
        return K == L;
    }
};

#define KEYWORD(TYPE, VALUE, NAME)      \
    export using TYPE = Keyword<VALUE>; \
    export constexpr inline TYPE NAME{};
#include "../defs/keywords.inc"
#undef KEYWORD

} // namespace Keywords

} // namespace Vaev::Experimental
