#pragma once

#include "css/parser.h"

namespace Vive::Style {

static void eatWhitespace(Cursor<Css::Sst>& c) {
    while (not c.ended() and c.peek() == Css::Token::WHITESPACE)
        c.next();
}

} // namespace Vaev::Style
