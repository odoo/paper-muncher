module;

#include <karm/macros>

export module Vaev.Engine:values.resolution;

import Karm.Core;

import :css;
import :values.base;
import :values.unitValue;

using namespace Karm;

namespace Vaev {

// 7.4. MARK: Resolution Units: the <resolution> type and dpi, dpcm, dppx units
// https://drafts.csswg.org/css-values/#resolution

export struct Resolution : TypedUnitValue<Some(NumericBaseType::RESOLUTION)> {
    using TypedUnitValue::TypedUnitValue;

    static constexpr Resolution fromDpi(f64 val) {
        return Resolution{UnitValue{val, UnitValue::Unit::DPI}};
    }

    static constexpr Resolution fromDpcm(f64 val) {
        return Resolution{UnitValue{val, UnitValue::Unit::DPCM}};
    }

    static constexpr Resolution fromDppx(f64 val) {
        return Resolution{UnitValue{val, UnitValue::Unit::DPPX}};
    }

    static constexpr Resolution infinite() {
        return fromDppx(Math::INF);
    }

    constexpr f64 toDpi() const {
        return _value.to(UnitValue::Unit::DPI);
    }

    constexpr f64 toDpcm() const {
        return _value.to(UnitValue::Unit::DPCM);
    }

    constexpr f64 toDppx() const {
        return _value.to(UnitValue::Unit::DPPX);
    }

    void repr(Io::Emit& e) const {
        if (_value.value() == Math::INF)
            e("infinite");
        else
            _value.repr(e);
    }
};

} // namespace Vaev
