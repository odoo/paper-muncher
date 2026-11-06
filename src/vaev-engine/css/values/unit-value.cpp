module;

#include <karm/macros>

export module Vaev.Engine:css.values.unitValue;

import Karm.Core;
import Karm.Math;

import :css.lexer;
import :css.parser;
import :css.serializer;
import :css.values.base;
import :css.values.keywords;
import :css.values.primitives;
import :values.unitValue;

using namespace Karm;

namespace Vaev::Css {

export template <>
struct ValueParser<UnitValue> {
    static Res<UnitValue> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Token::NUMBER) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(UnitValue{Io::atof(scan).unwrapOr(0.0), UnitValue::Unit::NONE});
        }

        if (c.peek() == Token::PERCENTAGE) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(UnitValue{Io::atof(scan).unwrapOr(0.0), UnitValue::Unit::PERCENT});
        }

        if (c.peek() == Token::DIMENSION) {
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
        try$(serializeNumber(w, value.value()));
        return w.writeStr(UnitValue::nameOf(value.unit()));
    }
};

export Res<UnitValue> parseUnitValue(Cursor<Sst>& c, Opt<NumericBaseType> type) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    // https://drafts.csswg.org/css-values/#lengths
    // For zero lengths the unit identifier is optional
    if (type == NumericBaseType::LENGTH and c.skip(Token::number("0")))
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

export template <Opt<NumericBaseType> TYPE, AcceptsPercent WITH_PERCENT = AcceptsPercent::NO>
struct TypedUnitValue {
    static constexpr Opt<NumericBaseType> BASE_TYPE = TYPE;
    static constexpr AcceptsPercent ACCEPTS_PERCENTS = WITH_PERCENT;

    UnitValue _value;

    constexpr TypedUnitValue()
        : _value(0, UnitValue::canonicalUnitOf(TYPE)) {}

    constexpr explicit TypedUnitValue(UnitValue value)
        : _value(value) {

        if (not (value.baseType() == TYPE or (WITH_PERCENT == AcceptsPercent::YES and value.baseType() == NumericBaseType::PERCENT)))
            panic("unit value has the wrong base type");
    }

    constexpr UnitValue unitValue() const {
        return _value;
    }

    constexpr f64 value() const {
        return _value.value();
    }

    constexpr UnitValue::Unit unit() const {
        return _value.unit();
    }

    constexpr auto operator<=>(TypedUnitValue const&) const = default;

    void repr(Io::Emit& e) const {
        if constexpr (TYPE == NumericBaseType::RESOLUTION) {
            if (_value.value() == Math::INF) {
                e("infinite");
                return;
            }
        }
        _value.repr(e);
    }

    // MARK: <length> ----------------------------------------------------------
    // https://drafts.csswg.org/css-values/#lengths

    static constexpr TypedUnitValue fromPx(f64 val)
        requires(TYPE == NumericBaseType::LENGTH)
    {
        return TypedUnitValue{UnitValue{val, UnitValue::Unit::PX}};
    }

    static constexpr TypedUnitValue fromEm(f64 val)
        requires(TYPE == NumericBaseType::LENGTH)
    {
        return TypedUnitValue{UnitValue{val, UnitValue::Unit::EM}};
    }

    // https://drafts.csswg.org/css-values/#absolute-lengths
    constexpr bool absolute() const
        requires(TYPE == NumericBaseType::LENGTH)
    {
        return _value.isAbsolute();
    }

    constexpr Opt<f64> absolutePixels() const
        requires(TYPE == NumericBaseType::LENGTH)
    {
        if (not absolute())
            return NONE;
        return Some(_value.to(UnitValue::Unit::PX));
    }

    // MARK: <angle> -----------------------------------------------------------
    // https://drafts.csswg.org/css-values/#angles

    static constexpr TypedUnitValue fromDegree(f64 val)
        requires(TYPE == NumericBaseType::ANGLE)
    {
        return TypedUnitValue{UnitValue{val, UnitValue::Unit::DEG}};
    }

    static constexpr TypedUnitValue fromRadian(f64 val)
        requires(TYPE == NumericBaseType::ANGLE)
    {
        return TypedUnitValue{UnitValue{val, UnitValue::Unit::RAD}};
    }

    static constexpr TypedUnitValue fromGrad(f64 val)
        requires(TYPE == NumericBaseType::ANGLE)
    {
        return TypedUnitValue{UnitValue{val, UnitValue::Unit::GRAD}};
    }

    static constexpr TypedUnitValue fromTurn(f64 val)
        requires(TYPE == NumericBaseType::ANGLE)
    {
        return TypedUnitValue{UnitValue{val, UnitValue::Unit::TURN}};
    }

    constexpr f64 toDegree() const
        requires(TYPE == NumericBaseType::ANGLE)
    {
        return _value.to(UnitValue::Unit::DEG);
    }

    constexpr f64 toRadian() const
        requires(TYPE == NumericBaseType::ANGLE)
    {
        return _value.to(UnitValue::Unit::RAD);
    }

    constexpr TypedUnitValue operator+(TypedUnitValue other) const
        requires(TYPE == NumericBaseType::ANGLE)
    {
        return fromRadian(toRadian() + other.toRadian());
    }

    constexpr TypedUnitValue operator-(TypedUnitValue other) const
        requires(TYPE == NumericBaseType::ANGLE)
    {
        return fromRadian(toRadian() - other.toRadian());
    }

    // MARK: <percentage> ------------------------------------------------------
    // https://drafts.csswg.org/css-values/#percentages

    constexpr explicit TypedUnitValue(f64 value)
        requires(TYPE == NumericBaseType::PERCENT)
        : _value(value, UnitValue::Unit::PERCENT) {}

    constexpr TypedUnitValue operator+(TypedUnitValue other) const
        requires(TYPE == NumericBaseType::PERCENT)
    {
        return TypedUnitValue{value() + other.value()};
    }

    constexpr TypedUnitValue operator-(TypedUnitValue other) const
        requires(TYPE == NumericBaseType::PERCENT)
    {
        return TypedUnitValue{value() - other.value()};
    }

    // MARK: <time> ------------------------------------------------------------
    // https://drafts.csswg.org/css-values/#time

    static constexpr TypedUnitValue fromSecond(f64 val)
        requires(TYPE == NumericBaseType::TIME)
    {
        return TypedUnitValue{UnitValue{val, UnitValue::Unit::S}};
    }

    static constexpr TypedUnitValue fromMillisecond(f64 val)
        requires(TYPE == NumericBaseType::TIME)
    {
        return TypedUnitValue{UnitValue{val, UnitValue::Unit::MS}};
    }

    constexpr f64 toSecond() const
        requires(TYPE == NumericBaseType::TIME)
    {
        return _value.to(UnitValue::Unit::S);
    }

    constexpr f64 toMillisecond() const
        requires(TYPE == NumericBaseType::TIME)
    {
        return _value.to(UnitValue::Unit::MS);
    }

    // MARK: <frequency> -------------------------------------------------------
    // https://drafts.csswg.org/css-values/#frequency

    static constexpr TypedUnitValue fromHz(f64 hz)
        requires(TYPE == NumericBaseType::FREQUENCY)
    {
        return TypedUnitValue{UnitValue{hz, UnitValue::Unit::HZ}};
    }

    static constexpr TypedUnitValue fromKHz(f64 khz)
        requires(TYPE == NumericBaseType::FREQUENCY)
    {
        return TypedUnitValue{UnitValue{khz, UnitValue::Unit::KHZ}};
    }

    constexpr f64 toHz() const
        requires(TYPE == NumericBaseType::FREQUENCY)
    {
        return _value.to(UnitValue::Unit::HZ);
    }

    // MARK: <resolution> ------------------------------------------------------
    // https://drafts.csswg.org/css-values/#resolution

    static constexpr TypedUnitValue fromDpi(f64 val)
        requires(TYPE == NumericBaseType::RESOLUTION)
    {
        return TypedUnitValue{UnitValue{val, UnitValue::Unit::DPI}};
    }

    static constexpr TypedUnitValue fromDpcm(f64 val)
        requires(TYPE == NumericBaseType::RESOLUTION)
    {
        return TypedUnitValue{UnitValue{val, UnitValue::Unit::DPCM}};
    }

    static constexpr TypedUnitValue fromDppx(f64 val)
        requires(TYPE == NumericBaseType::RESOLUTION)
    {
        return TypedUnitValue{UnitValue{val, UnitValue::Unit::DPPX}};
    }

    static constexpr TypedUnitValue infinite()
        requires(TYPE == NumericBaseType::RESOLUTION)
    {
        return fromDppx(Math::INF);
    }

    constexpr f64 toDpi() const
        requires(TYPE == NumericBaseType::RESOLUTION)
    {
        return _value.to(UnitValue::Unit::DPI);
    }

    constexpr f64 toDpcm() const
        requires(TYPE == NumericBaseType::RESOLUTION)
    {
        return _value.to(UnitValue::Unit::DPCM);
    }

    constexpr f64 toDppx() const
        requires(TYPE == NumericBaseType::RESOLUTION)
    {
        return _value.to(UnitValue::Unit::DPPX);
    }

    // MARK: <flex> ------------------------------------------------------------
    // https://drafts.csswg.org/css-grid/#typedef-flex

    static constexpr TypedUnitValue fromFr(f64 val)
        requires(TYPE == NumericBaseType::FLEX)
    {
        return TypedUnitValue{UnitValue{val, UnitValue::Unit::FR}};
    }

    constexpr f64 toFr() const
        requires(TYPE == NumericBaseType::FLEX)
    {
        return _value.to(UnitValue::Unit::FR);
    }
};

// https://drafts.csswg.org/css-values/#lengths
export using PureLength = TypedUnitValue<Some(NumericBaseType::LENGTH)>;

// https://drafts.csswg.org/css-values/#angles
export using PureAngle = TypedUnitValue<Some(NumericBaseType::ANGLE)>;

// https://drafts.csswg.org/css-values/#percentages
export using PurePercentage = TypedUnitValue<Some(NumericBaseType::PERCENT)>;

// https://drafts.csswg.org/css-values/#time
export using PureTime = TypedUnitValue<Some(NumericBaseType::TIME)>;

// https://drafts.csswg.org/css-values/#frequency
export using PureFrequency = TypedUnitValue<Some(NumericBaseType::FREQUENCY)>;

// https://drafts.csswg.org/css-values/#resolution
export using PureResolution = TypedUnitValue<Some(NumericBaseType::RESOLUTION)>;

// https://drafts.csswg.org/css-grid/#typedef-flex
export using PureFlex = TypedUnitValue<Some(NumericBaseType::FLEX)>;

export template <typename T>
    requires Meta::Derive<T, TypedUnitValue<T::BASE_TYPE, T::ACCEPTS_PERCENTS>>
struct ValueParser<T> {
    static Res<T> parse(Cursor<Sst>& c) {
        auto maybeBase = parseUnitValue(c, T::BASE_TYPE);

        if (maybeBase) {
            return Ok(T{maybeBase.take()});
        } else if (T::ACCEPTS_PERCENTS == AcceptsPercent::YES) {
            return Ok(T{try$(parseUnitValue(c, Some(NumericBaseType::PERCENT)))});
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

} // namespace Vaev::Css
