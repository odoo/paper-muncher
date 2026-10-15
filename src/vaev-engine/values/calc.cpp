module;

#include <karm/macros>

export module Vaev.Engine:values.calc;

import Karm.Core;

import :css;
import :values.base;
import :values.dimension;
import :values.length;
import :values.primitives;
import :values.resolved;

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

// 10. MARK: Mathematical Expressions
// https://drafts.csswg.org/css-values/#math

// An untyped math expression, every operand is a dimension.
export struct CalcExpr {
    struct Unary;
    struct Binary;

    using Inner = Union<
        Dimension,
        Box<Unary>,
        Box<Binary>>;

    Inner _inner;

    CalcExpr(Dimension dim)
        : _inner(dim) {}

    CalcExpr(CalcOp op, CalcExpr val);

    CalcExpr(CalcOp op, CalcExpr lhs, CalcExpr rhs);

    auto visit(this auto& self, auto visitor) {
        return self._inner.visit(
            [&](Dimension const& d) {
                return visitor(d);
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

    // Returns the operand if the expression is made of a single one.
    Opt<Dimension> dimension() const {
        if (auto dim = _inner.is<Dimension>())
            return Some(*dim);
        return NONE;
    }

    void repr(Io::Emit& e) const;
};

struct CalcExpr::Unary {
    CalcOp op = CalcOp::NOP;
    CalcExpr val;
};

struct CalcExpr::Binary {
    CalcOp op = CalcOp::NOP;
    CalcExpr lhs;
    CalcExpr rhs;
};

CalcExpr::CalcExpr(CalcOp op, CalcExpr val)
    : _inner(makeBox<Unary>(op, std::move(val))) {}

CalcExpr::CalcExpr(CalcOp op, CalcExpr lhs, CalcExpr rhs)
    : _inner(makeBox<Binary>(op, std::move(lhs), std::move(rhs))) {}

void CalcExpr::repr(Io::Emit& e) const {
    visit(
        [&](Dimension const& d) {
            e("{}", d);
        },
        [&](Unary const& u) {
            e("(calc {} {})", u.op, u.val);
        },
        [&](Binary const& b) {
            e("(calc {} {} {})", b.op, b.lhs, b.rhs);
        }
    );
}

// https://drafts.csswg.org/css-values/#calc-type-checking
// NOTE: This is a simplified version of the type checking algorithm, percentages
//       take the type they resolve against (the percent hint) when there is one.
Res<Dimension::Type> _typeOf(CalcExpr const& expr, Opt<Dimension::Type> percentHint) {
    return expr.visit(
        [&](Dimension const& d) -> Res<Dimension::Type> {
            if (d.type() == Dimension::PERCENTAGE and percentHint)
                return Ok(*percentHint);
            return Ok(d.type());
        },
        [&](CalcExpr::Unary const& u) -> Res<Dimension::Type> {
            auto type = try$(_typeOf(u.val, percentHint));
            if (type != Dimension::NUMBER and type != Dimension::ANGLE)
                return Error::invalidData("expected a number or an angle");
            return Ok(Dimension::NUMBER);
        },
        [&](CalcExpr::Binary const& b) -> Res<Dimension::Type> {
            auto lhs = try$(_typeOf(b.lhs, percentHint));
            auto rhs = try$(_typeOf(b.rhs, percentHint));
            switch (b.op) {
            case CalcOp::ADD:
            case CalcOp::SUBTRACT:
                if (lhs != rhs)
                    return Error::invalidData("adding values of different types");
                return Ok(lhs);
            case CalcOp::MULTIPLY:
                if (lhs == Dimension::NUMBER)
                    return Ok(rhs);
                if (rhs == Dimension::NUMBER)
                    return Ok(lhs);
                return Error::invalidData("multiplying two values with a unit");
            case CalcOp::DIVIDE:
                if (rhs != Dimension::NUMBER)
                    return Error::invalidData("dividing by a value with a unit");
                return Ok(lhs);
            default:
                return Error::invalidData("unexpected operator");
            }
        }
    );
}

// Evaluates the expression in canonical units, `operand` gives
// the canonical value of each dimension of the expression.
f64 _evalCalc(CalcExpr const& expr, auto const& operand) {
    return expr.visit(
        [&](Dimension const& d) -> f64 {
            return operand(d);
        },
        [&](CalcExpr::Unary const&) -> f64 {
            notImplemented();
        },
        [&](CalcExpr::Binary const& b) -> f64 {
            auto lhs = _evalCalc(b.lhs, operand);
            auto rhs = _evalCalc(b.rhs, operand);
            switch (b.op) {
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
    );
}

// A math expression resolving to one of Ts, the first one being the type it
// resolves to (eg. Calc<Length, Percent> for <length-percentage>).
export template <typename... Ts>
struct Calc {
    using First = Meta::First<Ts...>;

    CalcExpr _expr;

    static constexpr bool accepts(Dimension::Type type) {
        return ((type == dimensionTypeOf<Ts>) or ...);
    }

    static constexpr Opt<Dimension::Type> percentHint() {
        if (accepts(Dimension::PERCENTAGE) and dimensionTypeOf<First> != Dimension::PERCENTAGE)
            return Some(dimensionTypeOf<First>);
        return NONE;
    }

    static Res<> typeCheck(CalcExpr const& expr) {
        if (not accepts(try$(_typeOf(expr, percentHint()))))
            return Error::invalidData("unexpected calc() type");
        return Ok();
    }

    template <typename U>
    static Dimension _wrap(U const& val) {
        if constexpr (Meta::Contains<U, Ts...>) {
            return toDimension(val);
        } else {
            Opt<Dimension> result;
            (
                [&] {
                    if constexpr (Meta::Convertible<U, Ts>)
                        if (not result)
                            result = Some(toDimension(Ts(val)));
                }(),
                ...);
            return result.take();
        }
    }

    constexpr Calc()
        : _expr(Dimension{0, Dimension::canonicalUnitOf(dimensionTypeOf<First>)}) {}

    template <typename U>
        requires(Meta::Convertible<U, Ts> or ...)
    constexpr Calc(U const& val)
        : _expr(_wrap(val)) {}

    constexpr explicit Calc(CalcExpr expr)
        : _expr(std::move(expr)) {}

    constexpr Calc(CalcOp op, Calc lhs, Calc rhs)
        : _expr(op, std::move(lhs._expr), std::move(rhs._expr)) {}

    // Returns the operand if the expression is made of a single operand of type U.
    template <typename U>
    Opt<U> is() const {
        if (auto dim = _expr.dimension(); dim and dim->type() == dimensionTypeOf<U>)
            return Some(fromDimension<U>(*dim));
        return NONE;
    }

    void repr(Io::Emit& e) const {
        e("{}", _expr);
    }
};

export template <typename... Ts>
struct _Resolved<Calc<Ts...>> {
    using Type = Resolved<Meta::First<Ts...>>;
};

// Turns a value in the canonical unit of T into its resolved value.
template <typename T>
Resolved<T> _resolveCanonical(f64 value) {
    if constexpr (Meta::Same<Resolved<T>, Au>) {
        return Au::fromFloatNearest(value);
    } else {
        auto dim = Dimension{value, Dimension::canonicalUnitOf(dimensionTypeOf<T>)};
        return resolve(fromDimension<T>(dim));
    }
}

// Resolves an expression made of dimensions that don't depend on a context.
export template <typename... Ts>
Resolved<Calc<Ts...>> resolve(Calc<Ts...> const& calc) {
    auto value = _evalCalc(calc._expr, [](Dimension const& d) {
        return d.canonical().value();
    });
    return _resolveCanonical<Meta::First<Ts...>>(value);
}

export template <typename... Ts>
Resolved<Calc<Ts...>> resolve(Calc<Ts...> const& calc, LengthContext auto const& ctx) {
    auto value = _evalCalc(calc._expr, [&](Dimension const& d) {
        if (d.type() == Dimension::LENGTH and not d.isAbsolute())
            return resolve(Length{d}, ctx).template cast<f64>();
        return d.canonical().value();
    });
    return _resolveCanonical<Meta::First<Ts...>>(value);
}

// Resolves an expression whose percentages are relative to `basis`.
export template <typename... Ts>
Resolved<Calc<Ts...>> resolve(Calc<Ts...> const& calc, Resolved<Calc<Ts...>> basis) {
    auto canonicalBasis = toDimension(basis).canonical().value();
    auto value = _evalCalc(calc._expr, [&](Dimension const& d) {
        if (d.type() == Dimension::PERCENTAGE)
            return canonicalBasis * d.value() / 100.;
        return d.canonical().value();
    });
    return _resolveCanonical<Meta::First<Ts...>>(value);
}

// MARK: Computed Value --------------------------------------------------------
// https://drafts.csswg.org/css-values/#calc-computed-value

// Simplifies an operation between two operands in canonical units.
Opt<Dimension> _foldInfix(CalcOp op, Dimension lhs, Dimension rhs) {
    switch (op) {
    case CalcOp::ADD:
        if (lhs.unit() == rhs.unit())
            return Some(Dimension{lhs.value() + rhs.value(), lhs.unit()});
        return NONE;
    case CalcOp::SUBTRACT:
        if (lhs.unit() == rhs.unit())
            return Some(Dimension{lhs.value() - rhs.value(), lhs.unit()});
        return NONE;
    case CalcOp::MULTIPLY:
        if (lhs.type() == Dimension::NUMBER)
            return Some(Dimension{lhs.value() * rhs.value(), rhs.unit()});
        if (rhs.type() == Dimension::NUMBER)
            return Some(Dimension{lhs.value() * rhs.value(), lhs.unit()});
        return NONE;
    case CalcOp::DIVIDE:
        if (rhs.type() == Dimension::NUMBER)
            return Some(Dimension{lhs.value() / rhs.value(), lhs.unit()});
        return NONE;
    default:
        return NONE;
    }
}

// Every operand is converted to its canonical unit and the operations that
// don't depend on a percentage basis are simplified away, what is left is
// resolved during layout once the basis is known.
CalcExpr _computeCalc(CalcExpr const& expr, LengthContext auto const& ctx) {
    return expr.visit(
        [&](Dimension const& d) -> CalcExpr {
            if (d.type() == Dimension::LENGTH and not d.isAbsolute())
                return Dimension{resolve(Length{d}, ctx).template cast<f64>(), Dimension::Unit::PX};
            return d.canonical();
        },
        [&](CalcExpr::Unary const& u) -> CalcExpr {
            return {u.op, _computeCalc(u.val, ctx)};
        },
        [&](CalcExpr::Binary const& b) -> CalcExpr {
            auto lhs = _computeCalc(b.lhs, ctx);
            auto rhs = _computeCalc(b.rhs, ctx);
            if (auto l = lhs.dimension(), r = rhs.dimension(); l and r)
                if (auto folded = _foldInfix(b.op, *l, *r))
                    return *folded;
            return {b.op, std::move(lhs), std::move(rhs)};
        }
    );
}

export template <typename... Ts>
struct _Computed<Calc<Ts...>> {
    using Type = Calc<Computed<Ts>...>;
};

export template <typename... Ts>
    requires(not Meta::Same<Computed<Calc<Ts...>>, Calc<Ts...>>)
Computed<Calc<Ts...>> compute(Calc<Ts...> const& calc, LengthContext auto const& ctx) {
    return Computed<Calc<Ts...>>{_computeCalc(calc._expr, ctx)};
}

export template <typename... Ts>
struct _Specify<Calc<Ts...>> {
    static Calc<Ts...> from(Calc<Computed<Ts>...> const& calc) {
        return Calc<Ts...>{calc._expr};
    }
};

// A calc() resolving to a <length> doesn't depend on any percentage basis,
// so it computes all the way down to an absolute length.
export template <>
struct _Computed<Calc<Length>> {
    using Type = Au;
};

export Au compute(Calc<Length> const& calc, LengthContext auto const& ctx) {
    return resolve(calc, ctx);
}

export template <>
struct _Specify<Calc<Length>> {
    static Calc<Length> from(Au value) {
        return Length{value};
    }
};

// MARK: Parsing ---------------------------------------------------------------

export template <typename... Ts>
struct ValueParser<Calc<Ts...>> {
    static Res<Calc<Ts...>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Sst::FUNC) {
            auto const& prefix = c.peek().prefix;
            auto prefixToken = prefix.expect()->token;
            if (prefixToken.data == "calc(") {
                Cursor<Css::Sst> content = c.peek().content;
                CalcExpr expr = try$(parseValue<Dimension>(content));

                if (auto op = parseOp(content)) {
                    eatWhitespace(content);
                    auto rhs = try$(parseValue<Dimension>(content));
                    expr = CalcExpr{op.take(), std::move(expr), rhs};
                }

                try$(Calc<Ts...>::typeCheck(expr));
                c.next();
                return Ok(Calc<Ts...>{std::move(expr)});
            }
        }

        auto value = try$(parseValue<Union<Ts...>>(c));
        return Ok(value.visit([](auto const& v) {
            return Calc<Ts...>{v};
        }));
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
};

// MARK: Serialization ---------------------------------------------------------

export template <typename... Ts>
struct ValueSerializer<Calc<Ts...>> {
    // https://drafts.csswg.org/css-values/#calc-serialize
    static Res<> serialize(Io::TextWriter& w, Calc<Ts...> const& value) {
        // NOTE: The current implementation is a rough approximation of the spec.
        if (auto dim = value._expr.dimension())
            return serializeValue(w, *dim);

        try$(w.writeStr("calc("s));
        try$(serializeCalculationTree(w, value._expr));
        try$(w.writeStr(")"s));
        return Ok();
    }

    static Res<> serializeCalculationTree(Io::TextWriter& w, CalcExpr const& expr) {
        return expr.visit(
            [&](Dimension const& d) -> Res<> {
                return serializeValue(w, d);
            },
            [&](CalcExpr::Unary const& u) -> Res<> {
                auto functionalOp = try$(serializeCalcOp(w, u.op));

                try$(serializeCalculationTree(w, u.val));

                if (functionalOp) {
                    try$(w.writeRune(')'));
                }

                return Ok();
            },
            [&](CalcExpr::Binary const& b) -> Res<> {
                try$(serializeCalculationTree(w, b.lhs));
                try$(w.writeRune(' '));

                auto functionalOp = try$(serializeCalcOp(w, b.op));
                if (functionalOp)
                    return Error::unsupported();

                try$(w.writeRune(' '));
                try$(serializeCalculationTree(w, b.rhs));

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
