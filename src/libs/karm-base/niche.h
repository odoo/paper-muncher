#pragma once

#include <karm-meta/traits.h>

namespace Karm {

template <typename T>
struct Niche;

template <typename T>
concept Nicheable = requires {
    typename Niche<T>::Content;
};

template <>
struct Niche<bool> {
    struct Content {
        u8 data;

        constexpr Content() : data(2) {}

        constexpr bool has() const {
            return data != 2;
        }
    };
};

template <Meta::Enum T>
    requires requires { T::_LEN; }
struct Niche<T> {
    struct Content {
        using Underlying = Meta::UnderlyingType<T>;
        Underlying data;

        constexpr Content()
            : data(IntType(T::_LEN) + 1) {}

        constexpr bool has() const {
            return data != (IntType(T::_LEN) + 1);
        }
    };
};

inline constexpr char _NICHE_VALUE = 'e';
inline char const* NICHE_PTR = &_NICHE_VALUE;

} // namespace Karm