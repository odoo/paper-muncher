export module Vaev.Engine:values.sizing;

import Karm.Core;

import :values.keywords;

using namespace Karm;

namespace Vaev {

// https://www.w3.org/TR/css-sizing-3/#box-sizing
export using BoxSizing = Union<Keywords::ContentBox, Keywords::BorderBox>;

} // namespace Vaev
