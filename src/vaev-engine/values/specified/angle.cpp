module;

#include <karm/macros>

export module Vaev.Engine:values.specified.angle;

import Karm.Core;

import :css;
import :values.specified.base;

using namespace Karm;

namespace Vaev::Experimental {

// 7.1. MARK: Angle Units: the <angle> type and deg, grad, rad, turn units
// https://drafts.csswg.org/css-values/#angles

export struct Angle {
    enum struct Unit {
        DEGREE,
        RADIAN,
        GRAD,
        TURN,
    };

    using enum Unit;

    f64 _val;
    Unit _unit = Unit::DEGREE;

    constexpr f64 val() const {
        return _val;
    }

    constexpr Unit unit() const {
        return _unit;
    }

    constexpr Angle() = default;

    constexpr Angle(f64 val, Unit unit)
        : _val(val), _unit(unit) {}

    constexpr bool operator==(Angle const& other) const {
        return _val == other._val and _unit == other._unit;
    }

    void repr(Io::Emit& e) const {
        switch (_unit) {
        case DEGREE:
            e("{}deg", _val);
            break;
        case RADIAN:
            e("{}rad", _val);
            break;
        case GRAD:
            e("{}grad", _val);
            break;
        case TURN:
            e("{}turn", _val);
            break;
        }
    }
};

export template <>
struct ValueTraits<Angle> {
    using ComputedType = Degree;

    static ComputedType compute(Angle const& angle, ComputationContext const&, Style::ComputedValues const&) {
        switch (angle.unit()) {
        case Angle::DEGREE:
            return Degree(angle.val());
        case Angle::RADIAN:
            return Degree(angle.val() * 180.0 / Math::PI);
        case Angle::GRAD:
            return Degree(angle.val() * 0.9);
        case Angle::TURN:
            return Degree(angle.val() * 360.0);
        }
        unreachable();
    }

    static Angle fromComputed(ComputedType const& computed) {
        return Angle(computed.value(), Angle::DEGREE);
    }

    static Res<Angle::Unit> _parseAngleUnit(Str unit) {
        if (eqCi(unit, "deg"s))
            return Ok(Angle::Unit::DEGREE);
        else if (eqCi(unit, "grad"s))
            return Ok(Angle::Unit::GRAD);
        else if (eqCi(unit, "rad"s))
            return Ok(Angle::Unit::RADIAN);
        else if (eqCi(unit, "turn"s))
            return Ok(Angle::Unit::TURN);
        else
            return Error::invalidData("unknown length unit");
    }

    static Res<Angle> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::DIMENSION) {
            Io::SScan scan = c->token.data.str();
            auto value = Io::atof(scan).unwrapOr(0.0);
            auto unit = try$(_parseAngleUnit(scan.remStr()));

            c.next();
            return Ok(Angle{value, unit});
        }

        return Error::invalidData("expected angle");
    }
};

} // namespace Vaev::Experimental
