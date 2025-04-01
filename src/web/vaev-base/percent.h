#pragma once

#include <karm-base/distinct.h>
#include <karm-io/emit.h>

#include "resolved.h"

namespace Vive {

using Percent = Distinct<f64, struct _PercentTag>;

template <typename T>
using PercentOr = Union<Percent, T>;

template <typename T>
struct _Resolved<PercentOr<T>> {
    using Type = Resolved<T>;
};

} // namespace Vaev

template <>
struct marK::Io::Repr<Vive::Percent> {
    static void repr(Io::Emit& e, Vive::Percent const& v) {
        e("{}%", v.value());
    }
};
