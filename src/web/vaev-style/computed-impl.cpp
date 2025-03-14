module;

#include <karm-meta/id.h>

module Vaev.Style:computed_impl;

import :computed;
import :props;

namespace Vaev::Style {

Computed const& Computed::initial() {
    static Computed computed = [] {
        Computed res{};
        StyleProp::any([&]<typename T>() {
            if constexpr (requires { T::initial(); })
                T{}.apply(res);
        });
        return res;
    }();
    return computed;
}

} // namespace Vaev::Style
