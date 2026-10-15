module;

#include <karm/macros>

export module Vaev.Engine:values.length;

import Karm.Core;
import Karm.Math;

import :css;
import :values.base;
import :values.dimension;
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

export struct Length {
    static constexpr auto TYPE = Dimension::LENGTH;

    Dimension _dim;

    constexpr Length()
        : _dim(0, Dimension::Unit::PX) {}

    constexpr Length(Au value)
        : _dim(value.cast<f64>(), Dimension::Unit::PX) {}

    constexpr Length(f64 value, Dimension::Unit unit)
        : _dim(value, unit) {}

    constexpr explicit Length(Dimension dim)
        : _dim(dim) {}

    constexpr f64 value() const {
        return _dim.value();
    }

    constexpr Dimension::Unit unit() const {
        return _dim.unit();
    }

    // https://drafts.csswg.org/css-values/#absolute-lengths
    constexpr bool isAbsolute() const {
        return _dim.isAbsolute();
    }

    constexpr bool operator==(Length const&) const = default;

    constexpr std::partial_ordering operator<=>(Length const& other) const {
        return _dim <=> other._dim;
    }

    void repr(Io::Emit& e) const {
        e("{}", _dim);
    }
};

export template <>
struct _Resolved<Length> {
    using Type = Au;
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
Au _resolveRelative(f64 value, Dimension::Unit unit, RelativeLengthContext auto const& ctx) {
    switch (unit) {
    case Dimension::Unit::EM:
        return Au::fromFloatNearest(value * ctx.fontSize);

    case Dimension::Unit::REM:
        return Au::fromFloatNearest(value * ctx.rootFontSize);

    case Dimension::Unit::EX:
        return Au::fromFloatNearest(value * ctx.xHeight);

    case Dimension::Unit::REX:
        return Au::fromFloatNearest(value * ctx.rootXHeight);

    case Dimension::Unit::CAP:
        return Au::fromFloatNearest(value * ctx.capHeight);

    case Dimension::Unit::RCAP:
        return Au::fromFloatNearest(value * ctx.rootCapHeight);

    case Dimension::Unit::CH:
        return Au::fromFloatNearest(value * ctx.zeroAdvance);

    case Dimension::Unit::RCH:
        return Au::fromFloatNearest(value * ctx.rootZeroAdvance);

    case Dimension::Unit::IC:
        return Au::fromFloatNearest(value * ctx.zeroAdvance);

    case Dimension::Unit::RIC:
        return Au::fromFloatNearest(value * ctx.rootZeroAdvance);

    case Dimension::Unit::LH:
        return Au::fromFloatNearest(value * ctx.lineHeight);

    case Dimension::Unit::RLH:
        return Au::fromFloatNearest(value * ctx.rootLineHeight);
    // Viewport-relative

    // https://drafts.csswg.org/css-values/#vw

    // Equal to 1% of the width of current viewport.
    case Dimension::Unit::VW:
    case Dimension::Unit::LVW:
        return Au::fromFloatNearest(value * ctx.viewportLargeWidth / 100);

    case Dimension::Unit::SVW:
        return Au::fromFloatNearest(value * ctx.viewportSmallWidth / 100);

    case Dimension::Unit::DVW:
        return Au::fromFloatNearest(value * ctx.viewportDynamicWidth / 100);

    // https://drafts.csswg.org/css-values/#vh
    // Equal to 1% of the height of current viewport.
    case Dimension::Unit::VH:
    case Dimension::Unit::LVH:
        return Au::fromFloatNearest(value * ctx.viewportLargeHeight / 100);

    case Dimension::Unit::SVH:
        return Au::fromFloatNearest(value * ctx.viewportSmallHeight / 100);

    case Dimension::Unit::DVH:
        return Au::fromFloatNearest(value * ctx.viewportDynamicHeight / 100);

    // https://drafts.csswg.org/css-values/#vi
    // Equal to 1% of the size of the viewport in the box’s inline axis.
    case Dimension::Unit::VI:
    case Dimension::Unit::LVI:
        if (ctx.boxAxis == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value * ctx.viewportLargeWidth / 100);
        } else {
            return Au::fromFloatNearest(value * ctx.viewportLargeHeight / 100);
        }

    case Dimension::Unit::SVI:
        if (ctx.boxAxis == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value * ctx.viewportSmallWidth / 100);
        } else {
            return Au::fromFloatNearest(value * ctx.viewportSmallHeight / 100);
        }

    case Dimension::Unit::DVI:
        if (ctx.boxAxis == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value * ctx.viewportDynamicWidth / 100);
        } else {
            return Au::fromFloatNearest(value * ctx.viewportDynamicHeight / 100);
        }

    // https://drafts.csswg.org/css-values/#vb
    // Equal to 1% of the size of the viewport in the box’s block axis.
    case Dimension::Unit::VB:
    case Dimension::Unit::LVB:
        if (ctx.boxAxis.cross() == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value * ctx.viewportLargeWidth / 100);
        } else {
            return Au::fromFloatNearest(value * ctx.viewportLargeHeight / 100);
        }

    case Dimension::Unit::SVB:
        if (ctx.boxAxis.cross() == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value * ctx.viewportSmallWidth / 100);
        } else {
            return Au::fromFloatNearest(value * ctx.viewportSmallHeight / 100);
        }

    case Dimension::Unit::DVB:
        if (ctx.boxAxis.cross() == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value * ctx.viewportDynamicWidth / 100);
        } else {
            return Au::fromFloatNearest(value * ctx.viewportDynamicHeight / 100);
        }

    // https://drafts.csswg.org/css-values/#vmin
    // Equal to the smaller of vw and vh.
    case Dimension::Unit::VMIN:
    case Dimension::Unit::LVMIN:
        return min(
            _resolveRelative(value, Dimension::Unit::VW, ctx),
            _resolveRelative(value, Dimension::Unit::VH, ctx)
        );

    case Dimension::Unit::SVMIN:
        return min(
            _resolveRelative(value, Dimension::Unit::SVW, ctx),
            _resolveRelative(value, Dimension::Unit::SVH, ctx)
        );

    case Dimension::Unit::DVMIN:
        return min(
            _resolveRelative(value, Dimension::Unit::DVW, ctx),
            _resolveRelative(value, Dimension::Unit::DVH, ctx)
        );

    // https://drafts.csswg.org/css-values/#vmax
    // Equal to the larger of vw and vh.
    case Dimension::Unit::VMAX:
    case Dimension::Unit::LVMAX:
        return max(
            _resolveRelative(value, Dimension::Unit::VW, ctx),
            _resolveRelative(value, Dimension::Unit::VH, ctx)
        );

    case Dimension::Unit::DVMAX:
        return max(
            _resolveRelative(value, Dimension::Unit::DVW, ctx),
            _resolveRelative(value, Dimension::Unit::DVH, ctx)
        );

    case Dimension::Unit::SVMAX:
        return max(
            _resolveRelative(value, Dimension::Unit::SVW, ctx),
            _resolveRelative(value, Dimension::Unit::SVH, ctx)
        );
    default:
        unreachable();
    }
}

export Au resolve(Length const& value, LengthContext auto const& ctx) {
    if (value.isAbsolute())
        return Au::fromFloatNearest(value._dim.to(Dimension::Unit::PX));
    return _resolveRelative(value.value(), value.unit(), ctx);
}

// https://drafts.csswg.org/css-values/#lengths
// The computed value of a <length> is an absolute length.
export template <>
struct _Computed<Length> {
    using Type = Au;
};

export Au compute(Length const& l, LengthContext auto const& ctx) {
    return resolve(l, ctx);
}

export template <>
struct _Specify<Length> {
    static Length from(Au value) {
        return value;
    }
};

// A computed length is stored as a dimension in px
export template <>
struct _Dimension<Au> {
    static constexpr Dimension::Type TYPE = Dimension::LENGTH;

    static Dimension to(Au value) {
        return {value.cast<f64>(), Dimension::Unit::PX};
    }

    static Au from(Dimension dim) {
        return Au::fromFloatNearest(dim.to(Dimension::Unit::PX));
    }
};

export template <>
struct ValueParser<Length> {
    static Res<Length> parse(Cursor<Css::Sst>& c) {
        return Ok(Length{try$(parseDimension(c, Length::TYPE))});
    }
};

export template <>
struct ValueSerializer<Length> {
    static Res<> serialize(Io::TextWriter& w, Length const& value) {
        return serializeValue(w, value._dim);
    }
};

// 6.2. MARK: Absolute Lengths: the cm, mm, Q, in, pt, pc, px units
// https://drafts.csswg.org/css-values/#absolute-lengths

// A length that doesn't depend on any context.
export struct AbsoluteLength {
    static constexpr auto TYPE = Dimension::LENGTH;

    Dimension _dim;

    constexpr AbsoluteLength(Pixel px)
        : _dim(px.value(), Dimension::Unit::PX) {}

    constexpr AbsoluteLength(Au value)
        : _dim(value.cast<f64>(), Dimension::Unit::PX) {}

    constexpr explicit AbsoluteLength(Dimension dim)
        : _dim(dim) {}

    constexpr Pixel pixels() const {
        return Pixel{_dim.to(Dimension::Unit::PX)};
    }

    constexpr operator Length() const {
        return Length{_dim};
    }

    constexpr bool operator==(AbsoluteLength const& other) const {
        return _dim == other._dim;
    }

    constexpr auto operator<=>(AbsoluteLength const& other) const {
        return _dim <=> other._dim;
    }

    void repr(Io::Emit& e) const {
        e("{}", _dim);
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
        auto dim = try$(parseDimension(c, AbsoluteLength::TYPE));
        if (not dim.isAbsolute()) {
            c = save;
            return Error::invalidData("expected <absolute-length>");
        }
        return Ok(AbsoluteLength{dim});
    }
};

export template <>
struct ValueSerializer<AbsoluteLength> {
    static Res<> serialize(Io::TextWriter& w, AbsoluteLength const& value) {
        return serializeValue(w, value._dim);
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
