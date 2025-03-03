#pragma once

#include <karm-gfx/borders.h>
#include <karm-gfx/fill.h>
#include <karm-math/radii.h>

#include "calc.h"
#include "color.h"
#include "length.h"
#include "percent.h"

namespace Vaev {

struct LineWidth {
    enum struct Named {
        THIN,
        MEDIUM,
        THICK,

        _LEN,
    };

    static constexpr Length THIN_VALUE = 1_au;
    static constexpr Length MEDIUM_VALUE = 3_au;
    static constexpr Length THICK_VALUE = 5_au;

    using enum Named;

    using Inner = Union<CalcValue<Length>, Named>;

    Inner _inner;

    template <Meta::Convertible<Inner> T>
    constexpr LineWidth(T&& value)
        : _inner(std::forward<T>(value)) {
    }

    auto visit(this auto& self, auto&& visitor) {
        return self._inner.visit(std::forward<decltype(visitor)>(visitor));
    }

    void repr(Io::Emit& e) const {
        e("{}", _inner);
    }

    bool operator==(LineWidth const&) const = default;

    auto operator<=>(LineWidth const&) const = default;
};

struct Border {
    LineWidth width;
    Gfx::BorderStyle style;
    Color color = Color::CURRENT;

    void repr(Io::Emit& e) const {
        e("(border {} {} {})", width, style, color);
    }
};

struct BorderProps {
    Border top, start, bottom, end;
    Math::Radii<CalcValue<PercentOr<Length>>> radii;

    void all(Border b) {
        top = start = bottom = end = b;
    }

    void repr(Io::Emit& e) const {
        e("(borders");
        e(" top={}", top);
        e(" start={}", start);
        e(" bottom={}", bottom);
        e(" end={}", end);
        e(" radii={}", radii);
        e(")");
    }
};

} // namespace Vaev
