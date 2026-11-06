module;

#include <karm/macros>

export module Vaev.Engine:style.values.length;

import Karm.Core;

import :css.values.length;
import :css.values.unitValue;
import :values.unitValue;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev::Style {

export using Pixel = Distinct<f64, struct _PixelTag>;

// 6. MARK: Distance Units: the <length> type
// https://drafts.csswg.org/css-values/#lengths

export Pixel toComputed(Css::PureLength const& value, LengthContext auto const& ctx) {
    return Pixel{value.unitValue().canonical(ctx).value()};
}

export Pixel toComputed(Css::PureAbsoluteLength value) {
    return Pixel{value.toPx()};
}

} // namespace Vaev::Style

namespace Vaev::Literals {

export constexpr Style::Pixel operator""_px(unsigned long long val) {
    return Style::Pixel{static_cast<f32>(val)};
}

export constexpr Style::Pixel operator""_px(long double val) {
    return Style::Pixel{static_cast<f32>(val)};
}

} // namespace Vaev::Literals

export template <>
struct Karm::Io::Formatter<Vaev::Style::Pixel> {
    Res<> format(Io::TextWriter& writer, Vaev::Style::Pixel value) {
        return Io::format(writer, "{}px", value.value());
    }
};
