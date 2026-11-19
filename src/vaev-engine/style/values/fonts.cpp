export module Vaev.Engine:style.values.fonts;

import Karm.Core;
import Karm.Gfx;

import :css.values.fonts;
import :css.values.keywords;
import :css.values.numeric;
import :style.values.numeric;
import :style.values.base;

using namespace Karm;

namespace Vaev::Style {

export struct FontSizeContextData {
    f64 userFontSize = 16;   /// Font size of the user agent
    f64 parentFontSize = 16; /// Font size of the parent box
};

// https://drafts.csswg.org/css-fonts/#family-name-value
export using FontFamily = Css::FontFamily;

// https://drafts.csswg.org/css-fonts-4/#font-weight-absolute-values
export template <>
struct ValueComputer<Css::FontWeightAbsolute> {
    static f64 compute(Css::FontWeightAbsolute const& weight) {
        return weight.visit(
            [](Keywords::Normal) -> f64 {
                return 400;
            },
            [](Keywords::Bold) -> f64 {
                return 700;
            },
            [&](Css::Number const& number) {
                return computeValue(number);
            }
        );
    }

    static Css::FontWeightAbsolute specify(f64 value) {
        return specifyValue<Css::Number>(value);
    }
};

// https://drafts.csswg.org/css-fonts-4/#font-style-prop
// Computed value: the keyword specified, plus angle in degrees if specified
export struct FontStyle {
    Gfx::FontStyle val = Gfx::FontStyle::NORMAL;
    // The lack of an <angle> represents 14deg.
    Deg obliqueAngle = Deg{14.0};

    bool operator==(FontStyle const&) const = default;
};

export template <>
struct ValueComputer<Css::FontStyle> {
    static FontStyle compute(Css::FontStyle const& value) {
        return value.visit(
                [](Opt<Css::Angle> const& oblique) {
                    auto style = FontStyle{.val = Gfx::FontStyle::OBLIQUE};
                    if (auto const& [angle] = oblique)
                        style.obliqueAngle = computeValue(angle);
                    return style;
                },
                [](Keywords::Italic) {
                    return FontStyle{.val = Gfx::FontStyle::ITALIC};
                },
                // TODO: left and right
                [](auto) {
                    return FontStyle{.val = Gfx::FontStyle::NORMAL};
                }
            );
    }

    static Css::FontStyle specify(FontStyle const& value) {
        if (value.val == Gfx::FontStyle::ITALIC)
            return Keywords::ITALIC;
        if (value.val != Gfx::FontStyle::OBLIQUE)
            return Keywords::NORMAL;

        Opt<Css::Angle> angle = NONE;
        if (value.obliqueAngle != Deg{14})
            angle = Some(specifyValue<Css::Angle>(value.obliqueAngle));
        return angle;
    }
};

// https://drafts.csswg.org/css-fonts-4/#font-width-prop
// Computed value: a percentage
export template <>
struct ValueComputer<Css::FontWidth> {
    static Percent compute(Css::FontWidth const& value) {
        return value.visit(
            [](Keywords::UltraCondensed) {
                return Percent{50};
            },
            [](Keywords::ExtraCondensed) {
                return Percent{62.5};
            },
            [](Keywords::Condensed) {
                return Percent{75};
            },
            [](Keywords::SemiCondensed) {
                return Percent{87.5};
            },
            [](Keywords::Normal) {
                return Percent{100};
            },
            [](Keywords::SemiExpanded) {
                return Percent{112.5};
            },
            [](Keywords::Expanded) {
                return Percent{125};
            },
            [](Keywords::ExtraExpanded) {
                return Percent{150};
            },
            [](Keywords::UltraExpanded) {
                return Percent{200};
            },
            [](Css::Percentage const& percentage) {
                return computeValue(percentage);
            }
        );
    }

    static Css::FontWidth specify(Percent value) {
        return specifyValue<Css::Percentage>(value);
    }
};

} // namespace Vaev::Style
