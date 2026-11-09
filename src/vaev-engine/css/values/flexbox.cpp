export module Vaev.Engine:css.values.flexbox;

import Karm.Core;

import :css.values.keywords;

using namespace Karm;

// https://drafts.csswg.org/css-flexbox-1/

namespace Vaev::Css {

// https://drafts.csswg.org/css-flexbox-1/#flex-direction-property
export using FlexDirection = Union<
    Keywords::Row,
    Keywords::RowReverse,
    Keywords::Column,
    Keywords::ColumnReverse>;

// https://drafts.csswg.org/css-flexbox-1/#flex-wrap-property
export using FlexWrap = Union<
    Keywords::Nowrap,
    Keywords::Wrap,
    Keywords::WrapReverse>;

} // namespace Vaev::Css
