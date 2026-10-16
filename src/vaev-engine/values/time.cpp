module;

#include <karm/macros>

export module Vaev.Engine:values.time;

import Karm.Core;

import :css;
import :values.base;
import :values.unitValue;

using namespace Karm;

namespace Vaev {

// 7.2. MARK: Duration Units: the <time> type and s, ms units
// https://drafts.csswg.org/css-values/#time

export struct Time {
    UnitValue _value;

    static constexpr Time fromSecond(f64 val) {
        return Time{UnitValue{val, UnitValue::Unit::S}};
    }

    static constexpr Time fromMillisecond(f64 val) {
        return Time{UnitValue{val, UnitValue::Unit::MS}};
    }

    constexpr Time()
        : _value(0, UnitValue::Unit::S) {}

    constexpr explicit Time(UnitValue value)
        : _value(value) {
        if (value.baseType() != NumericBaseType::TIME)
            panic("unit value is not a <time>");
    }

    constexpr UnitValue unitValue() const {
        return _value;
    }

    constexpr f64 toSecond() const {
        return _value.to(UnitValue::Unit::S);
    }

    constexpr f64 toMillisecond() const {
        return _value.to(UnitValue::Unit::MS);
    }

    constexpr auto operator<=>(Time const&) const = default;

    void repr(Io::Emit& e) const {
        _value.repr(e);
    }
};

export template <>
struct ValueParser<Time> {
    static Res<Time> parse(Cursor<Css::Sst>& c) {
        return Ok(Time{try$(parseUnitValue(c, NumericBaseType::TIME))});
    }
};

export template <>
struct ValueSerializer<Time> {
    static Res<> serialize(Io::TextWriter& w, Time const& value) {
        return serializeValue(w, value.unitValue());
    }
};

} // namespace Vaev
