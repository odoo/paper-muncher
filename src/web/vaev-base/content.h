#pragma once

#include "image.h"
#include "string.h"

namespace Vaev {

enum struct ContentKeyword {
    NONE,
    NORMAL,
};

enum struct Quotes {
    OPEN_QUOTE,
    CLOSE_QUOTE,
    NO_OPEN_QUOTE,
    NO_CLOSE_QUOTE,
};

using _Content = Union<
    ContentKeyword,
    Quotes,
    StringFunc,
    Image>;

struct Content : public _Content {};

} // namespace Vaev
