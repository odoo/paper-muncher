export module Vaev.Engine:css.values.content;

import Karm.Core;

import :css.values.gcpm;
import :css.values.keywords;
import :css.values.lists;
import :css.values.primitives;

using namespace Karm;

namespace Vaev::Css {

// https://drafts.csswg.org/css-content-3/#content-property
export using ContentValue = Union<
    Keywords::Normal,
    Keywords::None,
    String,
    ElementFunc,
    CounterFunc,
    CountersFunc>;

} // namespace Vaev::Css
