module;

#include <karm/macros>

export module Vaev.Engine:values.unitValue;

import Karm.Core;

import :css;
import :values.base;

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

    constexpr bool absolute() const {
        return factorOf(_unit) != 0;
    }

    constexpr UnitValue canonical() const {
        if (not absolute())
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
        if (baseType() != other.baseType() or not absolute() or not other.absolute())
            return std::partial_ordering::unordered;
        return canonical().value() <=> other.canonical().value();
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

} // namespace Vaev
