#pragma once

#include <karm-meta/id.h>

namespace marK {

// It's not stupid if it works and it's fast.
//
// Requirements:
// - constexpr
// - all values should be named in a way that accessing them
//   is easy e.g. not std::get<0>(tuple) but tuple.v0
// - structured binding support
//
// If you know how do this with less code, while keeping the
// requirements, please do so and send me a PR

template <typename... Ts>
struct Tuple;

template <typename T0, typename T1 = T0>
using Pair = Tuple<T0, T1>;

template <>
struct Tuple<> {
    constexpr static usize len() {
        return 0;
    }

    constexpr auto visit(auto) {
        return true;
    }

    constexpr auto visit(auto) const {
        return true;
    }

    constexpr auto apply(auto) {
        return true;
    }

    constexpr auto apply(auto) const {
        return true;
    }

    template <typename U>
    constexpr U into() const {
        return U{};
    }
};

template <typename _T0>
struct Tuple<_T0> {
    using T0 = _T0;

    T0 v0;

    constexpr static usize len() {
        return 1;
    }

    constexpr auto visit(auto f) {
        return f(v0);
    }

    constexpr auto visit(auto f) const {
        return f(v0);
    }

    constexpr auto apply(auto f) {
        return f(v0);
    }

    constexpr auto apply(auto f) const {
        return f(v0);
    }

    template <typename U>
    constexpr U into() const {
        return U{v0};
    }

    bool operator==(Tuple const&) const = default;
    auto operator<=>(Tuple const&) const = default;
};

template <typename T0>
Tuple(T0) -> Tuple<T0>;

template <typename _T0, typename _T1>
struct Tuple<_T0, _T1> {
    using T0 = _T0;
    using T1 = _T1;

    T0 v0;
    T1 v1;

    constexpr static usize len() {
        return 2;
    }

    constexpr auto visit(auto f) {
        auto res = f(v0);
        if (not res)
            return res;
        return f(v1);
    }

    constexpr auto visit(auto f) const {
        auto res = f(v0);
        if (not res)
            return res;
        return f(v1);
    }

    constexpr auto apply(auto f) {
        return f(v0, v1);
    }

    constexpr auto apply(auto f) const {
        return f(v0, v1);
    }

    template <typename U>
    constexpr U into() const {
        return U{v0, v1};
    }

    bool operator==(Tuple const&) const = default;
    auto operator<=>(Tuple const&) const = default;
};

template <typename _T0, typename _T1>
Tuple(_T0, _T1) -> Tuple<_T0, _T1>;

template <typename _T0, typename _T1, typename _T2>
struct Tuple<_T0, _T1, _T2> {
    using T0 = _T0;
    using T1 = _T1;
    using T2 = _T2;

    T0 v0;
    T1 v1;
    T2 v2;

    constexpr static usize len() {
        return 3;
    }

    constexpr auto visit(auto f) {
        auto res = f(v0);
        if (not res)
            return res;
        res = f(v1);
        if (not res)
            return res;
        return f(v2);
    }

    constexpr auto visit(auto f) const {
        auto res = f(v0);
        if (not res)
            return res;
        res = f(v1);
        if (not res)
            return res;
        return f(v2);
    }

    constexpr auto apply(auto f) {
        return f(v0, v1, v2);
    }

    constexpr auto apply(auto f) const {
        return f(v0, v1, v2);
    }

    template <typename U>
    constexpr U into() const {
        return U{v0, v1, v2};
    }

    bool operator==(Tuple const&) const = default;
    auto operator<=>(Tuple const&) const = default;
};

template <typename _T0, typename _T1, typename _T2>
Tuple(_T0, _T1, _T2) -> Tuple<_T0, _T1, _T2>;

template <typename _T0, typename _T1, typename _T2, typename _T3>
struct Tuple<_T0, _T1, _T2, _T3> {
    using T0 = _T0;
    using T1 = _T1;
    using T2 = _T2;
    using T3 = _T3;

    T0 v0;
    T1 v1;
    T2 v2;
    T3 v3;

    constexpr static usize len() {
        return 4;
    }

    constexpr auto visit(auto f) {
        auto res = f(v0);
        if (not res)
            return res;
        res = f(v1);
        if (not res)
            return res;
        res = f(v2);
        if (not res)
            return res;
        return f(v3);
    }

    constexpr auto visit(auto f) const {
        auto res = f(v0);
        if (not res)
            return res;
        res = f(v1);
        if (not res)
            return res;
        res = f(v2);
        if (not res)
            return res;
        return f(v3);
    }

    constexpr auto apply(auto f) {
        return f(v0, v1, v2, v3);
    }

    constexpr auto apply(auto f) const {
        return f(v0, v1, v2, v3);
    }

    template <typename U>
    constexpr U into() const {
        return U{v0, v1, v2, v3};
    }

    bool operator==(Tuple const&) const = default;
    auto operator<=>(Tuple const&) const = default;
};

template <typename _T0, typename _T1, typename _T2, typename _T3>
Tuple(_T0, _T1, _T2, _T3) -> Tuple<_T0, _T1, _T2, _T3>;

template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4>
struct Tuple<_T0, _T1, _T2, _T3, _T4> {
    using T0 = _T0;
    using T1 = _T1;
    using T2 = _T2;
    using T3 = _T3;
    using T4 = _T4;

    T0 v0;
    T1 v1;
    T2 v2;
    T3 v3;
    T4 v4;

    constexpr static usize len() {
        return 5;
    }

    constexpr auto visit(auto f) {
        auto res = f(v0);
        if (not res)
            return res;
        res = f(v1);
        if (not res)
            return res;
        res = f(v2);
        if (not res)
            return res;
        res = f(v3);
        if (not res)
            return res;
        return f(v4);
    }

    constexpr auto visit(auto f) const {
        auto res = f(v0);
        if (not res)
            return res;
        res = f(v1);
        if (not res)
            return res;
        res = f(v2);
        if (not res)
            return res;
        res = f(v3);
        if (not res)
            return res;
        return f(v4);
    }

    constexpr auto apply(auto f) {
        return f(v0, v1, v2, v3, v4);
    }

    constexpr auto apply(auto f) const {
        return f(v0, v1, v2, v3, v4);
    }

    template <typename U>
    constexpr U into() const {
        return U{v0, v1, v2, v3, v4};
    }

    bool operator==(Tuple const&) const = default;
    auto operator<=>(Tuple const&) const = default;
};

template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4>
Tuple(_T0, _T1, _T2, _T3, _T4) -> Tuple<_T0, _T1, _T2, _T3, _T4>;

template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4, typename _T5>
struct Tuple<_T0, _T1, _T2, _T3, _T4, _T5> {
    using T0 = _T0;
    using T1 = _T1;
    using T2 = _T2;
    using T3 = _T3;
    using T4 = _T4;
    using T5 = _T5;

    T0 v0;
    T1 v1;
    T2 v2;
    T3 v3;
    T4 v4;
    T5 v5;

    constexpr static usize len() {
        return 6;
    }

    constexpr auto visit(auto f) {
        auto res = f(v0);
        if (not res)
            return res;
        res = f(v1);
        if (not res)
            return res;
        res = f(v2);
        if (not res)
            return res;
        res = f(v3);
        if (not res)
            return res;
        res = f(v4);
        if (not res)
            return res;
        return f(v5);
    }

    constexpr auto visit(auto f) const {
        auto res = f(v0);
        if (not res)
            return res;
        res = f(v1);
        if (not res)
            return res;
        res = f(v2);
        if (not res)
            return res;
        res = f(v3);
        if (not res)
            return res;
        res = f(v4);
        if (not res)
            return res;
        return f(v5);
    }

    constexpr auto apply(auto f) {
        return f(v0, v1, v2, v3, v4, v5);
    }

    constexpr auto apply(auto f) const {
        return f(v0, v1, v2, v3, v4, v5);
    }

    template <typename U>
    constexpr U into() const {
        return U{v0, v1, v2, v3, v4, v5};
    }

    bool operator==(Tuple const&) const = default;
    auto operator<=>(Tuple const&) const = default;
};

template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4, typename _T5>
Tuple(_T0, _T1, _T2, _T3, _T4, _T5) -> Tuple<_T0, _T1, _T2, _T3, _T4, _T5>;

template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4, typename _T5, typename _T6>
struct Tuple<_T0, _T1, _T2, _T3, _T4, _T5, _T6> {
    using T0 = _T0;
    using T1 = _T1;
    using T2 = _T2;
    using T3 = _T3;
    using T4 = _T4;
    using T5 = _T5;
    using T6 = _T6;

    T0 v0;
    T1 v1;
    T2 v2;
    T3 v3;
    T4 v4;
    T5 v5;
    T6 v6;

    constexpr static usize len() {
        return 7;
    }

    constexpr auto visit(auto f) {
        auto res = f(v0);
        if (not res)
            return res;
        res = f(v1);
        if (not res)
            return res;
        res = f(v2);
        if (not res)
            return res;
        res = f(v3);
        if (not res)
            return res;
        res = f(v4);
        if (not res)
            return res;
        res = f(v5);
        if (not res)
            return res;
        return f(v6);
    }

    constexpr auto visit(auto f) const {
        auto res = f(v0);
        if (not res)
            return res;
        res = f(v1);
        if (not res)
            return res;
        res = f(v2);
        if (not res)
            return res;
        res = f(v3);
        if (not res)
            return res;
        res = f(v4);
        if (not res)
            return res;
        res = f(v5);
        if (not res)
            return res;
        return f(v6);
    }

    constexpr auto apply(auto f) {
        return f(v0, v1, v2, v3, v4, v5, v6);
    }

    constexpr auto apply(auto f) const {
        return f(v0, v1, v2, v3, v4, v5, v6);
    }

    template <typename U>
    constexpr U into() const {
        return U{v0, v1, v2, v3, v4, v5, v6};
    }

    bool operator==(Tuple const&) const = default;
    auto operator<=>(Tuple const&) const = default;
};

template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4, typename _T5, typename _T6>
Tuple(_T0, _T1, _T2, _T3, _T4, _T5, _T6) -> Tuple<_T0, _T1, _T2, _T3, _T4, _T5, _T6>;

template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4, typename _T5, typename _T6, typename _T7>
struct Tuple<_T0, _T1, _T2, _T3, _T4, _T5, _T6, _T7> {
    using T0 = _T0;
    using T1 = _T1;
    using T2 = _T2;
    using T3 = _T3;
    using T4 = _T4;
    using T5 = _T5;
    using T6 = _T6;
    using T7 = _T7;

    T0 v0;
    T1 v1;
    T2 v2;
    T3 v3;
    T4 v4;
    T5 v5;
    T6 v6;
    T7 v7;

    constexpr static usize len() {
        return 8;
    }

    constexpr auto visit(auto f) {
        auto res = f(v0);
        if (not res)
            return res;
        res = f(v1);
        if (not res)
            return res;
        res = f(v2);
        if (not res)
            return res;
        res = f(v3);
        if (not res)
            return res;
        res = f(v4);
        if (not res)
            return res;
        res = f(v5);
        if (not res)
            return res;
        res = f(v6);
        if (not res)
            return res;
        return f(v7);
    }

    constexpr auto visit(auto f) const {
        auto res = f(v0);
        if (not res)
            return res;
        res = f(v1);
        if (not res)
            return res;
        res = f(v2);
        if (not res)
            return res;
        res = f(v3);
        if (not res)
            return res;
        res = f(v4);
        if (not res)
            return res;
        res = f(v5);
        if (not res)
            return res;
        res = f(v6);
        if (not res)
            return res;
        return f(v7);
    }

    constexpr auto apply(auto f) {
        return f(v0, v1, v2, v3, v4, v5, v6, v7);
    }

    constexpr auto apply(auto f) const {
        return f(v0, v1, v2, v3, v4, v5, v6, v7);
    }

    template <typename U>
    constexpr U into() const {
        return U{v0, v1, v2, v3, v4, v5, v6, v7};
    }

    bool operator==(Tuple const&) const = default;
    auto operator<=>(Tuple const&) const = default;
};

template <typename _T0, typename _T1, typename _T2, typename _T3, typename _T4, typename _T5, typename _T6, typename _T7>
Tuple(_T0, _T1, _T2, _T3, _T4, _T5, _T6, _T7) -> Tuple<_T0, _T1, _T2, _T3, _T4, _T5, _T6, _T7>;

} // namespace Karm
