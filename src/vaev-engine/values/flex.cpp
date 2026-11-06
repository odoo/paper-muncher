export module Vaev.Engine:values.flex;

import Karm.Core;

import :values.keywords;

using namespace Karm;

namespace Vaev {

export using FlexDirection = Union<
    Keywords::Row,
    Keywords::RowReverse,
    Keywords::Column,
    Keywords::ColumnReverse>;

export using FlexWrap = Union<
    Keywords::Nowrap,
    Keywords::Wrap,
    Keywords::WrapReverse>;

} // namespace Vaev
