export module Vaev.Engine:values.resolved;

import Karm.Core;

using namespace Karm;

namespace Vaev {

export template <typename T>
struct _Resolved {
    using Type = T;
};

export template <typename T>
using Resolved = typename _Resolved<T>::Type;

// MARK: Computed --------------------------------------------------------------
// https://drafts.csswg.org/css-cascade/#computed

// The type of the computed value of a specified value, by default a value
// is already absolute and computes to itself.
export template <typename T>
struct _Computed {
    using Type = T;
};

export template <typename T>
using Computed = typename _Computed<T>::Type;

export template <typename T>
    requires Meta::Same<Computed<T>, T>
T compute(T const& value, [[maybe_unused]] auto const& ctx) {
    return value;
}

// Values made of other values are written once, parametrized by the stage
// they are in: Foo<Specified> as parsed, Foo<Computed> once computed.
export template <typename T>
using Specified = T;

// Turns a computed value back into a specified value, so it can be fed back
// into the cascade (inheritance, toggle(), serialization, ...)
export template <typename T>
struct _Specify {
    static T from(Computed<T> const& value) {
        return value;
    }
};

export template <typename T>
T specified(Computed<T> const& value) {
    return _Specify<T>::from(value);
}

} // namespace Vaev
