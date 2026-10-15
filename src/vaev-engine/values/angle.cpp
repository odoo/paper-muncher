module;

#include <karm/macros>

export module Vaev.Engine:values.angle;

import Karm.Core;

import :css;
import :values.base;
import :values.dimension;
import :values.resolved;

using namespace Karm;

namespace Vaev {

// 7.1. MARK: Angle Units: the <angle> type and deg, grad, rad, turn units
// https://drafts.csswg.org/css-values/#angles

export using Rad = Distinct<f64, struct _RadTag>;

export struct Angle {
    static constexpr auto TYPE = Dimension::ANGLE;

    Dimension _dim;

    static constexpr Angle fromDegree(f64 val) {
        return Angle{Dimension{val, Dimension::Unit::DEG}};
    }

    static constexpr Angle fromRadian(f64 val) {
        return Angle{Dimension{val, Dimension::Unit::RAD}};
    }

    static constexpr Angle fromGrad(f64 val) {
        return Angle{Dimension{val, Dimension::Unit::GRAD}};
    }

    static constexpr Angle fromTurn(f64 val) {
        return Angle{Dimension{val, Dimension::Unit::TURN}};
    }

    constexpr Angle()
        : _dim(0, Dimension::Unit::DEG) {}

    constexpr explicit Angle(Dimension dim)
        : _dim(dim) {}

    constexpr f64 toDegree() const {
        return _dim.to(Dimension::Unit::DEG);
    }

    constexpr f64 toRadian() const {
        return _dim.to(Dimension::Unit::RAD);
    }

    constexpr bool operator==(Angle const& other) const {
        return _dim == other._dim;
    }

    constexpr std::partial_ordering operator<=>(Angle const& other) const {
        return _dim <=> other._dim;
    }

    constexpr Angle operator+(Angle other) const {
        return fromRadian(toRadian() + other.toRadian());
    }

    constexpr Angle operator-(Angle other) const {
        return fromRadian(toRadian() - other.toRadian());
    }

    void repr(Io::Emit& e) const {
        e("{}", _dim);
    }
};

export template <>
struct _Resolved<Angle> {
    using Type = Rad;
};

template <typename Ctx = None>
Rad resolve(Angle const& value, [[maybe_unused]] Ctx const& ctx = NONE) {
    return Rad{value.toRadian()};
}

export template <>
struct ValueParser<Angle> {
    static Res<Angle> parse(Cursor<Css::Sst>& c) {
        return Ok(Angle{try$(parseDimension(c, Angle::TYPE))});
    }
};

export template <>
struct ValueSerializer<Angle> {
    static Res<> serialize(Io::TextWriter& w, Angle const& value) {
        return serializeValue(w, value._dim);
    }
};

} // namespace Vaev
