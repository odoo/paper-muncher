module;

#include <karm/macros>

export module Vaev.Engine:values.length;

import Karm.Core;
import Karm.Math;

import :css;
import :values.base;
import :values.resolved;
import :values.writing;

using namespace Karm;

namespace Vaev {

export using Math::Au;
export constexpr Au INDEFINITE = Limits<Au>::MAX;

export using Math::InsetsAu;
export using Math::RectAu;
export using Math::Vec2Au;
export using Math::RadiiAu;
export using Math::EllipseAu;

export using Pixel = Distinct<f64, struct _PixelTag>;

// 6.1. MARK: Relative Lengths
// https://drafts.csswg.org/css-values/#relative-lengths

export struct RelativeLength {
    enum struct Unit {
#define LENGTH(NAME, ...) NAME,
#include "defs/relative-lengths.inc"

#undef LENGTH
        _LEN,
    };

    using enum Unit;

    float _value;
    Unit _unit;

    RelativeLength(f64 value, Unit unit)
        : _value(value), _unit(unit) {}

    constexpr f64 value() const {
        return _value;
    }

    constexpr Unit unit() const {
        return _unit;
    }

    constexpr bool operator==(RelativeLength const& other) const {
        return _value == other._value and _unit == other._unit;
    }

    constexpr std::partial_ordering operator<=>(RelativeLength const& other) const {
        if (_unit != other._unit)
            return std::partial_ordering::unordered;
        return _value <=> other._value;
    }

    void repr(Io::Emit& e) const {
        e("{}{}", _value, _unit);
    }
};

export template <>
struct _Resolved<RelativeLength> {
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

export Au resolve(RelativeLength value, RelativeLengthContext auto const& ctx) {
    switch (value.unit()) {
    case RelativeLength::EM:
        return Au::fromFloatNearest(value.value() * ctx.fontSize);

    case RelativeLength::REM:
        return Au::fromFloatNearest(value.value() * ctx.rootFontSize);

    case RelativeLength::EX:
        return Au::fromFloatNearest(value.value() * ctx.xHeight);

    case RelativeLength::REX:
        return Au::fromFloatNearest(value.value() * ctx.rootXHeight);

    case RelativeLength::CAP:
        return Au::fromFloatNearest(value.value() * ctx.capHeight);

    case RelativeLength::RCAP:
        return Au::fromFloatNearest(value.value() * ctx.rootCapHeight);

    case RelativeLength::CH:
        return Au::fromFloatNearest(value.value() * ctx.zeroAdvance);

    case RelativeLength::RCH:
        return Au::fromFloatNearest(value.value() * ctx.rootZeroAdvance);

    case RelativeLength::IC:
        return Au::fromFloatNearest(value.value() * ctx.zeroAdvance);

    case RelativeLength::RIC:
        return Au::fromFloatNearest(value.value() * ctx.rootZeroAdvance);

    case RelativeLength::LH:
        return Au::fromFloatNearest(value.value() * ctx.lineHeight);

    case RelativeLength::RLH:
        return Au::fromFloatNearest(value.value() * ctx.rootLineHeight);
    // Viewport-relative

    // https://drafts.csswg.org/css-values/#vw

    // Equal to 1% of the width of current viewport.
    case RelativeLength::VW:
    case RelativeLength::LVW:
        return Au::fromFloatNearest(value.value() * ctx.viewportLargeWidth / 100);

    case RelativeLength::SVW:
        return Au::fromFloatNearest(value.value() * ctx.viewportSmallWidth / 100);

    case RelativeLength::DVW:
        return Au::fromFloatNearest(value.value() * ctx.viewportDynamicWidth / 100);

    // https://drafts.csswg.org/css-values/#vh
    // Equal to 1% of the height of current viewport.
    case RelativeLength::VH:
    case RelativeLength::LVH:
        return Au::fromFloatNearest(value.value() * ctx.viewportLargeHeight / 100);

    case RelativeLength::SVH:
        return Au::fromFloatNearest(value.value() * ctx.viewportSmallHeight / 100);

    case RelativeLength::DVH:
        return Au::fromFloatNearest(value.value() * ctx.viewportDynamicHeight / 100);

    // https://drafts.csswg.org/css-values/#vi
    // Equal to 1% of the size of the viewport in the box’s inline axis.
    case RelativeLength::VI:
    case RelativeLength::LVI:
        if (ctx.boxAxis == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value.value() * ctx.viewportLargeWidth / 100);
        } else {
            return Au::fromFloatNearest(value.value() * ctx.viewportLargeHeight / 100);
        }

    case RelativeLength::SVI:
        if (ctx.boxAxis == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value.value() * ctx.viewportSmallWidth / 100);
        } else {
            return Au::fromFloatNearest(value.value() * ctx.viewportSmallHeight / 100);
        }

    case RelativeLength::DVI:
        if (ctx.boxAxis == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value.value() * ctx.viewportDynamicWidth / 100);
        } else {
            return Au::fromFloatNearest(value.value() * ctx.viewportDynamicHeight / 100);
        }

    // https://drafts.csswg.org/css-values/#vb
    // Equal to 1% of the size of the viewport in the box’s block axis.
    case RelativeLength::VB:
    case RelativeLength::LVB:
        if (ctx.boxAxis.cross() == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value.value() * ctx.viewportLargeWidth / 100);
        } else {
            return Au::fromFloatNearest(value.value() * ctx.viewportLargeHeight / 100);
        }

    case RelativeLength::SVB:
        if (ctx.boxAxis.cross() == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value.value() * ctx.viewportSmallWidth / 100);
        } else {
            return Au::fromFloatNearest(value.value() * ctx.viewportSmallHeight / 100);
        }

    case RelativeLength::DVB:
        if (ctx.boxAxis.cross() == Axis::HORIZONTAL) {
            return Au::fromFloatNearest(value.value() * ctx.viewportDynamicWidth / 100);
        } else {
            return Au::fromFloatNearest(value.value() * ctx.viewportDynamicHeight / 100);
        }

    // https://drafts.csswg.org/css-values/#vmin
    // Equal to the smaller of vw and vh.
    case RelativeLength::VMIN:
    case RelativeLength::LVMIN:
        return min(
            resolve(RelativeLength(value.value(), RelativeLength::VW), ctx),
            resolve(RelativeLength(value.value(), RelativeLength::VH), ctx)
        );

    case RelativeLength::SVMIN:
        return min(
            resolve(RelativeLength(value.value(), RelativeLength::SVW), ctx),
            resolve(RelativeLength(value.value(), RelativeLength::SVH), ctx)
        );

    case RelativeLength::DVMIN:
        return min(
            resolve(RelativeLength(value.value(), RelativeLength::DVW), ctx),
            resolve(RelativeLength(value.value(), RelativeLength::DVH), ctx)
        );

    // https://drafts.csswg.org/css-values/#vmax
    // Equal to the larger of vw and vh.
    case RelativeLength::VMAX:
    case RelativeLength::LVMAX:
        return max(
            resolve(RelativeLength(value.value(), RelativeLength::VW), ctx),
            resolve(RelativeLength(value.value(), RelativeLength::VH), ctx)
        );

    case RelativeLength::DVMAX:
        return max(
            resolve(RelativeLength(value.value(), RelativeLength::DVW), ctx),
            resolve(RelativeLength(value.value(), RelativeLength::DVH), ctx)
        );

    case RelativeLength::SVMAX:
        return max(
            resolve(RelativeLength(value.value(), RelativeLength::SVW), ctx),
            resolve(RelativeLength(value.value(), RelativeLength::SVH), ctx)
        );
    default:
        unreachable();
    }
}

export template <>
struct ValueParser<RelativeLength> {
    static Res<RelativeLength::Unit> _parseLengthUnit(Str unit) {
        return valueOfCi<RelativeLength::Unit>(unit)
            .okOr(Error::invalidData("absolute length unit"));
    }

    static Res<RelativeLength> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() != Css::Token::DIMENSION)
            return Error::invalidData("expected <relative-length>");

        Io::SScan scan = c->token.data.str();
        auto value = Io::atof(scan, {.allowExp = false}).unwrapOr(0.0);
        auto unit = try$(_parseLengthUnit(scan.remStr()));
        c.next();

        return Ok(RelativeLength{value, unit});
    }
};

// 6.2. MARK: Absolute Lengths: the cm, mm, Q, in, pt, pc, px units
// https://drafts.csswg.org/css-values/#absolute-lengths

export struct AbsoluteLength {
    enum struct Unit {
#define LENGTH(NAME, ...) NAME,
#include "defs/absolute-lengths.inc"

#undef LENGTH
        _LEN,
    };

    using enum Unit;

    float _value;
    Unit _unit;

    AbsoluteLength(Pixel px)
        : AbsoluteLength(px.value(), PX) {}

    AbsoluteLength(Au au)
        : AbsoluteLength(static_cast<f64>(au), PX) {}

    AbsoluteLength(f64 value, Unit unit)
        : _value(value), _unit(unit) {}

    constexpr f64 value() const {
        return _value;
    }

    constexpr Unit unit() const {
        return _unit;
    }

    constexpr Pixel pixels() const {
        switch (_unit) {
        case CM:
            return Pixel(_value * 96 / 2.54);

        case MM:
            return Pixel(_value * 96 / 25.4);

        case Q:
            return Pixel(_value * 96 / 101.6);

        case IN:
            return Pixel(_value * 96);

        case PT:
            return Pixel(_value * 96 / 72.0);

        case PC:
            return Pixel(_value * 96 / 6.0);

        case PX:
            return Pixel(_value);
        default:
            unreachable();
        }
    }

    constexpr bool operator==(AbsoluteLength const& other) const {
        return pixels() == other.pixels();
    }

    constexpr auto operator<=>(AbsoluteLength const& other) const {
        return pixels() <=> other.pixels();
    }

    void repr(Io::Emit& e) const {
        e("{}{}", _value, _unit);
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
    static Res<AbsoluteLength::Unit> _parseLengthUnit(Str unit) {
        return valueOfCi<AbsoluteLength::Unit>(unit)
            .okOr(Error::invalidData("expected <absolute-length>"));
    }

    static Res<AbsoluteLength> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() != Css::Token::DIMENSION)
            return Error::invalidData("expected <absolute-length>");

        Io::SScan scan = c->token.data.str();
        auto value = Io::atof(scan, {.allowExp = false}).unwrapOr(0.0);
        auto unit = try$(_parseLengthUnit(scan.remStr()));
        c.next();

        return Ok(AbsoluteLength{value, unit});
    }
};

// 6. MARK: Distance Units: the <length> type
// https://drafts.csswg.org/css-values/#lengths

using Length = Union<AbsoluteLength, RelativeLength>;

export template <>
struct _Resolved<Length> {
    using Type = Au;
};

export template <typename T>
concept LengthContext = RelativeLengthContext<T>;

export Au resolve(Length const& l, LengthContext auto const& ctx) {
    return l.visit(
        [&](RelativeLength l) {
            return resolve(l, ctx);
        },
        [](AbsoluteLength l) {
            return resolve(l);
        }
    );
}

export template <>
struct ValueParser<Length> {
    static Res<Length> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.skip(Css::Token::number("0")))
            return Ok(AbsoluteLength{0.0, AbsoluteLength::PX});

        if (c.peek() != Css::Token::DIMENSION)
            return Error::invalidData("expected <length>");

        if (auto v = parseValue<AbsoluteLength>(c))
            return v;

        if (auto v = parseValue<RelativeLength>(c))
            return v;

        return Error::invalidData("expected <length>");
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