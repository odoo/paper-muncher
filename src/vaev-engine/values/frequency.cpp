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

export struct Frequency : TypedUnitValue<NumericBaseType::FREQUENCY> {
    using TypedUnitValue::TypedUnitValue;

    static constexpr Frequency fromHz(f64 hz) {
        return Frequency{UnitValue{hz, UnitValue::Unit::HZ}};
    }

    static constexpr Frequency fromKHz(f64 khz) {
        return Frequency{UnitValue{khz, UnitValue::Unit::KHZ}};
    }

    constexpr f64 toHz() const {
        return _value.to(UnitValue::Unit::HZ);
    }
};

} // namespace Vaev
