module;

#include <karm/macros>

export module Vaev.Engine:css.values.list;

import :css.values.counter;
import :css.values.image;
import :css.values.keywords;
import :css.values.primitives;
import :values.keywords;
import :values.primitives;

namespace Vaev::Css {

// https://www.w3.org/TR/css-lists-3/#image-markers
export using ListImage = Union<Keywords::None, Image>;

// https://www.w3.org/TR/css-lists-3/#text-markers
export using ListType = Union<Keywords::None, CustomIdent, CounterSymbolsFunc, String>;

} // namespace Vaev::Css
