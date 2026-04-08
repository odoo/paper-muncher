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
        return self._inner.visit(Visitor{
            [&](Value const& v) {
                return visitor(v);
            },
            [&](Box<Unary> const& u) {
                return visitor(*u);
            },
            [&](Box<Binary> const& b) {
                return visitor(*b);
            },
        });
    }

    void repr(Io::Emit& e) const {
        visit(Visitor{
            [&](Value const& v) {
                e("{}", v);
            },
            [&](Unary const& u) {
                e("(calc {} {})", u.op, u.val);
            },
            [&](Binary const& b) {
                e("(calc {} {} {})", b.op, b.lhs, b.rhs);
            },
        });
    }
};

export template <typename T>
struct ValueTraits<CalcValue<T>> : DefaultValueTraits<CalcValue<T>> {
    using Computed = CalcValue<typename ValueTraits<T>::Computed>;

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

    static Computed compute(CalcValue<T> const& calc, ComputationContext const& ctx) {
        using InValue = typename CalcValue<T>::Value;
        using InUnary = typename CalcValue<T>::Unary;
        using InBinary = typename CalcValue<T>::Binary;

        auto valueVisitor = Visitor{
            [&](auto const& val) -> Computed {
                return Computed(typename Computed::Value(computeValue(val, ctx)));
            },
            [&](Box<CalcValue<T>> const& val) -> Computed {
                auto computedInner = compute(*val, ctx);
                return Computed(typename Computed::Value(makeBox<Computed>(computedInner)));
            },
            [&](Number const& val) -> Computed {
                return Computed(typename Computed::Value(val));
            },
        };

        return calc.visit(Visitor{
            [&](InValue const& v) -> Computed {
                return v.visit(valueVisitor);
            },
            [&](InUnary const& u) -> Computed {
                Computed computed_val = compute(CalcValue<T>(u.val), ctx);

                auto leafVal = typename Computed::Value(makeBox<Computed>(computed_val));
                return Computed(u.op, leafVal);
            },
            [&](InBinary const& b) -> Computed {
                Computed computed_lhs = compute(CalcValue<T>(b.lhs), ctx);
                Computed computed_rhs = compute(CalcValue<T>(b.rhs), ctx);

                auto leafLhs = typename Computed::Value(makeBox<Computed>(computed_lhs));
                auto leafRhs = typename Computed::Value(makeBox<Computed>(computed_rhs));

                return Computed(b.op, leafLhs, leafRhs);
            },
        });
    }
};

export template <typename T>
struct ComputedValueTraits<CalcValue<T>> {
    using Resolved = __Resolved<T>;

    static Resolved resolve(CalcValue<T> const& calc, Opt<Au> relative) {
        auto resolveUnion = Visitor{
            [&](T const& v) {
                return resolveValue(v, relative);
            },
            [&](CalcValue<T>::Leaf const& v) {
                return resolveValue(*v, relative);
            },
            [&](Number const& v)
                requires(not Meta::Same<T, Number>)
            {
                // HACK: Special case for Au
                if constexpr (Meta::Same<__Resolved<T>, Au>) {
                    return Au(v);
                } else {
                    return __Resolved<T>{v};
                }
            }
            };

        return calc.visit(Visitor{
            [&](typename CalcValue<T>::Value const& v) {
                return v.visit(resolveUnion);
            },
            [&](typename CalcValue<T>::Unary const& u) {
                return _resolveUnary(
                    u.op,
                    u.val.visit(resolveUnion)
                );
            },
            [&](typename CalcValue<T>::Binary const& b) {
                return _resolveInfix(
                    b.op,
                    b.lhs.visit(resolveUnion),
                    b.rhs.visit(resolveUnion)
                );
            },
        });
    }

    static Resolved _resolveUnary(CalcOp, Resolved) {
        notImplemented();
    }

    static Resolved _resolveInfix(CalcOp op, Resolved lhs, Resolved rhs) {
        switch (op) {
        case CalcOp::ADD:
            return lhs + rhs;
        case CalcOp::SUBTRACT:
            return lhs - rhs;
        case CalcOp::MULTIPLY:
            // HACK: Bypass dimensions restrictions
            if constexpr (Meta::Same<__Resolved<T>, Au>) {
                return Au(f64{lhs} * f64{rhs});
            } else {
                return lhs * rhs;
            }
        case CalcOp::DIVIDE:
            // HACK: Bypass dimensions restrictions
            if constexpr (Meta::Same<__Resolved<T>, Au>) {
                return Au(f64{lhs} / f64{rhs});
            } else {
                return lhs / rhs;
            }
        default:
            panic("unexpected operator");
        }
    }
};

} // namespace Vaev
