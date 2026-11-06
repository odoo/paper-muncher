module;

#include <karm/macros>

export module PaperMuncher:absoluteLength;

import Karm.Core;

import Vaev.Engine;

using namespace Karm;

namespace PaperMuncher {

// https://drafts.csswg.org/css-values/#absolute-lengths
export struct AbsoluteLength : Vaev::Css::PureLength {
    constexpr explicit AbsoluteLength(Vaev::UnitValue value)
        : Vaev::Css::PureLength(value) {
        if (not value.isAbsolute())
            panic("unit value is not an absolute <length>");
    }

    constexpr f64 toPx() const {
        return unitValue().to(Vaev::UnitValue::Unit::PX);
    }
};

} // namespace PaperMuncher

export template <>
struct Vaev::Css::ValueParser<PaperMuncher::AbsoluteLength> {
    static Res<PaperMuncher::AbsoluteLength> parse(Cursor<Sst>& c) {
        auto save = c;
        auto value = try$(parseUnitValue(c, Some(NumericBaseType::LENGTH)));
        if (not value.isAbsolute()) {
            c = save;
            return Error::invalidData("expected <absolute-length>");
        }
        return Ok(PaperMuncher::AbsoluteLength{value});
    }
};
