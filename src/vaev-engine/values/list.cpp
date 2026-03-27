module;

#include <karm/macros>

export module Vaev.Engine:values.list;

import :values.primitives;
import :values.keywords;
import :values.counter;

namespace Vaev {

// https://www.w3.org/TR/css-lists-3/#image-markers
export using ListImage = Union<Keywords::None, Image>;

// https://www.w3.org/TR/css-lists-3/#text-markers
export using ListType = Union<Keywords::None, CustomIdent, CounterSymbolsFunc, String>;

// https://www.w3.org/TR/css-lists-3/#list-style-position-property
export using ListPosition = Union<Keywords::Inside, Keywords::Outside>;

// https://www.w3.org/TR/css-lists-3/#marker-side
export using MarkerSide = Union<Keywords::MatchSelf, Keywords::MatchParent>;

struct ListProps {
    ListImage image = Keywords::NONE;
    ListType type = CustomIdent{"disc"_sym};
    ListPosition position = Keywords::OUTSIDE;
    MarkerSide markerSide = Keywords::MATCH_SELF;
};

} // namespace Vaev