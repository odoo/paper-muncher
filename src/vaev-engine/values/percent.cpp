module;

#include <karm/macros>

export module Vaev.Engine:values.percent;

import Karm.Core;

import :css;
import :values.base;
import :values.dimension;
import :values.primitives;

using namespace Karm;

namespace Vaev {

// MARK: Percentage ------------------------------------------------------------
// https://drafts.csswg.org/css-values/#percentages

export struct Percent {
    Dimension _dim;

    constexpr explicit Percent(f64 value)
        : _dim(value, Dimension::Unit::PERCENT) {}

    constexpr explicit Percent(Dimension dim)
        : _dim(dim) {}

    constexpr f64 value() const {
        return _dim.value();
    }

    constexpr Percent operator+(Percent other) const {
        return Percent{value() + other.value()};
    }

    constexpr Percent operator-(Percent other) const {
        return Percent{value() - other.value()};
    }

    constexpr bool operator==(Percent const&) const = default;

    constexpr std::partial_ordering operator<=>(Percent const& other) const {
        return _dim <=> other._dim;
    }

    void repr(Io::Emit& e) const {
        e("{}", _dim);
    }
};

// Resolves a percentage against its basis.
export template <>
struct ValueTraits<Percent> : DimensionValueTraits<Percent, Dimension::PERCENTAGE> {
    template <typename T>
    static T resolve(Percent const& value, T relative) {
        return T{relative.template cast<f64>() * (value.value() / 100.)};
    }
};

} // namespace Vaev
