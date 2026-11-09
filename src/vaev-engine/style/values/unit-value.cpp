export module Vaev.Engine:style.values.unitValue;

import Karm.Core;

import :css.values.unitValue;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.css-houdini.org/css-typed-om-1/#cssunitvalue
export using Css::NumericBaseType;
export using Css::UnitValue;

// MARK: Relative Lengths ------------------------------------------------------
// https://drafts.csswg.org/css-values/#relative-lengths

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

    // https://drafts.csswg.org/css-writing-modes-4/#inline-axis
    // Whether the box's inline axis is horizontal, for the vi/vb units.
    // FIXME: Always true, the box's writing-mode isn't wired in yet.
    bool horizontalWritingMode = true;
};

export template <typename T>
concept LengthContext = requires(T t) {
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

    // Writing mode of the box
    { t.horizontalWritingMode } -> Meta::Convertible<bool>;
};


// https://drafts.csswg.org/css-values/#relative-lengths
UnitValue _resolveRelative(UnitValue const& value, LengthContext auto const& ctx) {
    using Unit = UnitValue::Unit;
    auto n = value.value();
    switch (value.unit()) {
    case Unit::EM:
        return {n * ctx.fontSize, Unit::PX};

    case Unit::REM:
        return {n * ctx.rootFontSize, Unit::PX};

    case Unit::EX:
        return {n * ctx.xHeight, Unit::PX};

    case Unit::REX:
        return {n * ctx.rootXHeight, Unit::PX};

    case Unit::CAP:
        return {n * ctx.capHeight, Unit::PX};

    case Unit::RCAP:
        return {n * ctx.rootCapHeight, Unit::PX};

    case Unit::CH:
        return {n * ctx.zeroAdvance, Unit::PX};

    case Unit::RCH:
        return {n * ctx.rootZeroAdvance, Unit::PX};

    case Unit::IC:
        return {n * ctx.zeroAdvance, Unit::PX};

    case Unit::RIC:
        return {n * ctx.rootZeroAdvance, Unit::PX};

    case Unit::LH:
        return {n * ctx.lineHeight, Unit::PX};

    case Unit::RLH:
        return {n * ctx.rootLineHeight, Unit::PX};

    // Viewport-relative

    // https://drafts.csswg.org/css-values/#vw

    // Equal to 1% of the width of current viewport.
    case Unit::VW:
    case Unit::LVW:
        return {n * ctx.viewportLargeWidth / 100, Unit::PX};

    case Unit::SVW:
        return {n * ctx.viewportSmallWidth / 100, Unit::PX};

    case Unit::DVW:
        return {n * ctx.viewportDynamicWidth / 100, Unit::PX};

    // https://drafts.csswg.org/css-values/#vh
    // Equal to 1% of the height of current viewport.
    case Unit::VH:
    case Unit::LVH:
        return {n * ctx.viewportLargeHeight / 100, Unit::PX};

    case Unit::SVH:
        return {n * ctx.viewportSmallHeight / 100, Unit::PX};

    case Unit::DVH:
        return {n * ctx.viewportDynamicHeight / 100, Unit::PX};

    // https://drafts.csswg.org/css-values/#vi
    // Equal to 1% of the size of the viewport in the box’s inline axis.
    case Unit::VI:
    case Unit::LVI:
        if (ctx.horizontalWritingMode) {
            return {n * ctx.viewportLargeWidth / 100, Unit::PX};
        } else {
            return {n * ctx.viewportLargeHeight / 100, Unit::PX};
        }

    case Unit::SVI:
        if (ctx.horizontalWritingMode) {
            return {n * ctx.viewportSmallWidth / 100, Unit::PX};
        } else {
            return {n * ctx.viewportSmallHeight / 100, Unit::PX};
        }

    case Unit::DVI:
        if (ctx.horizontalWritingMode) {
            return {n * ctx.viewportDynamicWidth / 100, Unit::PX};
        } else {
            return {n * ctx.viewportDynamicHeight / 100, Unit::PX};
        }

    // https://drafts.csswg.org/css-values/#vb
    // Equal to 1% of the size of the viewport in the box’s block axis.
    case Unit::VB:
    case Unit::LVB:
        if (not ctx.horizontalWritingMode) {
            return {n * ctx.viewportLargeWidth / 100, Unit::PX};
        } else {
            return {n * ctx.viewportLargeHeight / 100, Unit::PX};
        }

    case Unit::SVB:
        if (not ctx.horizontalWritingMode) {
            return {n * ctx.viewportSmallWidth / 100, Unit::PX};
        } else {
            return {n * ctx.viewportSmallHeight / 100, Unit::PX};
        }

    case Unit::DVB:
        if (not ctx.horizontalWritingMode) {
            return {n * ctx.viewportDynamicWidth / 100, Unit::PX};
        } else {
            return {n * ctx.viewportDynamicHeight / 100, Unit::PX};
        }

    // https://drafts.csswg.org/css-values/#vmin
    // Equal to the smaller of vw and vh.
    case Unit::VMIN:
    case Unit::LVMIN:
        return min(
            _resolveRelative(UnitValue{n, Unit::VW}, ctx),
            _resolveRelative(UnitValue{n, Unit::VH}, ctx)
        );

    case Unit::SVMIN:
        return min(
            _resolveRelative(UnitValue{n, Unit::SVW}, ctx),
            _resolveRelative(UnitValue{n, Unit::SVH}, ctx)
        );

    case Unit::DVMIN:
        return min(
            _resolveRelative(UnitValue{n, Unit::DVW}, ctx),
            _resolveRelative(UnitValue{n, Unit::DVH}, ctx)
        );

    // https://drafts.csswg.org/css-values/#vmax
    // Equal to the larger of vw and vh.
    case Unit::VMAX:
    case Unit::LVMAX:
        return max(
            _resolveRelative(UnitValue{n, Unit::VW}, ctx),
            _resolveRelative(UnitValue{n, Unit::VH}, ctx)
        );

    case Unit::DVMAX:
        return max(
            _resolveRelative(UnitValue{n, Unit::DVW}, ctx),
            _resolveRelative(UnitValue{n, Unit::DVH}, ctx)
        );

    case Unit::SVMAX:
        return max(
            _resolveRelative(UnitValue{n, Unit::SVW}, ctx),
            _resolveRelative(UnitValue{n, Unit::SVH}, ctx)
        );
    default:
        unreachable();
    }
}

// https://drafts.csswg.org/css-values-4/#simplify-a-calculation-tree
export UnitValue simplify(UnitValue const& value, LengthContext auto const& ctx) {
    if (value.baseType() == NumericBaseType::LENGTH and not value.isAbsolute())
        return _resolveRelative(value, ctx);
    return value.canonical();
}

// https://drafts.csswg.org/css-values-4/#simplify-a-calculation-tree
export constexpr UnitValue simplify(UnitValue const& value, None = NONE) {
    if (not value.isAbsolute())
        return value;
    return value.canonical();
}

} // namespace Vaev::Style
