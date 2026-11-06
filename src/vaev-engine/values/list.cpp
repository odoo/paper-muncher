export module Vaev.Engine:values.list;

import Karm.Core;

import :values.keywords;

namespace Vaev {

// https://www.w3.org/TR/css-lists-3/#list-style-position-property
export using ListPosition = Union<Keywords::Inside, Keywords::Outside>;

// https://www.w3.org/TR/css-lists-3/#marker-side
export using MarkerSide = Union<Keywords::MatchSelf, Keywords::MatchParent>;

} // namespace Vaev
