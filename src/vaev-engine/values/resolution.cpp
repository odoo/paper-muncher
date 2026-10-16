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

export struct Resolution {
    UnitValue _value;

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

    constexpr Resolution()
        : _value(0, UnitValue::Unit::DPPX) {}

    constexpr explicit Resolution(UnitValue value)
        : _value(value) {
        if (value.baseType() != NumericBaseType::RESOLUTION)
            panic("unit value is not a <resolution>");
    }

    constexpr UnitValue unitValue() const {
        return _value;
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

    constexpr auto operator<=>(Resolution const&) const = default;

    void repr(Io::Emit& e) const {
        if (_value.value() == Math::INF)
            e("infinite");
        else
            _value.repr(e);
    }
};

export template <>
struct ValueParser<Resolution> {
    static Res<Resolution> parse(Cursor<Css::Sst>& c) {
        return Ok(Resolution{try$(parseUnitValue(c, NumericBaseType::RESOLUTION))});
    }
};

export template <>
struct ValueSerializer<Resolution> {
    static Res<> serialize(Io::TextWriter& w, Resolution const& value) {
        return serializeValue(w, value.unitValue());
    }
};

} // namespace Vaev
