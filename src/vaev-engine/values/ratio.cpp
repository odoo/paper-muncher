module;

#include <karm-base/base.h>

export module Vaev.Engine:values.ratio;

namespace Vaev {

export struct Ratio {
    f64 num;
    f64 den = 1.0;

    constexpr f64 eval() const {
        return num / den;
    }
};

} // namespace Vaev
