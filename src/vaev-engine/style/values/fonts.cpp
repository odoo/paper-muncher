export module Vaev.Engine:style.values.fonts;

import Karm.Core;
import Karm.Gfx;

import :css.values.fonts;
import :style.values.numeric;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-fonts/#family-name-value
export using FontFamily = Css::FontFamily;

export struct FontSizeContextData {
    f64 userFontSize = 16;   /// Font size of the user agent
    f64 parentFontSize = 16; /// Font size of the parent box
};

// https://drafts.csswg.org/css-fonts-4/#font-style-prop
// Computed value: the keyword specified, plus angle in degrees if specified
export struct FontStyle {
    Gfx::FontStyle val = Gfx::FontStyle::NORMAL;
    // The lack of an <angle> represents 14deg.
    Deg obliqueAngle = Deg{14};

    bool operator==(FontStyle const&) const = default;

    void repr(Io::Emit& e) const {
        e("{}", val);
        if (val == Gfx::FontStyle::OBLIQUE)
            e(" {}", obliqueAngle);
    }
};

} // namespace Vaev::Style
