export module Vaev.Engine:style.values.text;

import Karm.Core;

import :css.values.text;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-text-4/#propdef-text-align
export using TextAlign = Css::TextAlign;

// https://drafts.csswg.org/css-text-4/#propdef-text-transform
export using TextTransform = Css::TextTransform;

// https://drafts.csswg.org/css-text-4/#propdef-white-space
export using WhiteSpace = Css::WhiteSpace;

} // namespace Vaev::Style
