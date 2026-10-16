module;

#include <karm/macros>

export module Vaev.Engine:values.frequency;

import Karm.Core;

import :css;
import :values.base;
import :values.unitValue;

using namespace Karm;

namespace Vaev {

// https://drafts.csswg.org/css-values/#frequency
// It represents the number of occurrences per second.

export struct Frequency {
    UnitValue _value;

    static constexpr Frequency fromHz(f64 hz) {
        return Frequency{UnitValue{hz, UnitValue::Unit::HZ}};
    }

    static constexpr Frequency fromKHz(f64 khz) {
        return Frequency{UnitValue{khz, UnitValue::Unit::KHZ}};
    }

    constexpr Frequency()
        : _value(0, UnitValue::Unit::HZ) {}

    constexpr explicit Frequency(UnitValue value)
        : _value(value) {
        if (value.baseType() != NumericBaseType::FREQUENCY)
            panic("unit value is not a <frequency>");
    }

    constexpr UnitValue unitValue() const {
        return _value;
    }

    constexpr f64 toHz() const {
        return _value.to(UnitValue::Unit::HZ);
    }

    constexpr auto operator<=>(Frequency const&) const = default;

    void repr(Io::Emit& e) const {
        _value.repr(e);
    }
};

export template <>
struct ValueParser<Frequency> {
    static Res<Frequency> parse(Cursor<Css::Sst>& c) {
        return Ok(Frequency{try$(parseUnitValue(c, NumericBaseType::FREQUENCY))});
    }
};

export template <>
struct ValueSerializer<Frequency> {
    static Res<> serialize(Io::TextWriter& w, Frequency const& value) {
        return serializeValue(w, value.unitValue());
    }
};

} // namespace Vaev
