module;

#include <karm/macros>

export module Vaev.Engine:values.time;

import Karm.Core;

import :css;
import :values.base;
import :values.dimension;

using namespace Karm;

namespace Vaev {

// 7.2. MARK: Duration Units: the <time> type and s, ms units
// https://drafts.csswg.org/css-values/#time

export struct Time {
    Dimension _dim;

    static constexpr Time fromSecond(f64 val) {
        return Time{Dimension{val, Dimension::Unit::S}};
    }

    static constexpr Time fromMillisecond(f64 val) {
        return Time{Dimension{val, Dimension::Unit::MS}};
    }

    constexpr Time()
        : _dim(0, Dimension::Unit::S) {}

    constexpr explicit Time(Dimension dim)
        : _dim(dim) {}

    constexpr f64 toSecond() const {
        return _dim.to(Dimension::Unit::S);
    }

    constexpr f64 toMillisecond() const {
        return _dim.to(Dimension::Unit::MS);
    }

    constexpr bool operator==(Time const& other) const {
        return _dim == other._dim;
    }

    constexpr std::partial_ordering operator<=>(Time const& other) const {
        return _dim <=> other._dim;
    }

    void repr(Io::Emit& e) const {
        e("{}", _dim);
    }
};

export template <>
struct ValueTraits<Time> : DimensionValueTraits<Time, Dimension::TIME> {
};

} // namespace Vaev
