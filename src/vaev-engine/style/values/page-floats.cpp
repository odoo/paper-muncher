export module Vaev.Engine:style.values.pageFloats;

import Karm.Core;

import :css.values.pageFloats;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-page-floats-3/#propdef-float
export using Float = Css::Float;

// https://drafts.csswg.org/css-page-floats-3/#propdef-clear
export using Clear = Css::Clear;

} // namespace Vaev::Style
