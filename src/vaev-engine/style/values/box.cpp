export module Vaev.Engine:style.values.box;

import Karm.Core;
import Karm.Math;

import :css.values.box;
import :css.values.keywords;
import :style.values.numeric;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-box-4/#propdef-margin-top
export using Margin = Union<Keywords::Auto, PxPercent>;

// https://drafts.csswg.org/css-box-4/#margins
export using Margins = Math::Insets<Margin>;

// https://drafts.csswg.org/css-box-4/#paddings
export using Paddings = Math::Insets<PxPercent>;

} // namespace Vaev::Style
