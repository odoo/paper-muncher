module;

#include <karm/macros>

export module Vaev.Engine:values.frequency;

import Karm.Core;

import :css;
import :values.base;
import :values.dimension;

using namespace Karm;

namespace Vaev {

// https://drafts.csswg.org/css-values/#frequency
// It represents the number of occurrences per second.
export struct Frequency {
    Dimension _dim;

    static constexpr Frequency fromHz(f64 hz) {
        return Frequency{Dimension{hz, Dimension::Unit::HZ}};
    }

    static constexpr Frequency fromKHz(f64 khz) {
        return Frequency{Dimension{khz, Dimension::Unit::KHZ}};
    }

    constexpr Frequency()
        : _dim(0, Dimension::Unit::HZ) {}

    constexpr explicit Frequency(Dimension dim)
        : _dim(dim) {}

    constexpr f64 toHz() const {
        return _dim.to(Dimension::Unit::HZ);
    }

    constexpr bool operator==(Frequency const& other) const {
        return _dim == other._dim;
    }

    constexpr std::partial_ordering operator<=>(Frequency const& other) const {
        return _dim <=> other._dim;
    }

    void repr(Io::Emit& e) const {
        e("{}", _dim);
    }
};

export template <>
struct ValueTraits<Frequency> : DimensionValueTraits<Frequency, Dimension::FREQUENCY> {
};

} // namespace Vaev
