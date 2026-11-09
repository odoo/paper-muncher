export module Vaev.Engine:style.values.multicol;

import Karm.Core;

import :css.values.multicol;

using namespace Karm;

// https://drafts.csswg.org/css-multicol-1/

namespace Vaev::Style {

// https://drafts.csswg.org/css-multicol-2/#cc
// FIXME: Enforce the [1,∞] range.
export using ColumnCount = Css::ColumnCount;

} // namespace Vaev::Style
