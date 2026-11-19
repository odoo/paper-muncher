export module Vaev.Engine:style.values.svg;

import Karm.Core;
import Karm.Math;
import Karm.Gfx;

import :css.values.color;
import :css.values.svg;
import :style.values.color;
import :style.values.base;

using namespace Karm::Literals;
using namespace Karm::Math::Literals;

using namespace Karm;

namespace Vaev::Style {

// https://svgwg.org/svg2-draft/coords.html#PreserveAspectRatioAttribute
export using SvgAlignAxis = Css::SvgAlignAxis;

// https://svgwg.org/svg2-draft/coords.html#PreserveAspectRatioAttribute
export using SvgAlign = Css::SvgAlign;

// https://svgwg.org/svg2-draft/coords.html#PreserveAspectRatioAttribute
export using SvgMeetOrSlice = Css::SvgMeetOrSlice;

// https://svgwg.org/svg2-draft/painting.html#SpecifyingPaint
export using SvgPaint = Opt<Color>;

export template <>
struct ValueComputer<Css::SvgPaint> {
    static SvgPaint compute(Css::SvgPaint const& value, Gfx::Color currentColor) {
        return value.map([&](Css::Color const& color) {
            return computeValue(color, currentColor);
        });
    }

    static Css::SvgPaint specify(SvgPaint const& value) {
        return value.map([](Color const& color) {
            return specifyValue<Css::Color>(color);
        });
    }
};

export Opt<Gfx::Color> resolve(SvgPaint const& value, Gfx::Color currentColor) {
    return value.map([&](Color const& color) {
        return resolve(color, currentColor);
    });
}

// https://svgwg.org/svg2-draft/coords.html#ViewBoxAttribute
export using SvgViewBox = Css::SvgViewBox;

} // namespace Vaev::Style
