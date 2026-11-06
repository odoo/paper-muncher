module;

#include <karm/macros>

export module Vaev.Engine:css.values.length;

import Karm.Core;

import :css.parser;
import :css.values.base;
import :css.values.keywords;
import :css.values.primitives;
import :css.values.unitValue;
import :values.unitValue;

using namespace Karm;

namespace Vaev::Css {

// 6.2. MARK: Absolute Lengths: the cm, mm, Q, in, pt, pc, px units
// https://drafts.csswg.org/css-values/#absolute-lengths

export struct PureAbsoluteLength : TypedUnitValue<Some(NumericBaseType::LENGTH)> {
    constexpr explicit PureAbsoluteLength(UnitValue value)
        : TypedUnitValue(value) {
        if (not value.isAbsolute())
            panic("unit value is not an absolute <length>");
    }

    constexpr f64 toPx() const {
        return _value.to(UnitValue::Unit::PX);
    }
};

export template <>
struct ValueParser<PureAbsoluteLength> {
    static Res<PureAbsoluteLength> parse(Cursor<Sst>& c) {
        auto save = c;
        auto value = try$(parseUnitValue(c, Some(NumericBaseType::LENGTH)));
        if (not value.isAbsolute()) {
            c = save;
            return Error::invalidData("expected <absolute-length>");
        }
        return Ok(PureAbsoluteLength{value});
    }
};

} // namespace Vaev::Css
