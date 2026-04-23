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
    using ComputedType = CalcValue<typename ValueTraits<T>::ComputedType>;

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

    static ComputedType compute(CalcValue<T> const& calc, ComputationContext const& ctx) {
        using InValue = typename CalcValue<T>::Value;
        using InUnary = typename CalcValue<T>::Unary;
        using InBinary = typename CalcValue<T>::Binary;

        auto valueVisitor = Visitor{
            [&](auto const& val) -> ComputedType {
                return ComputedType(typename ComputedType::Value(computeValue(val, ctx)));
            },
            [&](Box<CalcValue<T>> const& val) -> ComputedType {
                auto computedInner = compute(*val, ctx);
                return ComputedType(typename ComputedType::Value(makeBox<ComputedType>(computedInner)));
            },
            [&](Number const& val) -> ComputedType {
                return ComputedType(typename ComputedType::Value(val));
            },
        };

        return calc.visit(Visitor{
            [&](InValue const& v) -> ComputedType {
                return v.visit(valueVisitor);
            },
            [&](InUnary const& u) -> ComputedType {
                ComputedType computed_val = compute(CalcValue<T>(u.val), ctx);

                auto leafVal = typename ComputedType::Value(makeBox<ComputedType>(computed_val));
                return ComputedType(u.op, leafVal);
            },
            [&](InBinary const& b) -> ComputedType {
                ComputedType computed_lhs = compute(CalcValue<T>(b.lhs), ctx);
                ComputedType computed_rhs = compute(CalcValue<T>(b.rhs), ctx);

                auto leafLhs = typename ComputedType::Value(makeBox<ComputedType>(computed_lhs));
                auto leafRhs = typename ComputedType::Value(makeBox<ComputedType>(computed_rhs));

                return ComputedType(b.op, leafLhs, leafRhs);
            },
        });
    }

    static CalcValue<T> fromComputed(ComputedType const& computed) {
        using InValue = Computed<CalcValue<T>>::Value;
        using InUnary = Computed<CalcValue<T>>::Unary;
        using InBinary = Computed<CalcValue<T>>::Binary;

        auto valueVisitor = Visitor{
            [&](auto const& val) -> CalcValue<T> {
                return CalcValue<T>(typename CalcValue<T>::Value(valueFromComputed<T>(val)));
            },
            [&](Box<ComputedType> const& val) -> CalcValue<T> {
                auto inner = fromComputed(*val);
                return CalcValue<T>(typename CalcValue<T>::Value(makeBox<CalcValue<T>>(inner)));
            },
            [&](Number const& val) -> CalcValue<T> {
                return CalcValue<T>(typename CalcValue<T>::Value(val));
            },
        };

        return computed.visit(Visitor{
            [&](InValue const& v) -> CalcValue<T> {
                return v.visit(valueVisitor);
            },
            [&](InUnary const& u) -> CalcValue<T> {
                CalcValue<T> val = fromComputed(ComputedType(u.val));

                auto leafVal = typename CalcValue<T>::Value(makeBox<CalcValue<T>>(val));
                return CalcValue<T>(u.op, leafVal);
            },
            [&](InBinary const& b) -> CalcValue<T> {
                CalcValue<T> computed_lhs = fromComputed(ComputedType(b.lhs));
                CalcValue<T> computed_rhs = fromComputed(ComputedType(b.rhs));

                auto leafLhs = typename CalcValue<T>::Value(makeBox<CalcValue<T>>(computed_lhs));
                auto leafRhs = typename CalcValue<T>::Value(makeBox<CalcValue<T>>(computed_rhs));

                return CalcValue<T>(b.op, leafLhs, leafRhs);
            },
        });
    }
};

} // namespace Vaev
