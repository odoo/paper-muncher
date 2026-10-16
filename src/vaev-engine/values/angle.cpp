module;

#include <karm/macros>

export module Vaev.Engine:values.angle;

import Karm.Core;

import :css;
import :values.base;
import :values.unitValue;
import :values.resolved;

using namespace Karm;

namespace Vaev {

// 7.1. MARK: Angle Units: the <angle> type and deg, grad, rad, turn units
// https://drafts.csswg.org/css-values/#angles

export using Rad = Distinct<f64, struct _RadTag>;

export struct Angle {
    UnitValue _value;

    static constexpr Angle fromDegree(f64 val) {
        return Angle{UnitValue{val, UnitValue::Unit::DEG}};
    }

    static constexpr Angle fromRadian(f64 val) {
        return Angle{UnitValue{val, UnitValue::Unit::RAD}};
    }

    static constexpr Angle fromGrad(f64 val) {
        return Angle{UnitValue{val, UnitValue::Unit::GRAD}};
    }

    static constexpr Angle fromTurn(f64 val) {
        return Angle{UnitValue{val, UnitValue::Unit::TURN}};
    }

    constexpr Angle()
        : _value(0, UnitValue::Unit::DEG) {}

    constexpr explicit Angle(UnitValue value)
        : _value(value) {
        if (value.baseType() != NumericBaseType::ANGLE)
            panic("unit value is not an <angle>");
    }

    constexpr UnitValue unitValue() const {
        return _value;
    }

    constexpr f64 toDegree() const {
        return _value.to(UnitValue::Unit::DEG);
    }

    constexpr f64 toRadian() const {
        return _value.to(UnitValue::Unit::RAD);
    }

    constexpr auto operator<=>(Angle const&) const = default;

    constexpr Angle operator+(Angle other) const {
        return fromRadian(toRadian() + other.toRadian());
    }

    constexpr Angle operator-(Angle other) const {
        return fromRadian(toRadian() - other.toRadian());
    }

    void repr(Io::Emit& e) const {
        _value.repr(e);
    }
};

export template <>
struct _Resolved<Angle> {
    using Type = Rad;
};

Rad resolve(Angle const& value, [[maybe_unused]] auto const& ctx = NONE) {
    return Rad{value.toRadian()};
}

export template <>
struct ValueParser<Angle> {
    static Res<Angle> parse(Cursor<Css::Sst>& c) {
        return Ok(Angle{try$(parseUnitValue(c, NumericBaseType::ANGLE))});
    }
};

export template <>
struct ValueSerializer<Angle> {
    static Res<> serialize(Io::TextWriter& w, Angle const& value) {
        return serializeValue(w, value.unitValue());
    }
};

} // namespace Vaev
