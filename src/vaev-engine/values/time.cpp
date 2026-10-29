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

export struct Time : TypedUnitValue<Some(NumericBaseType::TIME)> {
    using TypedUnitValue::TypedUnitValue;

    static constexpr Time fromSecond(f64 val) {
        return Time{UnitValue{val, UnitValue::Unit::S}};
    }

    static constexpr Time fromMillisecond(f64 val) {
        return Time{UnitValue{val, UnitValue::Unit::MS}};
    }

    constexpr f64 toSecond() const {
        return _value.to(UnitValue::Unit::S);
    }

    constexpr f64 toMillisecond() const {
        return _value.to(UnitValue::Unit::MS);
    }
};

} // namespace Vaev
