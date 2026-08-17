module;

#include <karm/macros>

export module Vaev.Engine:values.calc;

import Karm.Core;

import :css;
import :values.base;
import :values.length;
import :values.percent;
import :values.primitives;

using namespace Karm;

namespace Vaev {

// https://drafts.csswg.org/css-values/#typedef-rounding-strategy
export enum struct RoundingStrategy {
    NEAREST, // default
    UP,
    DOWN,
    TO_ZERO,
    LINE_WIDTH,

    _LEN,
};

// https://drafts.csswg.org/css-values/#typedef-calc-keyword
export enum struct CalcKeyword {
    E,
    PI,
    INFINITY,
    MINUS_INFINITY,
    NAN,

    _LEN,
};

// https://drafts.csswg.org/css-values/#typedef-calc-sum
export enum struct CalcOp {
    NOP,

    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    SIN,
    TAN,
    COS,

    _LEN
};

// 10. Mathematical Expressions
// https://drafts.csswg.org/css-values/#math
export template <typename... Ts>
struct Calc {
    using Primary = Meta::First<Ts...>;
    using Scalar = Meta::ListUniq<Union<Number, Ts...>>;

    struct Unary {
        CalcOp op = CalcOp::NOP;
        Box<Calc> val;
    };

    struct Binary {
        CalcOp op = CalcOp::NOP;
        Box<Calc> lhs;
        Box<Calc> rhs;
    };

    // NOTE: Order matters here, as we want to match Number first, then Ts..., then Unary, then Binary.
    using Inner = Meta::ListUniq<Union<
        Number,
        Ts...,
        Unary,
        Binary>>;

    Inner _inner;

    always_inline constexpr Calc()
        : Calc(Primary{}) {
    }

    always_inline constexpr Calc(CanonicalUnit<Primary> val)
        : _inner(Primary{val}) {
    }

    template <Meta::Contains<Ts...> U>
    always_inline constexpr Calc(U val)
        : _inner(val) {
    }

    template <Meta::Convertible<Primary> U>
    always_inline constexpr Calc(U val)
        requires(not Meta::Same<Primary, U>)
        : _inner(Primary{val}) {
    }

    always_inline constexpr Calc(Scalar val)
        : _inner(val) {
    }

    always_inline constexpr Calc(CalcOp op, Calc val)
        : _inner(Unary(op, val)) {
    }

    always_inline constexpr Calc(CalcOp op, Calc lhs, Calc rhs)
        : _inner(Binary(op, lhs, rhs)) {
    }

    always_inline constexpr Opt<Union<Ts...>> value() const {
        return _inner.visit(
            [&](Meta::Contains<Ts...> auto const& v) -> Opt<Union<Ts...>> {
                return v;
            },
            [&](auto const&) {
                return NONE;
            }
        );
    }

    always_inline constexpr Opt<Percent> percent() const {
        return _inner.visit(
            [&](Percent const& v) -> Opt<Percent> {
                return v;
            },
            [&](auto const&) {
                return NONE;
            }
        );
    }

    always_inline auto visit(this auto& self, auto visitor) {
        return self._inner.visit(visitor);
    }

    always_inline auto visit(this auto& self, auto&&... visitors) {
        return self.visit(Visitor{std::forward<decltype(visitors)>(visitors)...});
    }

    void repr(Io::Emit& e) const {
        visit(
            [&](Meta::Contains<Ts..., Number> auto const& v) {
                e("{}", v);
            },
            [&](Unary const& u) {
                e("(calc {} {})", u.op, u.val);
            },
            [&](Binary const& b) {
                e("(calc {} {} {})", b.op, b.lhs, b.rhs);
            }
        );
    }
};

template <typename T>
T _resolveUnary(CalcOp, T) {
    notImplemented();
}

template <typename T>
T _resolveInfix(CalcOp op, T lhs, T rhs) {
    switch (op) {
    case CalcOp::ADD:
        return lhs + rhs;
    case CalcOp::SUBTRACT:
        return lhs - rhs;
    case CalcOp::MULTIPLY:
        // NOTE: Normally, direct multiplication on Au is not allowed.
        //       However, CSS calc() treats pixel units as floating-point numbers,
        //       permitting all standard math operations between them.
        if constexpr (Meta::Same<T, Au>) {
            return Au(f64{lhs} * f64{rhs});
        } else {
            return lhs * rhs;
        }
    case CalcOp::DIVIDE:
        // NOTE: Normally, direct multiplication on Au is not allowed.
        //       However, CSS calc() treats pixel units as floating-point numbers,
        //       permitting all standard math operations between them.
        if constexpr (Meta::Same<T, Au>) {
            return Au(f64{lhs} / f64{rhs});
        } else {
            return lhs / rhs;
        }
    default:
        panic("unexpected operator");
    }
}

export template <typename Primary, typename... Ts, typename... Args>
CanonicalUnit<Primary> resolve(Calc<Primary, Ts...> const& calc, Args const&... args) {
    return calc.visit(
        [&](Meta::Contains<Primary, Ts..., Number> auto const& v) -> CanonicalUnit<Primary> {
            return CanonicalUnit<Primary>{resolve(v, args...)};
        },
        [&](typename Calc<Primary, Ts...>::Unary const& u) -> CanonicalUnit<Primary> {
            return _resolveUnary<CanonicalUnit<Primary>>(
                u.op,
                resolve(*u.val, args...)
            );
        },
        [&](typename Calc<Primary, Ts...>::Binary const& b) -> CanonicalUnit<Primary> {
            return _resolveInfix<CanonicalUnit<Primary>>(
                b.op,
                resolve(*b.lhs, args...),
                resolve(*b.rhs, args...)
            );
        }
    );
}

export template <typename... Ts>
struct ValueTraits<Calc<Ts...>> {
    static Res<Calc<Ts...>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Sst::FUNC) {
            auto const& prefix = c.peek().prefix;
            auto prefixToken = prefix.unwrap()->token;
            if (prefixToken.data == "calc(") {
                Cursor<Css::Sst> content = c.peek().content;
                auto lhs = try$(parseScalar(content));

                auto op = parseOp(content);
                if (not op) {
                    c.next();
                    return Ok(Calc<Ts...>{lhs});
                }

                eatWhitespace(content);
                auto rhs = try$(parseScalar(content));

                c.next();
                return Ok(Calc<Ts...>{op.unwrap(), lhs, rhs});
            }
        }

        return Ok(try$(parseValue<Union<Ts...>>(c)));
    }

    static Res<CalcOp> parseOp(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::WHITESPACE) {
            c.next();
            return parseOp(c);
        }

        if (c.peek().token.data == "+") {
            c.next();
            return Ok(CalcOp::ADD);
        } else if (c.peek().token.data == "-") {
            c.next();
            return Ok(CalcOp::SUBTRACT);
        } else if (c.peek().token.data == "*") {
            c.next();
            return Ok(CalcOp::MULTIPLY);
        } else if (c.peek().token.data == "/") {
            c.next();
            return Ok(CalcOp::DIVIDE);
        }
        return Error::invalidData("unexpected operator");
    }

    static Res<typename Calc<Ts...>::Scalar> parseScalar(Cursor<Css::Sst>& c) {
        return parseValue<typename Calc<Ts...>::Scalar>(c);
    }
};

} // namespace Vaev
