export module Vaev.Engine:values.computed.length;

import Karm.Core;

using namespace Karm;

namespace Vaev::Experimental {

export struct Px : Distinct<f64, struct _PxTag> {
    using Distinct::Distinct;

    // https://drafts.csswg.org/css-values-4/#snap-a-length-as-a-border-width
    Px snappedAsBorderWidth() const {
        // 1. Assert: len is non-negative.
        if (value() <= 0.0f) {
            panic("border snapping negative length");
        }

        // 2. If len is an integer number of device pixels, do nothing.
        if (Math::abs(value() - Math::round(value())) < Limits<f32>::EPSILON) {
            return Px(Math::round(value()));
        }

        // 3. If len is greater than zero, but less than 1 device pixel, round len up to 1 device pixel.
        if (value() <= 1.0f) {
            return Px(1.0f);
        }

        // 4. If len is greater than 1 device pixel, round it down to the nearest integer number of device pixels.
        return Px(Math::floor(value()));
    }

    void repr(Io::Emit& e) const {
        e("{}px", value());
    }
};

} // namespace Vaev::Experimental
