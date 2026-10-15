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
struct Calc {
    using Leaf = Box<Calc<T>>;

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

    constexpr Calc()
        : Calc(T{}) {
    }

    constexpr Calc(Meta::Convertible<T> auto val)
        : _inner(Value{T{val}}) {
    }

    constexpr Calc(Value val)
        : _inner(val) {
    }

    constexpr Calc(CalcOp op, Value val)
        : _inner(makeBox<Unary>(op, val)) {
    }

    constexpr Calc(CalcOp op, Value lhs, Value rhs)
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
struct _Resolved<Calc<T>> {
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
        return lhs * rhs;
    case CalcOp::DIVIDE:
        return lhs / rhs;
    default:
        panic("unexpected operator");
    }
}

export template <typename T, typename... Args>
Resolved<T> resolve(Calc<T> const& calc, Args... args) {
    auto resolveUnion = Visitor{
        [&](T const& v) {
            return resolve(v, args...);
        },
        [&](Calc<T>::Leaf const& v) {
            return resolve<T>(*v, args...);
        },
        [&](Number const& v)
            requires(not Meta::Same<T, Number>)
        {
            return Resolved<T>{v};
        }
        };

    return calc.visit(
        [&](typename Calc<T>::Value const& v) {
            return v.visit(resolveUnion);
        },
        [&](typename Calc<T>::Unary const& u) {
            return _resolveUnary<T>(
                u.op,
                u.val.visit(resolveUnion)
            );
        },
        [&](typename Calc<T>::Binary const& b) {
            return _resolveInfix<T>(
                b.op,
                b.lhs.visit(resolveUnion),
                b.rhs.visit(resolveUnion)
            );
        }
    );
}

export template <typename T>
struct ValueParser<Calc<T>> {
    static Res<Calc<T>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Sst::FUNC) {
            auto const& prefix = c.peek().prefix;
            auto prefixToken = prefix.expect()->token;
            if (prefixToken.data == "calc(") {
                Cursor<Css::Sst> content = c.peek().content;
                auto lhs = try$(parseVal(content));

                auto op = parseOp(content);
                if (not op) {
                    c.next();
                    return Ok(Calc<T>{lhs});
                }

                eatWhitespace(content);
                auto rhs = try$(parseVal(content));

                c.next();
                return Ok(Calc<T>{op.expect(), lhs, rhs});
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

    static Res<typename Calc<T>::Value> parseVal(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek().token == Css::Token::NUMBER) {
            return Ok(try$(parseValue<Number>(c)));
        }

        return Ok(try$(parseValue<T>(c)));
    }
};

export template <typename T>
struct ValueSerializer<Calc<T>> {
    // https://drafts.csswg.org/css-values/#calc-serialize
    static Res<> serialize(Io::TextWriter& w, Calc<T> const& value) {
        // NOTE: The current implementation is a rough approximation of the spec.
        return value.visit(
            [&](typename Calc<T>::Value const& v) -> Res<> {
                return v.visit(Visitor{
                    [&](T const& v) {
                        return serializeValue(w, v);
                    },
                    [&](typename Calc<T>::Leaf const& v) {
                        return serialize(w, *v);
                    },
                    [&](Number const& v) {
                        return serializeValue(w, v);
                    }
                });
            },
            [&](auto const&) -> Res<> {
                try$(w.writeStr("calc("s));
                try$(serializeCalculationTree(w, value));
                try$(w.writeStr(")"s));
                return Ok();
            }
        );
    }

    static Res<> serializeCalculationTree(Io::TextWriter& w, Calc<T> const& value) {
        auto serializeUnion = Visitor{
            [&](T const& v) {
                return serializeValue(w, v);
            },
            [&](Calc<T>::Leaf const& v) {
                return serializeCalculationTree(w, *v);
            },
            [&](Number const& v) {
                return serializeValue(w, v);
            }
        };

        return value.visit(
            [&](typename Calc<T>::Value const& v) -> Res<> {
                return v.visit(serializeUnion);
            },
            [&](typename Calc<T>::Unary const& u) -> Res<> {
                auto functionalOp = try$(serializeCalcOp(w, u.op));

                try$(u.val.visit(serializeUnion));

                if (functionalOp) {
                    try$(w.writeRune(')'));
                }

                return Ok();
            },
            [&](typename Calc<T>::Binary const& b) -> Res<> {
                try$(b.lhs.visit(serializeUnion));
                try$(w.writeRune(' '));

                auto functionalOp = try$(serializeCalcOp(w, b.op));
                if (functionalOp)
                    return Error::unsupported();

                try$(w.writeRune(' '));
                try$(b.rhs.visit(serializeUnion));

                return Ok();
            }
        );
    }

    static Res<bool> serializeCalcOp(Io::TextWriter& w, CalcOp op) {
        switch (op) {
        case CalcOp::NOP:
            return Ok(false);
        case CalcOp::ADD:
            try$(w.writeRune('+'));
            return Ok(false);
        case CalcOp::SUBTRACT:
            try$(w.writeRune('-'));
            return Ok(false);
        case CalcOp::MULTIPLY:
            try$(w.writeRune('*'));
            return Ok(false);
        case CalcOp::DIVIDE:
            try$(w.writeRune('/'));
            return Ok(false);
        case CalcOp::SIN:
            try$(w.writeStr("sin("s));
            return Ok(true);
        case CalcOp::TAN:
            try$(w.writeStr("tan("s));
            return Ok(true);
        case CalcOp::COS:
            try$(w.writeStr("cos("s));
            return Ok(true);
        case CalcOp::_LEN:
            unreachable();
        }
    }
};

} // namespace Vaev
