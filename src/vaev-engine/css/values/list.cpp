module;

#include <karm/macros>

export module Vaev.Engine:css.values.list;

import :css.values.image;
import :css.values.keywords;
import :values.keywords;

namespace Vaev::Css {

// https://www.w3.org/TR/css-lists-3/#image-markers
export using ListImage = Union<Keywords::None, Image>;

} // namespace Vaev::Css
