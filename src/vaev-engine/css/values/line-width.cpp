module;

#include <karm/macros>

export module Vaev.Engine:css.values.lineWidth;

import :css.lexer;
import :css.parser;
import :css.values.base;
import :css.values.calc;
import :css.values.keywords;
import :css.values.primitives;
import :values.keywords;

namespace Vaev::Css {

// https://drafts.csswg.org/css-backgrounds/#typedef-line-width
export using LineWidth = Union<
    Keywords::Thin,
    Keywords::Medium,
    Keywords::Thick,
    Length>;

} // namespace Vaev::Css
