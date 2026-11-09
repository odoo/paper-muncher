module;

#include <karm/macros>

export module Vaev.Engine:css.values.numeric;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.serializer;
import :css.values.base;
import :css.values.calc;
import :css.values.primitives;
import :css.values.unitValue;

using namespace Karm;

// Specified values of the CSS numeric data types.
// https://drafts.csswg.org/css-values/#numeric-types

namespace Vaev::Css {

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

        if (not(value.baseType() == TYPE or (WITH_PERCENT == AcceptsPercent::YES and value.baseType() == NumericBaseType::PERCENT)))
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

    // MARK: <angle> -----------------------------------------------------------
    // https://drafts.csswg.org/css-values/#angles

    constexpr f64 toDegree() const
        requires(TYPE == NumericBaseType::ANGLE)
    {
        return _value.to(UnitValue::Unit::DEG);
    }

    // MARK: <resolution> ------------------------------------------------------
    // https://drafts.csswg.org/css-values/#resolution

    constexpr f64 toDppx() const
        requires(TYPE == NumericBaseType::RESOLUTION)
    {
        return _value.to(UnitValue::Unit::DPPX);
    }
};

// https://drafts.csswg.org/css-values/#numbers
export using PureNumber = TypedUnitValue<NONE>;

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

// MARK: Literal values --------------------------------------------------------
// A value in a given unit, e.g. `Css::px(0)`, `Css::percent(50)`.
// They widen to the calc-capable types that accept them (see TypedCalc).

export constexpr PureNumber number(f64 value) {
    return PureNumber{UnitValue{value, UnitValue::Unit::NONE}};
}

export constexpr PurePercentage percent(f64 value) {
    return PurePercentage{UnitValue{value, UnitValue::Unit::PERCENT}};
}

export constexpr PureLength px(f64 value) {
    return PureLength{UnitValue{value, UnitValue::Unit::PX}};
}

export constexpr PureAngle deg(f64 value) {
    return PureAngle{UnitValue{value, UnitValue::Unit::DEG}};
}

export constexpr PureResolution dppx(f64 value) {
    return PureResolution{UnitValue{value, UnitValue::Unit::DPPX}};
}

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

// MARK: Typed Calc ------------------------------------------------------------

export template <Opt<NumericBaseType> TYPE, AcceptsPercent WITH_PERCENT = AcceptsPercent::NO>
struct TypedCalc {
    static_assert(WITH_PERCENT == AcceptsPercent::NO or TYPE);

    Calc _calc;

    TypedCalc(Unchecked, Calc calc) : _calc(std::move(calc)) {}

    // A plain value of a type this accepts, e.g. `LengthPercentage start = Css::px(0);`
    template <Opt<NumericBaseType> T>
        requires(T == TYPE or (T == NumericBaseType::PERCENT and WITH_PERCENT == AcceptsPercent::YES))
    TypedCalc(TypedUnitValue<T> value) : _calc(value.unitValue()) {}

    Calc const& calcTree() const {
        return _calc;
    }

    void repr(Io::Emit& e) const {
        e("{}", _calc);
    }
};

// https://drafts.csswg.org/css-values-4/#numbers
export using Number = TypedCalc<NONE>;

// https://drafts.csswg.org/css-values-4/#lengths
export using Length = TypedCalc<Some(NumericBaseType::LENGTH)>;

// https://drafts.csswg.org/css-values-4/#angles
export using Angle = TypedCalc<Some(NumericBaseType::ANGLE)>;

// https://drafts.csswg.org/css-values-4/#frequency
export using Frequency = TypedCalc<Some(NumericBaseType::FREQUENCY)>;

// https://drafts.csswg.org/css-values-4/#resolution
export using Resolution = TypedCalc<Some(NumericBaseType::RESOLUTION)>;

// https://drafts.csswg.org/css-values-4/#percentages
export using Percentage = TypedCalc<Some(NumericBaseType::PERCENT)>;

// https://drafts.csswg.org/css-values-4/#mixed-percentages
export using LengthPercentage = TypedCalc<Some(NumericBaseType::LENGTH), AcceptsPercent::YES>;
export using FrequencyPercentage = TypedCalc<Some(NumericBaseType::FREQUENCY), AcceptsPercent::YES>;
export using AnglePercentage = TypedCalc<Some(NumericBaseType::ANGLE), AcceptsPercent::YES>;
export using TimePercentage = TypedCalc<Some(NumericBaseType::TIME), AcceptsPercent::YES>;

template <Opt<NumericBaseType> TYPE, AcceptsPercent WITH_PERCENT>
struct ValueParser<TypedCalc<TYPE, WITH_PERCENT>> {
    static Res<TypedCalc<TYPE, WITH_PERCENT>> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto numeric = parseValue<TypedUnitValue<TYPE, WITH_PERCENT>>(c);
        if (numeric) {
            return Ok(TypedCalc<TYPE, WITH_PERCENT>{UNCHECKED, numeric.take().unitValue()});
        }

        if (c.peek() == Sst::FUNC) {
            auto save = c;
            auto res = _parseTypedMathFunction(c);

            if (not res)
                c = save;
            return res;
        }

        return numeric.none();
    }

    // https://drafts.csswg.org/css-values-4/#calc-syntax
    static Res<TypedCalc<TYPE, WITH_PERCENT>> _parseTypedMathFunction(Cursor<Sst>& c) {
        auto calc = try$(parseMathFunction(c));

        auto type = determineType(calc, WITH_PERCENT == AcceptsPercent::YES ? TYPE : NONE);
        if (not type)
            return Error::invalidData("calc() type error");

        if (!type->matches(TYPE, WITH_PERCENT == AcceptsPercent::YES))
            return Error::invalidData("calc() does not produce the right type");

        return Ok(TypedCalc<TYPE, WITH_PERCENT>{UNCHECKED, std::move(calc)});
    }
};

template <Opt<NumericBaseType> TYPE, AcceptsPercent WITH_PERCENT>
struct ValueSerializer<TypedCalc<TYPE, WITH_PERCENT>> {
    static Res<> serialize(Io::TextWriter& w, TypedCalc<TYPE, WITH_PERCENT> const& value) {
        return serializeMathFunction(w, value.calcTree());
    }
};

} // namespace Vaev::Css
