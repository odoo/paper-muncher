module;

#include <karm/macros>

export module Vaev.Engine:values.percent;

import Karm.Core;

import :css;
import :values.base;
import :values.unitValue;
import :values.primitives;
import :values.resolved;

using namespace Karm;

namespace Vaev {

// MARK: Percentage ------------------------------------------------------------
// https://drafts.csswg.org/css-values/#percentages

export struct Percent {
    UnitValue _value;

    constexpr explicit Percent(f64 value)
        : _value(value, UnitValue::Unit::PERCENT) {}

    constexpr explicit Percent(UnitValue value)
        : _value(value) {
        if (value.baseType() != NumericBaseType::PERCENT)
            panic("unit value is not a <percentage>");
    }

    constexpr UnitValue unitValue() const {
        return _value;
    }

    constexpr f64 value() const {
        return _value.value();
    }

    constexpr Percent operator+(Percent other) const {
        return Percent{value() + other.value()};
    }

    constexpr Percent operator-(Percent other) const {
        return Percent{value() - other.value()};
    }

    constexpr auto operator<=>(Percent const&) const = default;

    void repr(Io::Emit& e) const {
        _value.repr(e);
    }
};

export template <>
struct ValueParser<Percent> {
    static Res<Percent> parse(Cursor<Css::Sst>& c) {
        return Ok(Percent{try$(parseUnitValue(c, NumericBaseType::PERCENT))});
    }
};

export template <>
struct ValueSerializer<Percent> {
    static Res<> serialize(Io::TextWriter& w, Percent const& value) {
        return serializeValue(w, value.unitValue());
    }
};

export template <typename T>
using PercentOr = Union<Percent, T>;

export template <typename T>
struct _Resolved<PercentOr<T>> {
    using Type = Resolved<T>;
};

export template <typename T>
Resolved<T> resolve(PercentOr<T> const& value, auto const& ctx, Resolved<T> relative) {
    if (auto v = value.template is<Percent>())
        return Resolved<T>{relative.template cast<f64>() * ((*v).value() / 100.)};
    return resolve(value.template expect<T>(), ctx);
}

} // namespace Vaev
