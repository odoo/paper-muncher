module;

#include <karm/macros>

export module Vaev.Engine:values.dimension;

import Karm.Core;

import :css;
import :values.base;
import :values.primitives;

using namespace Karm;

namespace Vaev {

// MARK: Dimension -------------------------------------------------------------
// https://drafts.csswg.org/css-values/#dimensions

// A number with a unit, every quantity in CSS (length, angle, percentage, ...)
// is stored as a dimension, the unit tells which kind of quantity it is.
export struct Dimension {
    enum struct Type : u8 {
        NUMBER,
        PERCENTAGE,
        LENGTH,
        ANGLE,
        TIME,
        FREQUENCY,
        RESOLUTION,

        _LEN,
    };

    using enum Type;

    enum struct Unit : u8 {
#define UNIT(NAME, ...) NAME,
#include "defs/units.inc"
#undef UNIT

        _LEN,
    };

    f64 _value = 0;
    Unit _unit = Unit::NONE;

    static constexpr Type typeOf(Unit unit) {
        switch (unit) {
#define UNIT(NAME, TYPE, ...) \
    case Unit::NAME:          \
        return TYPE;
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

    // Factor to convert a value into the canonical unit of its type,
    // zero if the unit depends on a context.
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
    static constexpr Unit canonicalUnitOf(Type type) {
        switch (type) {
        case NUMBER:
            return Unit::NONE;
        case PERCENTAGE:
            return Unit::PERCENT;
        case LENGTH:
            return Unit::PX;
        case ANGLE:
            return Unit::DEG;
        case TIME:
            return Unit::S;
        case FREQUENCY:
            return Unit::HZ;
        case RESOLUTION:
            return Unit::DPPX;
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

    constexpr Dimension() = default;

    constexpr Dimension(f64 value, Unit unit)
        : _value(value), _unit(unit) {}

    constexpr f64 value() const {
        return _value;
    }

    constexpr Unit unit() const {
        return _unit;
    }

    constexpr Type type() const {
        return typeOf(_unit);
    }

    // Whether the dimension can be converted to its canonical unit without a context.
    constexpr bool isAbsolute() const {
        return factorOf(_unit) != 0;
    }

    constexpr Dimension canonical() const {
        if (not isAbsolute())
            panic("dimension depends on a context");
        return {_value * factorOf(_unit), canonicalUnitOf(type())};
    }

    // The value converted to another absolute unit of the same type.
    constexpr f64 to(Unit unit) const {
        if (typeOf(unit) != type())
            panic("converting between dimensions of different types");
        return canonical().value() / factorOf(unit);
    }

    constexpr bool operator==(Dimension const& other) const {
        if (_unit == other._unit)
            return _value == other._value;
        if (type() != other.type() or not isAbsolute() or not other.isAbsolute())
            return false;
        return canonical().value() == other.canonical().value();
    }

    constexpr std::partial_ordering operator<=>(Dimension const& other) const {
        if (_unit == other._unit)
            return _value <=> other._value;
        if (type() != other.type() or not isAbsolute() or not other.isAbsolute())
            return std::partial_ordering::unordered;
        return canonical().value() <=> other.canonical().value();
    }

    void repr(Io::Emit& e) const {
        e("{}{}", _value, nameOf(_unit));
    }
};

export template <>
struct ValueParser<Dimension> {
    static Res<Dimension> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::NUMBER) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(Dimension{Io::atof(scan).unwrapOr(0.0), Dimension::Unit::NONE});
        }

        if (c.peek() == Css::Token::PERCENTAGE) {
            Io::SScan scan = c->token.data.str();
            c.next();
            return Ok(Dimension{Io::atof(scan).unwrapOr(0.0), Dimension::Unit::PERCENT});
        }

        if (c.peek() == Css::Token::DIMENSION) {
            Io::SScan scan = c->token.data.str();
            auto value = Io::atof(scan, {.allowExp = false}).unwrapOr(0.0);
            auto unit = try$(Dimension::unitFromName(scan.remStr()));
            c.next();
            return Ok(Dimension{value, unit});
        }

        return Error::invalidData("expected dimension");
    }
};

export template <>
struct ValueSerializer<Dimension> {
    // https://drafts.csswg.org/cssom/#serialize-a-css-component-value
    static Res<> serialize(Io::TextWriter& w, Dimension const& value) {
        // The <number> component serialized as per <number> followed by the unit in its canonical form as defined in its respective specification.
        try$(serializeValue(w, value.value()));
        return w.writeStr(Dimension::nameOf(value.unit()));
    }
};

// Parses a dimension of the given type.
export Res<Dimension> parseDimension(Cursor<Css::Sst>& c, Dimension::Type type) {
    if (c.ended())
        return Error::invalidData("unexpected end of input");

    // https://drafts.csswg.org/css-values/#lengths
    // For zero lengths the unit identifier is optional
    if (type == Dimension::LENGTH and c.skip(Css::Token::number("0")))
        return Ok(Dimension{0, Dimension::Unit::PX});

    auto save = c;
    auto dim = try$(parseValue<Dimension>(c));
    if (dim.type() != type) {
        c = save;
        return Error::invalidData("unexpected dimension type");
    }
    return Ok(dim);
}

// MARK: Dimension Traits ------------------------------------------------------

// How a value is stored as a dimension, values that are a dimension of a single
// type (Length, Angle, ...) hold it in `_dim` and declare their type in `TYPE`.
export template <typename T>
struct _Dimension {
    static constexpr Dimension::Type TYPE = T::TYPE;

    static Dimension to(T const& value) {
        return value._dim;
    }

    static T from(Dimension dim) {
        return T{dim};
    }
};

export template <>
struct _Dimension<Number> {
    static constexpr Dimension::Type TYPE = Dimension::NUMBER;

    static Dimension to(Number value) {
        return {value, Dimension::Unit::NONE};
    }

    static Number from(Dimension dim) {
        return dim.value();
    }
};

export template <typename T>
constexpr Dimension::Type dimensionTypeOf = _Dimension<T>::TYPE;

export template <typename T>
Dimension toDimension(T const& value) {
    return _Dimension<T>::to(value);
}

export template <typename T>
T fromDimension(Dimension dim) {
    return _Dimension<T>::from(dim);
}

} // namespace Vaev
