#pragma once

#include "css/parser.h"

namespace Vaev::Style {

static inline void eatWhitespace(Cursor<Css::Sst>& c) {
    while (not c.ended() and c.peek() == Css::Token::WHITESPACE)
        c.next();
}

} // namespace Vaev::Style
