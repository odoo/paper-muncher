export module Vaev.Engine:style.values.fonts;

import Karm.Core;

import :css.values.fonts;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-fonts/#family-name-value
export using FontFamily = Css::FontFamily;

export struct FontSizeContextData {
    f64 userFontSize = 16;   /// Font size of the user agent
    f64 parentFontSize = 16; /// Font size of the parent box
};

} // namespace Vaev::Style
