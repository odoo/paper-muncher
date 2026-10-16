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
import :values.writing;

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

export struct Length : TypedUnitValue<NumericBaseType::LENGTH> {
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
        return _value.absolute();
    }

    constexpr Opt<f64> absolutePixels() const {
        if (not absolute())
            return NONE;
        return Some(_value.to(UnitValue::Unit::PX));
    }
};

export struct RelativeLengthContextData {
    // current font metrics
    f64 fontSize;
    f64 xHeight;
    f64 capHeight;
    f64 zeroAdvance;
    f64 lineHeight;

    // relative font metrics
    f64 rootFontSize;
    f64 rootXHeight;
    f64 rootCapHeight;
    f64 rootZeroAdvance;
    f64 rootLineHeight;

    f64 viewportSmallWidth;
    f64 viewportSmallHeight;

    f64 viewportLargeWidth;
    f64 viewportLargeHeight;

    f64 viewportDynamicWidth;
    f64 viewportDynamicHeight;

    Axis boxAxis = Axis::HORIZONTAL;
};

export template <typename T>
concept RelativeLengthContext = requires(T t) {
    // Current font metrics
    { t.fontSize } -> Meta::Convertible<f64>;
    { t.xHeight } -> Meta::Convertible<f64>;
    { t.capHeight } -> Meta::Convertible<f64>;
    { t.zeroAdvance } -> Meta::Convertible<f64>;
    { t.lineHeight } -> Meta::Convertible<f64>;

    // Relative font metrics
    { t.rootFontSize } -> Meta::Convertible<f64>;
    { t.rootXHeight } -> Meta::Convertible<f64>;
    { t.rootCapHeight } -> Meta::Convertible<f64>;
    { t.rootZeroAdvance } -> Meta::Convertible<f64>;
    { t.rootLineHeight } -> Meta::Convertible<f64>;

    // Viewport dimensions (Small, Large, Dynamic)
    { t.viewportSmallWidth } -> Meta::Convertible<f64>;
    { t.viewportSmallHeight } -> Meta::Convertible<f64>;
    { t.viewportLargeWidth } -> Meta::Convertible<f64>;
    { t.viewportLargeHeight } -> Meta::Convertible<f64>;
    { t.viewportDynamicWidth } -> Meta::Convertible<f64>;
    { t.viewportDynamicHeight } -> Meta::Convertible<f64>;

    // Layout axis constraint
    { t.boxAxis } -> Meta::Convertible<Axis>;
};

export template <typename T>
concept LengthContext = RelativeLengthContext<T>;

// https://drafts.csswg.org/css-values/#relative-lengths
Au _resolveRelative(f64 value, UnitValue::Unit unit, RelativeLengthContext auto const& ctx) {
    switch (unit) {
    case UnitValue::Unit::EM:
        return Au::fromFloatNearest(value * ctx.fontSize);

    case UnitValue::Unit::REM:
        return Au::fromFloatNearest(value * ctx.rootFontSize);

    case UnitValue::Unit::EX:
        return Au::fromFloatNearest(value * ctx.xHeight);

    case UnitValue::Unit::REX:
        return Au::fromFloatNearest(value * ctx.rootXHeight);

    case UnitValue::Unit::CAP:
        return Au::fromFloatNearest(value * ctx.capHeight);

    case UnitValue::Unit::RCAP:
        return Au::fromFloatNearest(value * ctx.rootCapHeight);

    case UnitValue::Unit::CH:
        return Au::fromFloatNearest(value * ctx.zeroAdvance);

    case UnitValue::Unit::RCH:
        return Au::fromFloatNearest(value * ctx.rootZeroAdvance);

    case UnitValue::Unit::IC:
        return Au::fromFloatNearest(value * ctx.zeroAdvance);

    case UnitValue::Unit::RIC:
        return Au::fromFloatNearest(value * ctx.rootZeroAdvance);

    case UnitValue::Unit::LH:
        return Au::fromFloatNearest(value * ctx.lineHeight);

    case UnitValue::Unit::RLH:
        return Au::fromFloatNearest(value * ctx.rootLineHeight);
    // Viewport-relative

    // https://drafts.csswg.org/css-values/#vw

    // Equal to 1% of the width of current viewport.
    case UnitValue::Unit::VW:
    case UnitValue::Unit::LVW:
        return Au::fromFloatNearest(value * ctx.viewportLargeWidth / 100);

    case UnitValue::Unit::SVW:
        return Au::fromFloatNearest(value * ctx.viewportSmallWidth / 100);

    case UnitValue::Unit::DVW:
        return Au::fromFloatNearest(value * ctx.viewportDynamicWidth / 100);

    // https://drafts.csswg.org/css-values/#vh
    // Equal to 1% of the height of current viewport.
    case UnitValue::Unit::VH:
    case UnitValue::Unit::LVH:
        return Au::fromFloatNearest(value * ctx.viewportLargeHeight / 100);

    case UnitValue::Unit::SVH:
        return Au::fromFloatNearest(value * ctx.viewportSmallHeight / 100);

    case UnitValue::Unit::DVH:
        return Au::fromFloatNearest(value * ctx.viewportDynamicHeight / 100);

    // https://drafts.csswg.org/css-values/#vi
    // Equal to 1% of the size of the viewport in the box’s inline axis.
    case UnitValue::Unit::VI:
    case UnitValue::Unit::LVI:
        if (ctx.boxAxis == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value * ctx.viewportLargeWidth / 100);
        } else {
            return Au::fromFloatNearest(value * ctx.viewportLargeHeight / 100);
        }

    case UnitValue::Unit::SVI:
        if (ctx.boxAxis == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value * ctx.viewportSmallWidth / 100);
        } else {
            return Au::fromFloatNearest(value * ctx.viewportSmallHeight / 100);
        }

    case UnitValue::Unit::DVI:
        if (ctx.boxAxis == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value * ctx.viewportDynamicWidth / 100);
        } else {
            return Au::fromFloatNearest(value * ctx.viewportDynamicHeight / 100);
        }

    // https://drafts.csswg.org/css-values/#vb
    // Equal to 1% of the size of the viewport in the box’s block axis.
    case UnitValue::Unit::VB:
    case UnitValue::Unit::LVB:
        if (ctx.boxAxis.cross() == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value * ctx.viewportLargeWidth / 100);
        } else {
            return Au::fromFloatNearest(value * ctx.viewportLargeHeight / 100);
        }

    case UnitValue::Unit::SVB:
        if (ctx.boxAxis.cross() == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value * ctx.viewportSmallWidth / 100);
        } else {
            return Au::fromFloatNearest(value * ctx.viewportSmallHeight / 100);
        }

    case UnitValue::Unit::DVB:
        if (ctx.boxAxis.cross() == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value * ctx.viewportDynamicWidth / 100);
        } else {
            return Au::fromFloatNearest(value * ctx.viewportDynamicHeight / 100);
        }

    // https://drafts.csswg.org/css-values/#vmin
    // Equal to the smaller of vw and vh.
    case UnitValue::Unit::VMIN:
    case UnitValue::Unit::LVMIN:
        return min(
            _resolveRelative(value, UnitValue::Unit::VW, ctx),
            _resolveRelative(value, UnitValue::Unit::VH, ctx)
        );

    case UnitValue::Unit::SVMIN:
        return min(
            _resolveRelative(value, UnitValue::Unit::SVW, ctx),
            _resolveRelative(value, UnitValue::Unit::SVH, ctx)
        );

    case UnitValue::Unit::DVMIN:
        return min(
            _resolveRelative(value, UnitValue::Unit::DVW, ctx),
            _resolveRelative(value, UnitValue::Unit::DVH, ctx)
        );

    // https://drafts.csswg.org/css-values/#vmax
    // Equal to the larger of vw and vh.
    case UnitValue::Unit::VMAX:
    case UnitValue::Unit::LVMAX:
        return max(
            _resolveRelative(value, UnitValue::Unit::VW, ctx),
            _resolveRelative(value, UnitValue::Unit::VH, ctx)
        );

    case UnitValue::Unit::DVMAX:
        return max(
            _resolveRelative(value, UnitValue::Unit::DVW, ctx),
            _resolveRelative(value, UnitValue::Unit::DVH, ctx)
        );

    case UnitValue::Unit::SVMAX:
        return max(
            _resolveRelative(value, UnitValue::Unit::SVW, ctx),
            _resolveRelative(value, UnitValue::Unit::SVH, ctx)
        );
    default:
        unreachable();
    }
}

export template <>
struct _Resolved<Length> {
    using Type = Au;
};

export Au resolve(Length const& value, LengthContext auto const& ctx) {
    if (auto px = value.absolutePixels())
        return Au::fromFloatNearest(*px);
    return _resolveRelative(value.value(), value.unit(), ctx);
}

// 6.2. MARK: Absolute Lengths: the cm, mm, Q, in, pt, pc, px units
// https://drafts.csswg.org/css-values/#absolute-lengths

export struct AbsoluteLength : TypedUnitValue<NumericBaseType::LENGTH> {
    constexpr AbsoluteLength(Pixel px)
        : TypedUnitValue(UnitValue{px.value(), UnitValue::Unit::PX}) {}

    constexpr AbsoluteLength(Au value)
        : TypedUnitValue(UnitValue{value.cast<f64>(), UnitValue::Unit::PX}) {}

    constexpr explicit AbsoluteLength(UnitValue value)
        : TypedUnitValue(value) {
        if (not value.absolute())
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
        auto value = try$(parseUnitValue(c, NumericBaseType::LENGTH));
        if (not value.absolute()) {
            c = save;
            return Error::invalidData("expected <absolute-length>");
        }
        return Ok(AbsoluteLength{value});
    }
};

} // namespace Vaev

namespace Vaev::Literals {

export constexpr Pixel operator""_px(unsigned long long val) {
    return Pixel{static_cast<f64>(val)};
}

export constexpr Pixel operator""_px(long double val) {
    return Pixel{static_cast<f64>(val)};
}

} // namespace Vaev::Literals
