export module Vaev.Engine:values.common.keywords;

namespace Vaev::Experimental {

namespace Keywords {

#define KEYWORD(TYPE, VALUE, NAME)      \
    export using TYPE = Keyword<VALUE>; \
    export constexpr inline TYPE NAME{};
#include "../defs/keywords.inc"
#undef KEYWORD

} // namespace Keywords

} // namespace Vaev::Experimental
