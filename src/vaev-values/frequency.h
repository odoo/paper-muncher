#pragma once

#include <karm-base/distinct.h>

namespace Vaev {

// https://drafts.csswg.org/css-values/#frequency
// It represents the number of occurrences per second.
struct Frequency : Distinct<f64, struct FrequencyTag> {
    using Distinct::Distinct;

    static constexpr Frequency fromHz(f64 hz) {
        return {hz};
    }

    static constexpr Frequency fromKHz(f64 khz) {
        return {khz * 1e3};
    }
};

} // namespace Vaev
