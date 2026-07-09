module;

#include <karm/macros>

export module Vaev.Engine:values.calc;

import Karm.Core;

import :css;
import :values.base;
import :values.length;
import :values.primitives;

using namespace Karm;

namespace Vaev {

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
export template <typename T>
struct CalcValue {
    using Leaf = Box<CalcValue<T>>;

    using Value = Union<
        T,
        Leaf,
        Number>;

    struct Unary {
        CalcOp op = CalcOp::NOP;
        Value val;
    };

    struct Binary {
        CalcOp op = CalcOp::NOP;
        Value lhs;
        Value rhs;
    };

    using Inner = Union<
        Value,
        Box<Unary>,
        Box<Binary>>;

    Inner _inner;

    constexpr CalcValue()
        : CalcValue(T{}) {
    }

    constexpr CalcValue(Meta::Convertible<T> auto val)
        : _inner(Value{T{val}}) {
    }

    constexpr CalcValue(Value val)
        : _inner(val) {
    }

    constexpr CalcValue(CalcOp op, Value val)
        : _inner(makeBox<Unary>(op, val)) {
    }

    constexpr CalcValue(CalcOp op, Value lhs, Value rhs)
        : _inner(makeBox<Binary>(op, lhs, rhs)) {
    }

    auto visit(this auto& self, auto visitor) {
        return self._inner.visit(
            [&](Value const& v) {
                return visitor(v);
            },
            [&](Box<Unary> const& u) {
                return visitor(*u);
            },
            [&](Box<Binary> const& b) {
                return visitor(*b);
            }
        );
    }

    always_inline auto visit(this auto& self, auto&&... visitors) {
        return self.visit(Visitor{std::forward<decltype(visitors)>(visitors)...});
    }

    void repr(Io::Emit& e) const {
        visit(
            [&](Value const& v) {
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

export template <typename T>
struct _Resolved<CalcValue<T>> {
    using Type = _Resolved<T>;
};

template <typename T>
Resolved<T> _resolveUnary(CalcOp, Resolved<T>) {
    notImplemented();
}

template <typename T>
Resolved<T> _resolveInfix(CalcOp op, Resolved<T> lhs, Resolved<T> rhs) {
    switch (op) {
    case CalcOp::ADD:
        return lhs + rhs;
    case CalcOp::SUBTRACT:
        return lhs - rhs;
    case CalcOp::MULTIPLY:
        // NOTE: Normally, direct multiplication on Au is not allowed.
        //       However, CSS calc() treats pixel units as floating-point numbers,
        //       permitting all standard math operations between them.
        if constexpr (Meta::Same<Resolved<T>, Au>) {
            return Au(f64{lhs} * f64{rhs});
        } else {
            return lhs * rhs;
        }
    case CalcOp::DIVIDE:
        // NOTE: Normally, direct multiplication on Au is not allowed.
        //       However, CSS calc() treats pixel units as floating-point numbers,
        //       permitting all standard math operations between them.
        if constexpr (Meta::Same<Resolved<T>, Au>) {
            return Au(f64{lhs} / f64{rhs});
        } else {
            return lhs / rhs;
        }
    default:
        panic("unexpected operator");
    }
}

export template <typename T, typename... Args>
Resolved<T> resolve(CalcValue<T> const& calc, Args... args) {
    auto resolveUnion = Visitor{
        [&](T const& v) {
            return resolve(v, args...);
        },
        [&](CalcValue<T>::Leaf const& v) {
            return resolve<T>(*v, args...);
        },
        [&](Number const& v)
            requires(not Meta::Same<T, Number>)
        {
            return Resolved<T>{v};
        }
        };

    return calc.visit(
        [&](typename CalcValue<T>::Value const& v) {
            return v.visit(resolveUnion);
        },
        [&](typename CalcValue<T>::Unary const& u) {
            return _resolveUnary<T>(
                u.op,
                u.val.visit(resolveUnion)
            );
        },
        [&](typename CalcValue<T>::Binary const& b) {
            return _resolveInfix<T>(
                b.op,
                b.lhs.visit(resolveUnion),
                b.rhs.visit(resolveUnion)
            );
        }
    );
}

export template <typename T>
struct ValueParser<CalcValue<T>> {
    static Res<CalcValue<T>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Sst::FUNC) {
            auto const& prefix = c.peek().prefix;
            auto prefixToken = prefix.unwrap()->token;
            if (prefixToken.data == "calc(") {
                Cursor<Css::Sst> content = c.peek().content;
                auto lhs = try$(parseVal(content));

                auto op = parseOp(content);
                if (not op) {
                    c.next();
                    return Ok(CalcValue<T>{lhs});
                }

                eatWhitespace(content);
                auto rhs = try$(parseVal(content));

                c.next();
                return Ok(CalcValue<T>{op.unwrap(), lhs, rhs});
            }
        }

        return Ok(try$(parseValue<T>(c)));
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

    static Res<typename CalcValue<T>::Value> parseVal(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek().token == Css::Token::NUMBER) {
            return Ok(try$(parseValue<Number>(c)));
        }

        return Ok(try$(parseValue<T>(c)));
    }
};

} // namespace Vaev
