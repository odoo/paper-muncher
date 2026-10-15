module;

#include <karm/macros>

export module Vaev.Engine:values.resolution;

import Karm.Core;

import :css;
import :values.base;
import :values.dimension;

using namespace Karm;

namespace Vaev {

// 7.4. MARK: Resolution Units: the <resolution> type and dpi, dpcm, dppx units
// https://drafts.csswg.org/css-values/#resolution

export struct Resolution {
    static constexpr auto TYPE = Dimension::RESOLUTION;

    Dimension _dim;

    static constexpr Resolution fromDpi(f64 val) {
        return Resolution{Dimension{val, Dimension::Unit::DPI}};
    }

    static constexpr Resolution fromDpcm(f64 val) {
        return Resolution{Dimension{val, Dimension::Unit::DPCM}};
    }

    static constexpr Resolution fromDppx(f64 val) {
        return Resolution{Dimension{val, Dimension::Unit::DPPX}};
    }

    static constexpr Resolution infinite() {
        return fromDppx(Math::INF);
    }

    constexpr Resolution()
        : _dim(0, Dimension::Unit::DPPX) {}

    constexpr explicit Resolution(Dimension dim)
        : _dim(dim) {}

    constexpr f64 toDpi() const {
        return _dim.to(Dimension::Unit::DPI);
    }

    constexpr f64 toDpcm() const {
        return _dim.to(Dimension::Unit::DPCM);
    }

    constexpr f64 toDppx() const {
        return _dim.to(Dimension::Unit::DPPX);
    }

    constexpr bool operator==(Resolution const& other) const {
        return _dim == other._dim;
    }

    constexpr std::partial_ordering operator<=>(Resolution const& other) const {
        return _dim <=> other._dim;
    }

    void repr(Io::Emit& e) const {
        if (_dim.value() == Math::INF)
            e("infinite");
        else
            e("{}", _dim);
    }
};

export template <>
struct ValueParser<Resolution> {
    static Res<Resolution> parse(Cursor<Css::Sst>& c) {
        return Ok(Resolution{try$(parseDimension(c, Resolution::TYPE))});
    }
};

export template <>
struct ValueSerializer<Resolution> {
    static Res<> serialize(Io::TextWriter& w, Resolution const& value) {
        return serializeValue(w, value._dim);
    }
};

} // namespace Vaev
