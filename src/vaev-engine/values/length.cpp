module;

#include <karm/macros>

export module Vaev.Engine:values.length;

import Karm.Core;
import Karm.Math;

import :css;
import :values.base;
import :values.unitValue;
import :values.primitives;

using namespace Karm;
using namespace Karm::Math::Literals;

namespace Vaev {

export using Math::Au;
export constexpr Au INDEFINITE = Limits<Au>::MAX;

export using Math::InsetsAu;
export using Math::RectAu;
export using Math::Vec2Au;
export using Math::RadiiAu;
export using Math::EllipseAu;

} // namespace Vaev

namespace Vaev::Style {

export using Pixel = Distinct<f64, struct _PixelTag>;

// 6. MARK: Distance Units: the <length> type
// https://drafts.csswg.org/css-values/#lengths

export Au resolve(Css::PureLength const& value, Css::LengthContext auto const& ctx) {
    return Au::fromFloatNearest(value.unitValue().canonical(ctx).value());
}

} // namespace Vaev::Style

namespace Vaev::Css {

// 6.2. MARK: Absolute Lengths: the cm, mm, Q, in, pt, pc, px units
// https://drafts.csswg.org/css-values/#absolute-lengths

export struct PureAbsoluteLength : TypedUnitValue<Some(NumericBaseType::LENGTH)> {
    constexpr PureAbsoluteLength(Style::Pixel px)
        : TypedUnitValue(UnitValue{px.value(), UnitValue::Unit::PX}) {}

    constexpr PureAbsoluteLength(Au value)
        : TypedUnitValue(UnitValue{value.cast<f64>(), UnitValue::Unit::PX}) {}

    constexpr explicit PureAbsoluteLength(UnitValue value)
        : TypedUnitValue(value) {
        if (not value.isAbsolute())
            panic("unit value is not an absolute <length>");
    }

    constexpr Style::Pixel pixels() const {
        return Style::Pixel{_value.to(UnitValue::Unit::PX)};
    }
};

} // namespace Vaev::Css

namespace Vaev::Style {

export Au resolve(Css::PureAbsoluteLength value) {
    return Au::fromFloatNearest(value.pixels().value());
}

} // namespace Vaev::Style

namespace Vaev::Css {

export template <>
struct ValueParser<PureAbsoluteLength> {
    static Res<PureAbsoluteLength> parse(Cursor<Sst>& c) {
        auto save = c;
        auto value = try$(parseUnitValue(c, Some(NumericBaseType::LENGTH)));
        if (not value.isAbsolute()) {
            c = save;
            return Error::invalidData("expected <absolute-length>");
        }
        return Ok(PureAbsoluteLength{value});
    }
};

} // namespace Vaev::Css

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
