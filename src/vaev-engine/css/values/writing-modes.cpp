export module Vaev.Engine:css.values.writingModes;

import Karm.Core;

import :css.values.keywords;

using namespace Karm;

// https://drafts.csswg.org/css-writing-modes-4/

namespace Vaev::Css {

// https://drafts.csswg.org/css-writing-modes-4/#propdef-writing-mode
export using WritingMode = Union<Keywords::HorizontalTb, Keywords::VerticalRl, Keywords::VerticalLr, Keywords::SidewaysRl, Keywords::SidewaysLr>;

// https://drafts.csswg.org/css-writing-modes-4/#propdef-direction
export using Direction = Union<Keywords::Ltr, Keywords::Rtl>;

} // namespace Vaev::Css
