#pragma once

#include "_prelude.h"
#include "macros.h"

namespace Karm {

// MARK: Unsigned --------------------------------------------------------------

using usize = __SIZE_TYPE__;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
#ifdef __SIZEOF_INT128__
using u128 = __uint128_t;

inline u128 _bswap128(u128 value) {
    u64 high = __builtin_bswap64(static_cast<u64>(value));
    u64 low = __builtin_bswap64(static_cast<u64>(value >> 64));
    return (static_cast<u128>(high) << 64) | low;
}
#endif

template <typename T>
    requires(sizeof(T) <= 16)
always_inline constexpr T bswap(T value) {
#ifdef __SIZEOF_INT128__
    if (sizeof(T) == 16)
        return _bswap128(value);
#endif
    if (sizeof(T) == 8)
        return __builtin_bswap64(value);
    if (sizeof(T) == 4)
        return __builtin_bswap32(value);
    if (sizeof(T) == 2)
        return __builtin_bswap16(value);
    if (sizeof(T) == 1)
        return value;
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
template <typename T>
always_inline constexpr T toLe(T value) {
    return value;
}
#else
template <typename T>
always_inline constexpr T toLe(T value) {
    return bswap(value);
}
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
template <typename T>
always_inline constexpr T toBe(T value) {
    return bswap(value);
}
#else
template <typename T>
always_inline constexpr T toBe(T value) {
    return value;
}
#endif

static constexpr bool isLittleEndian() {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return true;
#else
    return false;
#endif
}

static constexpr bool isBigEndian() {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    return true;
#else
    return false;
#endif
}

template <typename T>
always_inline constexpr usize popcount(T value) {
    usize count = 0;
    for (usize i = 0; i < sizeof(T) * 8; i++)
        if (value & (1uz << i))
            count++;
    return count;
}

template <typename T>
always_inline constexpr T rol(T x, usize n) {
    return (x << n) | (x >> (sizeof(x) * 8 - n));
}

template <typename T>
always_inline constexpr T ror(T x, usize n) {
    return (x >> n) | (x << (sizeof(x) * 8 - n));
}

template <typename T>
always_inline constexpr T rotl(T x, usize n) {
    return (x << n) | (x >> (sizeof(x) * 8 - n));
}

template <typename T>
always_inline constexpr T rotr(T x, usize n) {
    return (x >> n) | (x << (sizeof(x) * 8 - n));
}

// MARK: Signed -----------------------------------------------------------

using isize = __PTRDIFF_TYPE__;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

#ifdef __SIZEOF_INT128__
using i128 = __int128_t;
#endif

// MARK: Floating point --------------------------------------------------------

using f16 = _Float16;
using f32 = float;
using f64 = double;

#ifdef __SIZEOF_FLOAT128__
using f128 = long double;
#endif

// MARK: Tags ------------------------------------------------------------------

/* The object should take the ownership of the memory */

struct Move {};

constexpr inline auto MOVE = Move{};

/* The object should make a copy of the memory */

struct Copy {};

constexpr inline auto COPY = Copy{};

/* The object should wrap the memory without doing a copy
   nor taking the ownership */

struct Wrap {};

constexpr inline auto WRAP = Wrap{};

/* The object should be empty initialized */

struct None {
    constexpr None() {}

    explicit operator bool() const { return false; }

    bool operator==(None const&) const = default;
    auto operator<=>(None const&) const = default;
};

constexpr inline auto NONE = None{};

template <typename T>
bool operator==(None, T* ptr) {
    return ptr == nullptr;
}

// MARK: Misc ------------------------------------------------------------------

/// A linker symbol.
using ExternSym = uint8_t[];

// MARK: Utilities -------------------------------------------------------------

template <typename T, typename U>
always_inline static inline T unionCast(U value)
    requires(sizeof(T) == sizeof(U))
{
    union X {
        U u;
        T t;
    };

    return X{.u = value}.t;
}

} // namespace Karm
