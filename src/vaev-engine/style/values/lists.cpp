export module Vaev.Engine:style.values.lists;

import Karm.Core;

import :css.values.lists;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-lists/#counter-functions
export using CounterFunc = Css::CounterFunc;

export using CountersFunc = Css::CountersFunc;

// https://www.w3.org/TR/css-lists-3/#image-markers
export using ListImage = Css::ListImage;

// https://www.w3.org/TR/css-lists-3/#list-style-position-property
export using ListPosition = Css::ListPosition;

// https://www.w3.org/TR/css-lists-3/#marker-side
export using MarkerSide = Css::MarkerSide;

// https://www.w3.org/TR/css-lists-3/#text-markers
export using ListType = Css::ListType;

// https://www.w3.org/TR/css-lists-3/#counter-reset
export using CounterReset = Css::CounterReset;

// https://www.w3.org/TR/css-lists-3/#increment-set
export using CounterIncrement = Css::CounterIncrement;

} // namespace Vaev::Style
