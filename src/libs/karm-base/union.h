#pragma once

#include <karm-meta/pack.h>

#include "clamp.h"
#include "cursor.h"
#include "opt.h"

namespace Karm {

struct Monostate {};

template <typename T>
struct _Empty {
    static constexpr bool value = sizeof(T) == 0;
};

template <typename T>
concept Empty = _Empty<T>::value and requires { T{}; };

template <typename T>
constexpr usize sizeOfType() {
    if constexpr (Empty<T>)
        return 0;
    return sizeof(T);
}

template <typename... Ts>
struct Union {
    static_assert(sizeof...(Ts) <= 255, "Union can only hold up to 255 types");

    alignas(max(alignof(Ts)...)) char _buf[max(sizeOfType<Ts>()...)];
    u8 _index;

    always_inline Union()
        requires(Meta::Contains<Monostate, Ts...>)
        : Union(Monostate{}) {}

    template <Meta::Contains<Ts...> T>
    always_inline Union(T const& value)
        : _index(Meta::indexOf<T, Ts...>()) {
        if constexpr (not Empty<T>)
            new (_buf) T(value);
    }

    template <Meta::Contains<Ts...> T>
    always_inline Union(T&& value)
        : _index(Meta::indexOf<T, Ts...>()) {
        if constexpr (not Empty<T>)
            new (_buf) T(std::move(value));
    }

    always_inline Union(Union const& other)
        : _index(other._index) {
        Meta::indexCast<Ts...>(
            _index, other._buf,
            [this]<typename T>(T const& ptr) {
                if constexpr (not Empty<T>)
                    new (_buf) T(ptr);
            }
        );
    }

    always_inline Union(Union&& other)
        : _index(other._index) {
        Meta::indexCast<Ts...>(_index, other._buf, [this]<typename T>(T& ptr) {
            if constexpr (not Empty<T>)
                new (_buf) T(std::move(ptr));
        });
    }

    always_inline ~Union() {
        Meta::indexCast<Ts...>(_index, _buf, []<typename T>(T& ptr) {
            if constexpr (not Empty<T>)
                ptr.~T();
        });
    }

    template <Meta::Contains<Ts...> T>
    always_inline Union& operator=(T const& value) {
        *this = Union(value);
        return *this;
    }

    template <Meta::Contains<Ts...> T>
    always_inline Union& operator=(T&& value) {
        Meta::indexCast<Ts...>(_index, _buf, []<typename U>(U& ptr) {
            if constexpr (not Empty<T>)
                ptr.~U();
        });

        _index = Meta::indexOf<T, Ts...>();
        if constexpr (not Empty<T>)
            new (_buf) T(std::move(value));

        return *this;
    }

    always_inline Union& operator=(Union const& other) {
        *this = Union(other);
        return *this;
    }

    always_inline Union& operator=(Union&& other) {
        Meta::indexCast<Ts...>(_index, _buf, []<typename T>(T& ptr) {
            if constexpr (not Empty<T>)
                ptr.~T();
        });

        _index = other._index;

        Meta::indexCast<Ts...>(_index, other._buf, [this]<typename T>(T& ptr) {
            if constexpr (not Empty<T>)
                new (_buf) T(std::move(ptr));
        });

        return *this;
    }

    template <Meta::Contains<Ts...> T>
        requires(not Empty<T>)
    always_inline T& unwrap(char const* msg = "unwrapping wrong type") lifetimebound {
        if (_index != Meta::indexOf<T, Ts...>()) [[unlikely]]
            panic(msg);

        return *reinterpret_cast<T*>(_buf);
    }

    template <Meta::Contains<Ts...> T>
        requires(not Empty<T>)
    always_inline T const& unwrap(char const* msg = "unwrapping wrong type") const lifetimebound {
        if (_index != Meta::indexOf<T, Ts...>()) [[unlikely]]
            panic(msg);

        return *reinterpret_cast<T const*>(_buf);
    }

    template <Meta::Contains<Ts...> T>
        requires(not Empty<T>)
    always_inline T const& unwrapOr(T const& fallback) const lifetimebound {
        if (_index != Meta::indexOf<T, Ts...>())
            return fallback;

        return *reinterpret_cast<T const*>(_buf);
    }

    template <typename T, typename... Args>
        requires(not Empty<T>)
    always_inline T& emplace(Args&&... args) {
        if (_index != Meta::indexOf<T, Ts...>()) {
            Meta::indexCast<Ts...>(_index, _buf, []<typename U>(U& ptr) {
                ptr.~U();
            });

            _index = Meta::indexOf<T, Ts...>();
        }

        return *new (_buf) T(std::forward<Args>(args)...);
    }

    template <Meta::Contains<Ts...> T>
    always_inline Opt<T> take() {
        if (_index != Meta::indexOf<T, Ts...>()) {
            return NONE;
        }

        if constexpr (Empty<T>)
            return T{};
        return std::move(*reinterpret_cast<T*>(_buf));
    }

    template <Meta::Contains<Ts...> T>
    always_inline Opt<T> take() const {
        if (_index != Meta::indexOf<T, Ts...>()) {
            return NONE;
        }

        if constexpr (Empty<T>)
            return T{};
        return *reinterpret_cast<T const*>(_buf);
    }

    always_inline auto visit(auto visitor) {
        return Meta::indexCast<Ts...>(_index, _buf, visitor);
    }

    always_inline auto visit(auto visitor) const {
        return Meta::indexCast<Ts...>(_index, _buf, visitor);
    }

    always_inline static auto any(auto visitor) {
        return Meta::any<Ts...>(visitor);
    }

    template <Meta::Contains<Ts...> T>
        requires(not Empty<T>)
    always_inline MutCursor<T> is() lifetimebound {
        if (_index != Meta::indexOf<T, Ts...>())
            return nullptr;

        return (T*)_buf;
    }

    template <Meta::Contains<Ts...> T>
        requires(not Empty<T>)
    always_inline Cursor<T> is() const lifetimebound {
        if (_index != Meta::indexOf<T, Ts...>())
            return nullptr;

        return (T const*)_buf;
    }

    template <Meta::Contains<Ts...> T>
        requires(Empty<T>)
    always_inline bool is() const {
        if (_index != Meta::indexOf<T, Ts...>())
            return false;

        return true;
    }

    always_inline usize index() const { return _index; }

    always_inline bool valid() const { return _index < sizeof...(Ts); }

    template <Meta::Contains<Ts...> T>
    std::partial_ordering operator<=>(T const& other) const {
        if constexpr (Meta::Comparable<T>)
            if (is<T>()) {
                if constexpr (not Empty<T>)
                    return unwrap<T>() <=> other;
                return T{} <=> other;
            }
        return std::partial_ordering::unordered;
    }

    template <Meta::Contains<Ts...> T>
        requires Meta::Equatable<T>
    bool operator==(T const& other) const {
        if (is<T>()) {
            if constexpr (not Empty<T>)
                return unwrap<T>() == other;
            return T{} == other;
        }
        return false;
    }

    std::partial_ordering operator<=>(Union const& other) const {
        if (_index == other._index)
            return visit(
                [&]<typename T>(T const& ptr)
                    requires Meta::Comparable<T>
                {
                    if constexpr (not Empty<T>)
                        return ptr <=> other.unwrap<T>();
                    return std::partial_ordering::equivalent;
                }
            );
        return std::partial_ordering::unordered;
    }

    bool operator==(Union const& other) const {
        if (_index == other._index)
            return visit(
                [&]<typename T>(T const& ptr) {
                    if constexpr (not Empty<T>)
                        return ptr == other.unwrap<T>();
                    return true;
                }
            );
        return false;
    }
};

template <typename... Ts>
struct Visitor : Ts... {
    using Ts::operator()...;
};

template <typename... Ts>
Visitor(Ts...) -> Visitor<Ts...>;

template <typename T, typename... Ts>
struct _UnionFlatten {
    using type = T;
};

// If the type is already in the Union we discard it
template <typename... Ts, Meta::Contains<Ts...> A, typename... Us>
struct _UnionFlatten<Union<Ts...>, A, Us...> {
    using type = _UnionFlatten<Union<Ts...>, Us...>::type;
};

// Else we add it to the Union
template <typename... Ts, typename A, typename... Us>
struct _UnionFlatten<Union<Ts...>, A, Us...> {
    using type = _UnionFlatten<Union<Ts..., A>, Us...>::type;
};

template <typename... Ts, typename... Us, typename... Vs>
struct _UnionFlatten<Union<Ts...>, Union<Us...>, Vs...> {
    using type = _UnionFlatten<Union<Ts...>, Us..., Vs...>::type;
};

template <typename... Ts>
using FlatUnion = _UnionFlatten<Union<>, Ts...>::type;

} // namespace Karm
