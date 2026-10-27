module;

#include <karm/macros>

export module Vaev.Engine:values.length;

import Karm.Core;
import Karm.Math;

import :css;
import :values.base;
import :values.unitValue;
import :values.primitives;
import :values.resolved;

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

export using Pixel = Distinct<f64, struct _PixelTag>;

// 6. MARK: Distance Units: the <length> type
// https://drafts.csswg.org/css-values/#lengths

export struct Length : TypedUnitValue<Some(NumericBaseType::LENGTH)> {
    using TypedUnitValue::TypedUnitValue;

    static constexpr Length fromPx(f64 val) {
        return Length{UnitValue{val, UnitValue::Unit::PX}};
    }

    static constexpr Length fromEm(f64 val) {
        return Length{UnitValue{val, UnitValue::Unit::EM}};
    }

    constexpr Length() = default;

    constexpr Length(Au value)
        : TypedUnitValue(UnitValue{value.cast<f64>(), UnitValue::Unit::PX}) {}

    constexpr f64 value() const {
        return _value.value();
    }

    constexpr UnitValue::Unit unit() const {
        return _value.unit();
    }

    // https://drafts.csswg.org/css-values/#absolute-lengths
    constexpr bool absolute() const {
        return _value.isAbsolute();
    }

    constexpr Opt<f64> absolutePixels() const {
        if (not absolute())
            return NONE;
        return Some(_value.to(UnitValue::Unit::PX));
    }
};

export template <>
struct _Resolved<Length> {
    using Type = Au;
};

export Au resolve(Length const& value, LengthContext auto const& ctx) {
    return Au::fromFloatNearest(value.unitValue().canonical(ctx).value());
}

// 6.2. MARK: Absolute Lengths: the cm, mm, Q, in, pt, pc, px units
// https://drafts.csswg.org/css-values/#absolute-lengths

export struct AbsoluteLength : TypedUnitValue<Some(NumericBaseType::LENGTH)> {
    constexpr AbsoluteLength(Pixel px)
        : TypedUnitValue(UnitValue{px.value(), UnitValue::Unit::PX}) {}

    constexpr AbsoluteLength(Au value)
        : TypedUnitValue(UnitValue{value.cast<f64>(), UnitValue::Unit::PX}) {}

    constexpr explicit AbsoluteLength(UnitValue value)
        : TypedUnitValue(value) {
        if (not value.isAbsolute())
            panic("unit value is not an absolute <length>");
    }

    constexpr Pixel pixels() const {
        return Pixel{_value.to(UnitValue::Unit::PX)};
    }

    constexpr operator Length() const {
        return Length{_value};
    }
};

export template <>
struct _Resolved<AbsoluteLength> {
    using Type = Au;
};

export Au resolve(AbsoluteLength value) {
    return Au::fromFloatNearest(value.pixels().value());
}

export template <>
struct ValueParser<AbsoluteLength> {
    static Res<AbsoluteLength> parse(Cursor<Css::Sst>& c) {
        auto save = c;
        auto value = try$(parseUnitValue(c, Some(NumericBaseType::LENGTH)));
        if (not value.isAbsolute()) {
            c = save;
            return Error::invalidData("expected <absolute-length>");
        }
        return Ok(AbsoluteLength{value});
    }
};

} // namespace Vaev

namespace Vaev::Literals {

export constexpr Pixel operator""_px(unsigned long long val) {
    return Pixel{static_cast<f32>(val)};
}

export constexpr Pixel operator""_px(long double val) {
    return Pixel{static_cast<f32>(val)};
}

} // namespace Vaev::Literals

export template <>
struct Karm::Io::Formatter<Vaev::Pixel> {
    Res<> format(Io::TextWriter& writer, Vaev::Pixel value) {
        return Io::format(writer, "{}px", value.value());
    }
};
