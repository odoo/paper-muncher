#pragma once

#include <karm-base/map.h>
#include <karm-base/string.h>

namespace Vaev {

enum struct StringKeyword {
    FIRST,
    START,
    LAST,
    FIRST_EXCEPT,
};

struct StringFunc {
    String ident;
    StringKeyword keyword;
};

struct StringSet {
    Map<String, String> values;
};

} // namespace Vaev
