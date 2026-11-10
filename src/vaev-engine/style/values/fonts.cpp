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

// https://drafts.csswg.org/css-fonts-4/#font-weight-absolute-values
export f64 toComputed(Css::FontWeightAbsolute const& weight) {
    return weight.visit(
        [](Css::Keywords::Normal) -> f64 {
            return 400;
        },
        [](Css::Keywords::Bold) -> f64 {
            return 700;
        },
        [&](Css::Number const& number) {
            return toComputed(number);
        }
    );
}

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

// https://drafts.csswg.org/css-fonts-4/#font-style-prop
// Computed value: the keyword specified, plus angle in degrees if specified
export FontStyle toComputed(Css::FontStyle const& value) {
    if (not value.obliqueAngle)
        return {value.val};
    // <angle [-90deg,90deg]>: math functions are clamped to the range.
    auto angle = clamp(toComputed(*value.obliqueAngle).value(), -90.0, 90.0);
    return {value.val, Deg{angle}};
}

export Css::FontStyle toSpecified(FontStyle const& value) {
    // NOTE: 14deg is what an omitted angle represents, so it isn't written back.
    if (value.val == Gfx::FontStyle::OBLIQUE and value.obliqueAngle != Deg{14})
        return Css::FontStyle{toSpecified(value.obliqueAngle)};
    return Css::FontStyle{value.val};
}

// https://drafts.csswg.org/css-fonts-4/#font-width-prop
// Computed value: a percentage
export Percent toComputed(Css::FontWidth const& value) {
    return toComputed(value.val());
}

} // namespace Vaev::Style
