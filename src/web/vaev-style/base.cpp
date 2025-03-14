module;

#include <karm-base/cursor.h>

export module Vaev.Style:base;

import Vaev.Style.Css;

namespace Vaev::Style {

export void eatWhitespace(Cursor<Css::Sst>& c) {
    while (not c.ended() and c.peek() == Css::Token::WHITESPACE)
        c.next();
}

} // namespace Vaev::Style
