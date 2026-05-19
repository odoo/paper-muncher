module;

#include <karm/macros>

export module Vaev.Engine:values.computed.dimensions;

import Karm.Core;

using namespace Karm;

namespace Vaev::Experimental {

export template <typename Tag>
struct _Dimension {
    f64 _value;

    always_inline constexpr _Dimension() = default;

    always_inline constexpr explicit _Dimension(f64 value) : _value(value) {}

    always_inline constexpr f64 value() const { return _value; }

    always_inline constexpr bool operator==(_Dimension const& other) const = default;

    always_inline constexpr auto operator<=>(_Dimension const& other) const = default;

    always_inline constexpr _Dimension operator-() const {
        return _Dimension(-_value);
    }

    always_inline constexpr _Dimension operator+(_Dimension other) const {
        return _Dimension(_value + other._value);
    }

    always_inline constexpr _Dimension operator-(_Dimension other) const {
        return _Dimension(_value - other._value);
    }

    always_inline constexpr _Dimension operator*(f64 other) const {
        return _Dimension(_value * other);
    }

    always_inline constexpr _Dimension operator/(f64 other) const {
        return _Dimension(_value / other);
    }

    always_inline constexpr f64 operator/(_Dimension other) const {
        return _value / other._value;
    }

    always_inline constexpr _Dimension& operator+=(_Dimension other) {
        *this = *this + other;
        return *this;
    }

    always_inline constexpr _Dimension& operator-=(_Dimension other) {
        *this = *this - other;
        return *this;
    }

    always_inline constexpr _Dimension& operator*=(f64 other) {
        *this = *this * other;
        return *this;
    }

    always_inline constexpr _Dimension& operator/=(f64 other) {
        *this = *this / other;
        return *this;
    }
};

export using Px = _Dimension<struct _PxTag>;

export using Degree = _Dimension<struct _DegreeTag>;

export using Second = _Dimension<struct _SecondTag>;

export using Hertz = _Dimension<struct _HertzTag>;

export using Dppx = _Dimension<struct _DppxTag>;

} // namespace Vaev::Experimental

namespace Vaev::Experimental::Literals {

export constexpr Px operator""_px(unsigned long long val) {
    return Px{static_cast<f64>(val)};
}

export constexpr Px operator""_px(long double val) {
    return Px{static_cast<f64>(val)};
}

export constexpr Degree operator""_deg(unsigned long long val) {
    return Degree{static_cast<f64>(val)};
}

export constexpr Degree operator""_deg(long double val) {
    return Degree{static_cast<f64>(val)};
}

export constexpr Second operator""_s(unsigned long long val) {
    return Second{static_cast<f64>(val)};
}

export constexpr Second operator""_s(long double val) {
    return Second{static_cast<f64>(val)};
}

export constexpr Hertz operator""_Hz(unsigned long long val) {
    return Hertz{static_cast<f64>(val)};
}

export constexpr Hertz operator""_Hz(long double val) {
    return Hertz{static_cast<f64>(val)};
}

export constexpr Dppx operator""_dppx(unsigned long long val) {
    return Dppx{static_cast<f64>(val)};
}

export constexpr Dppx operator""_dppx(long double val) {
    return Dppx{static_cast<f64>(val)};
}

} // namespace Vaev::Experimental::Literals
