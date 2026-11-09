export module Vaev.Engine:style.values.writingModes;

import Karm.Core;

import :css.values.writingModes;

using namespace Karm;

// https://drafts.csswg.org/css-writing-modes-4/

namespace Vaev::Style {

// https://drafts.csswg.org/css-writing-modes-4/#propdef-writing-mode
export using WritingMode = Css::WritingMode;

// https://drafts.csswg.org/css-writing-modes-4/#propdef-direction
export using Direction = Css::Direction;

} // namespace Vaev::Style
