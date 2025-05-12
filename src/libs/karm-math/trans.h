#pragma once

#include <karm-io/fmt.h>

#include "edge.h"
#include "quad.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
union Trans2 {
    using Scalar = T;

    static constexpr const T ZERO = T{};
    static constexpr const T ONE = T{1};

    struct
    {
        T xx;
        T xy;
        T yx;
        T yy;

        T ox;
        T oy;
    };

    struct
    {
        Vec2<T> x;
        Vec2<T> y;
        Vec2<T> o;
    };

    static Trans2 const IDENTITY;

    static constexpr Trans2 makeRotate(T angle) {
        T c = cos(angle);
        T s = sin(angle);
        return {
            c, -s,
            s, c,
            ZERO, ZERO
        };
    }

    static constexpr Trans2 makeSkew(Vec2<T> v) {
        return {
            ONE, v.x,
            v.y, ONE,
            ZERO, ZERO
        };
    }

    static constexpr Trans2 makeScale(Vec2<T> v) {
        return {
            v.x, ZERO,
            ZERO, v.y,
            ZERO, ZERO
        };
    }

    static constexpr Trans2 makeTranslate(Vec2<T> v) {
        return {
            ONE, ZERO,
            ZERO, ONE,
            v.x, v.y
        };
    }

    Array<T, 6> _els{};

    bool rotated() const {
        return xx * yy - xy * yx < ZERO;
    }

    bool skewed() const {
        return xx * yy - xy * yx != ONE;
    }

    bool scaled() const {
        return xx != ONE or yy != ONE;
    }

    bool translated() const {
        return ox != ZERO or oy != ZERO;
    }

    bool simple() const {
        return not rotated() and not skewed();
    }

    constexpr Trans2()
        : _els{
              ONE, ZERO,
              ZERO, ONE,
              ZERO, ZERO
          } {}

    constexpr Trans2(T xx, T xy, T yx, T yy, T ox, T oy)
        : _els{xx, xy, yx, yy, ox, oy} {}

    constexpr Trans2(Trans2 const& other)
        : _els{other._els} {}

    constexpr Trans2(Trans2&& other)
        : _els{std::move(other._els)} {}

    constexpr Trans2& operator=(Trans2 const& other) {
        _els = other._els;
        return *this;
    }

    constexpr Trans2& operator=(Trans2&& other) {
        _els = std::move(other._els);
        return *this;
    }

    constexpr ~Trans2() {
        _els.~Array();
    }

    constexpr Vec2<T> applyVector(Vec2<T> v) const {
        return {
            v.x * xx + v.y * yx,
            v.x * xy + v.y * yy,
        };
    }

    constexpr Vec2<T> apply(Vec2<T> v) const {
        return applyVector(v) + o;
    }

    constexpr Edge<T> apply(Edge<T> e) const {
        return {apply(e.start), apply(e.end)};
    }

    constexpr Quad<T> apply(Quad<T> q) const {
        return {apply(q.a), apply(q.b), apply(q.c), apply(q.d)};
    }

    constexpr Quad<T> apply(Rect<T> r) const {
        return apply(Quad<T>{r});
    }

    constexpr Trans2 multiply(Trans2 const& other) const {
        Trans2 res = {
            xx * other.xx,
            ZERO,
            ZERO,
            yy * other.yy,
            ox * other.xx + other.ox,
            oy * other.yy + other.oy,
        };

        if (xy != ZERO or yx != ZERO or
            other.xy != ZERO or other.yx != ZERO) {
            res.xx += xy * other.yx;
            res.xy += xx * other.xy + xy * other.yy;
            res.yx += yx * other.xx + yy * other.yx;
            res.yy += yx * other.xy;
            res.ox += oy * other.yx;
            res.oy += ox * other.xy;
        }

        return res;
    }

    constexpr Trans2 rotated(T angle) {
        return multiply(makeRotate(angle));
    }

    constexpr Trans2 skewed(Vec2<T> v) {
        return multiply(makeSkew(v));
    }

    constexpr Trans2 scaled(Vec2<T> v) {
        return multiply(makeScale(v));
    }

    constexpr Trans2 translated(Vec2<T> v) {
        return multiply(makeTranslate(v));
    }

    constexpr Trans2 inverse() const {
        T det = xx * yy - xy * yx;
        return {
            yy / det, -xy / det,
            -yx / det, xx / det,
            -(ox * yy - oy * xy) / det,
            -(oy * xx - ox * yx) / det
        };
    }

    constexpr bool isIdentity(T epsilon = Limits<T>::EPSILON) const {
        return epsilonEq(xx, ONE, epsilon) and
               epsilonEq(yx, ZERO, epsilon) and
               epsilonEq(xy, ZERO, epsilon) and
               epsilonEq(yy, ONE, epsilon) and
               epsilonEq(ox, ZERO, epsilon) and
               epsilonEq(oy, ZERO, epsilon);
    }

    constexpr bool hasNan() const {
        return x.hasNan() or y.hasNan() or o.hasNan();
    }

    constexpr Vec2<T> delta() const {
        return {xx, xy};
    }

    template <typename U>
    constexpr Trans2<U> cast() const {
        return {
            static_cast<U>(xx),
            static_cast<U>(xy),
            static_cast<U>(yx),
            static_cast<U>(yy),
            static_cast<U>(ox),
            static_cast<U>(oy),
        };
    }

    void repr(Io::Emit& e) const {
        e("(trans {} {} {} {} {} {})", xx, xy, yx, yy, ox, oy);
    }
};

template <typename T>
Trans2<T> const Trans2<T>::IDENTITY = {
    ONE, ZERO,
    ZERO, ONE,
    ZERO, ZERO
};

using Trans2i = Trans2<isize>;

using Trans2f = Trans2<f64>;

} // namespace Karm::Math
