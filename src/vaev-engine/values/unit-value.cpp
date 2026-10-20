module;

#include <karm/macros>

export module Vaev.Engine:values.unitValue;

import Karm.Core;

import :css;
import :values.base;
import :values.writing;

using namespace Karm;

namespace Vaev {

// MARK: Numeric Base Type -----------------------------------------------------
// https://drafts.css-houdini.org/css-typed-om-1/#enumdef-cssnumericbasetype

export enum struct NumericBaseType : u8 {
    LENGTH,
    ANGLE,
    TIME,
    FREQUENCY,
    RESOLUTION,
    FLEX,
    PERCENT,

    _LEN,
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

// MARK: Unit Value ------------------------------------------------------------
// https://drafts.css-houdini.org/css-typed-om-1/#cssunitvalue

constexpr Opt<NumericBaseType> _baseType(None) {
    return NONE;
}

constexpr Opt<NumericBaseType> _baseType(NumericBaseType type) {
    return Some(type);
}

export struct UnitValue {
    enum struct Unit : u8 {
#define UNIT(NAME, ...) NAME,
#include "defs/units.inc"
#undef UNIT

        _LEN,
    };

    f64 _value = 0;
    Unit _unit = Unit::NONE;

    static constexpr Opt<NumericBaseType> baseTypeOf(Unit unit) {
        using enum NumericBaseType;
        switch (unit) {
#define UNIT(NAME, TYPE, ...) \
    case Unit::NAME:          \
        return _baseType(TYPE);
#include "defs/units.inc"
#undef UNIT

        default:
            unreachable();
        }
    }

    static constexpr Str nameOf(Unit unit) {
        switch (unit) {
#define UNIT(NAME, TYPE, STR, ...) \
    case Unit::NAME:               \
        return STR;
#include "defs/units.inc"
#undef UNIT

        default:
            unreachable();
        }
    }

    static constexpr f64 factorOf(Unit unit) {
        switch (unit) {
#define UNIT(NAME, TYPE, STR, FACTOR) \
    case Unit::NAME:                  \
        return FACTOR;
#include "defs/units.inc"
#undef UNIT

        default:
            unreachable();
        }
    }

    // https://drafts.csswg.org/css-values/#canonical-unit
    static constexpr Unit canonicalUnitOf(Opt<NumericBaseType> type) {
        if (not type)
            return Unit::NONE;

        switch (*type) {
        case NumericBaseType::LENGTH:
            return Unit::PX;
        case NumericBaseType::ANGLE:
            return Unit::DEG;
        case NumericBaseType::TIME:
            return Unit::S;
        case NumericBaseType::FREQUENCY:
            return Unit::HZ;
        case NumericBaseType::RESOLUTION:
            return Unit::DPPX;
        case NumericBaseType::FLEX:
            return Unit::FR;
        case NumericBaseType::PERCENT:
            return Unit::PERCENT;
        default:
            unreachable();
        }
    }

    static Res<Unit> unitFromName(Str name) {
        for (usize i = 0; i < toUnderlyingType(Unit::_LEN); i++) {
            auto unit = static_cast<Unit>(i);
            if (eqCi(name, nameOf(unit)))
                return Ok(unit);
        }
        return Error::invalidData("unknown unit");
    }

    constexpr UnitValue() = default;

    constexpr UnitValue(f64 value, Unit unit)
        : _value(value), _unit(unit) {}

    constexpr f64 value() const {
        return _value;
    }

    constexpr Unit unit() const {
        return _unit;
    }

    constexpr Opt<NumericBaseType> baseType() const {
        return baseTypeOf(_unit);
    }

    constexpr bool isAbsolute() const {
        return factorOf(_unit) != 0;
    }

    constexpr bool isCanonical() const {
        return canonicalUnitOf(baseType()) == _unit;
    }


    // https://drafts.csswg.org/css-values/#relative-lengths
    UnitValue _resolveRelative(Unit unit, RelativeLengthContext auto const& ctx) const {
        switch (unit) {
        case Unit::EM:
            return {_value * ctx.fontSize, Unit::PX};

        case Unit::REM:
            return {_value * ctx.rootFontSize, Unit::PX};

        case Unit::EX:
            return {_value * ctx.xHeight, Unit::PX};

        case Unit::REX:
            return {_value * ctx.rootXHeight, Unit::PX};

        case Unit::CAP:
            return {_value * ctx.capHeight, Unit::PX};

        case Unit::RCAP:
            return {_value * ctx.rootCapHeight, Unit::PX};

        case Unit::CH:
            return {_value * ctx.zeroAdvance, Unit::PX};

        case Unit::RCH:
            return {_value * ctx.rootZeroAdvance, Unit::PX};

        case Unit::IC:
            return {_value * ctx.zeroAdvance, Unit::PX};

        case Unit::RIC:
            return {_value * ctx.rootZeroAdvance, Unit::PX};

        case Unit::LH:
            return {_value * ctx.lineHeight, Unit::PX};

        case Unit::RLH:
            return {_value * ctx.rootLineHeight, Unit::PX};

        // Viewport-relative

        // https://drafts.csswg.org/css-values/#vw

        // Equal to 1% of the width of current viewport.
        case Unit::VW:
        case Unit::LVW:
            return {_value * ctx.viewportLargeWidth / 100, Unit::PX};

        case Unit::SVW:
            return {_value * ctx.viewportSmallWidth / 100, Unit::PX};

        case Unit::DVW:
            return {_value * ctx.viewportDynamicWidth / 100, Unit::PX};

        // https://drafts.csswg.org/css-values/#vh
        // Equal to 1% of the height of current viewport.
        case Unit::VH:
        case Unit::LVH:
            return {_value * ctx.viewportLargeHeight / 100, Unit::PX};

        case Unit::SVH:
            return {_value * ctx.viewportSmallHeight / 100, Unit::PX};

        case Unit::DVH:
            return {_value * ctx.viewportDynamicHeight / 100, Unit::PX};

        // https://drafts.csswg.org/css-values/#vi
        // Equal to 1% of the size of the viewport in the box’s inline axis.
        case Unit::VI:
        case Unit::LVI:
            if (ctx.boxAxis == Axis::HORIZONTAL) {
                return {_value * ctx.viewportLargeWidth / 100, Unit::PX};
            } else {
                return {_value * ctx.viewportLargeHeight / 100, Unit::PX};
            }

        case Unit::SVI:
            if (ctx.boxAxis == Axis::HORIZONTAL) {
                return {_value * ctx.viewportSmallWidth / 100, Unit::PX};
            } else {
                return {_value * ctx.viewportSmallHeight / 100, Unit::PX};
            }

        case Unit::DVI:
            if (ctx.boxAxis == Axis::HORIZONTAL) {
                return {_value * ctx.viewportDynamicWidth / 100, Unit::PX};
            } else {
                return {_value * ctx.viewportDynamicHeight / 100, Unit::PX};
            }

        // https://drafts.csswg.org/css-values/#vb
        // Equal to 1% of the size of the viewport in the box’s block axis.
        case Unit::VB:
        case Unit::LVB:
            if (ctx.boxAxis.cross() == Axis::HORIZONTAL) {
                return {_value * ctx.viewportLargeWidth / 100, Unit::PX};
            } else {
                return {_value * ctx.viewportLargeHeight / 100, Unit::PX};
            }

        case Unit::SVB:
            if (ctx.boxAxis.cross() == Axis::HORIZONTAL) {
                return {_value * ctx.viewportSmallWidth / 100, Unit::PX};
            } else {
                return {_value * ctx.viewportSmallHeight / 100, Unit::PX};
            }

        case Unit::DVB:
            if (ctx.boxAxis.cross() == Axis::HORIZONTAL) {
                return {_value * ctx.viewportDynamicWidth / 100, Unit::PX};
            } else {
                return {_value * ctx.viewportDynamicHeight / 100, Unit::PX};
            }

        // https://drafts.csswg.org/css-values/#vmin
        // Equal to the smaller of vw and vh.
        case Unit::VMIN:
        case Unit::LVMIN:
            return min(
                _resolveRelative(Unit::VW, ctx),
                _resolveRelative(Unit::VH, ctx)
            );

        case Unit::SVMIN:
            return min(
                _resolveRelative(Unit::SVW, ctx),
                _resolveRelative(Unit::SVH, ctx)
            );

        case Unit::DVMIN:
            return min(
                _resolveRelative(Unit::DVW, ctx),
                _resolveRelative(Unit::DVH, ctx)
            );

        // https://drafts.csswg.org/css-values/#vmax
        // Equal to the larger of vw and vh.
        case Unit::VMAX:
        case Unit::LVMAX:
            return max(
                _resolveRelative(Unit::VW, ctx),
                _resolveRelative(Unit::VH, ctx)
            );

        case Unit::DVMAX:
            return max(
                _resolveRelative(Unit::DVW, ctx),
                _resolveRelative(Unit::DVH, ctx)
            );

        case Unit::SVMAX:
            return max(
                _resolveRelative(Unit::SVW, ctx),
                _resolveRelative(Unit::SVH, ctx)
            );
        default:
            unreachable();
        }
    }

    constexpr UnitValue canonical(LengthContext auto const& ctx) const {
        if (baseType() == NumericBaseType::LENGTH and not isAbsolute())
            return _resolveRelative(_unit, ctx);

        if (not isAbsolute())
            panic("unit value depends on a context");

        return {_value * factorOf(_unit), canonicalUnitOf(baseType())};
    }

    constexpr UnitValue canonical(None = NONE) const {
        if (not isAbsolute())
            panic("unit value depends on a context");
        return {_value * factorOf(_unit), canonicalUnitOf(baseType())};
    }

    constexpr f64 to(Unit unit) const {
        if (baseTypeOf(unit) != baseType())
            panic("converting between unit values of different types");
        return canonical().value() / factorOf(unit);
    }

    // https://drafts.css-houdini.org/css-typed-om-1/#equal-numeric-value
    // If value1 and value2 are both CSSUnitValues, return true if they have equal unit and value internal slots, or false otherwise.
    constexpr bool operator==(UnitValue const&) const = default;

    constexpr std::partial_ordering operator<=>(UnitValue const& other) const {
        if (_unit == other._unit)
            return _value <=> other._value;
        if (baseType() != other.baseType() or not isAbsolute() or not other.isAbsolute())
            return std::partial_ordering::unordered;
        return canonical().value() <=> other.canonical().value();
    }

    constexpr UnitValue toComputed() const {
        if (baseType() == NumericBaseType::LENGTH) {
            // FIXME: Resolve relative units
            return canonical();
        } else {
            return canonical();
        }
    }

    void repr(Io::Emit& e) const {
        e("{}{}", _value, nameOf(_unit));
    }
};

export template <>
struct ValueParser<UnitValue> {
    static Res<UnitValue> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::NUMBER) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(UnitValue{Io::atof(scan).unwrapOr(0.0), UnitValue::Unit::NONE});
        }

        if (c.peek() == Css::Token::PERCENTAGE) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(UnitValue{Io::atof(scan).unwrapOr(0.0), UnitValue::Unit::PERCENT});
        }

        if (c.peek() == Css::Token::DIMENSION) {
            Io::SScan scan = c->token.data.str();
            auto value = Io::atof(scan, {.allowExp = false}).unwrapOr(0.0);
            auto unit = try$(UnitValue::unitFromName(scan.remStr()));
            c.next();
            return Ok(UnitValue{value, unit});
        }

        return Error::invalidData("expected numeric value");
    }
};

export template <>
struct ValueSerializer<UnitValue> {
    // https://drafts.csswg.org/cssom/#serialize-a-css-component-value
    static Res<> serialize(Io::TextWriter& w, UnitValue const& value) {
        // The <number> component serialized as per <number> followed by the unit in its canonical form as defined in its respective specification.
        try$(Css::serializeNumber(w, value.value()));
        return w.writeStr(UnitValue::nameOf(value.unit()));
    }
};

export Res<UnitValue> parseUnitValue(Cursor<Css::Sst>& c, NumericBaseType type) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    // https://drafts.csswg.org/css-values/#lengths
    // For zero lengths the unit identifier is optional
    if (type == NumericBaseType::LENGTH and c.skip(Css::Token::number("0")))
        return Ok(UnitValue{0, UnitValue::Unit::PX});

    auto save = c;
    auto value = try$(parseValue<UnitValue>(c));
    if (value.baseType() != type) {
        c = save;
        return Error::invalidData("unexpected numeric base type");
    }
    return Ok(value);
}

// MARK: Typed Unit Value ------------------------------------------------------

enum struct AcceptsPercent {
    NO,
    YES,
    _LEN,
};

export template <NumericBaseType TYPE, AcceptsPercent WITH_PERCENT = AcceptsPercent::NO>
struct TypedUnitValue {
    static constexpr NumericBaseType BASE_TYPE = TYPE;
    static constexpr AcceptsPercent ACCEPTS_PERCENTS = WITH_PERCENT;

    UnitValue _value;

    constexpr TypedUnitValue()
        : _value(0, UnitValue::canonicalUnitOf(Some(TYPE))) {}

    constexpr explicit TypedUnitValue(UnitValue value)
        : _value(value) {

        if (not (value.baseType() == TYPE or (ACCEPTS_PERCENTS == AcceptsPercent::YES and value.baseType() == NumericBaseType::PERCENT)))
            panic("unit value has the wrong base type");
    }

    constexpr UnitValue unitValue() const {
        return _value;
    }

    constexpr auto operator<=>(TypedUnitValue const&) const = default;

    void repr(Io::Emit& e) const {
        _value.repr(e);
    }
};

export template <typename T>
    requires Meta::Derive<T, TypedUnitValue<T::BASE_TYPE, T::ACCEPTS_PERCENTS>>
struct ValueParser<T> {
    static Res<T> parse(Cursor<Css::Sst>& c) {
        auto maybeBase = parseUnitValue(c, T::BASE_TYPE);

        if (maybeBase) {
            return Ok(T{maybeBase.take()});
        } else if (T::ACCEPTS_PERCENTS == AcceptsPercent::YES) {
            return Ok(T{try$(parseUnitValue(c, NumericBaseType::PERCENT))});
        } else {
            return *maybeBase.error();
        }
    }
};

export template <typename T>
    requires Meta::Derive<T, TypedUnitValue<T::BASE_TYPE>>
struct ValueSerializer<T> {
    static Res<> serialize(Io::TextWriter& w, T const& value) {
        return serializeValue(w, value.unitValue());
    }
};

} // namespace Vaev
