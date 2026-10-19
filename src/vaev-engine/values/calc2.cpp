module;

#include <karm/macros>

export module Vaev.Engine:values.calc2;

import Karm.Core;

import :css;
import :values.base;
import :values.primitives;
import :values.unitValue;
import :values.length;
import :values.percent;
import :values.angle;

using namespace Karm;

namespace Vaev::Ng {

struct NumericType {
    Array<i32, toUnderlyingType(NumericBaseType::_LEN)> _els{};

    Opt<NumericBaseType> percentHint = NONE;

    NumericType() = default;

    NumericType(NumericBaseType baseType, i32 exponent = 1) {
        _els[toUnderlyingType(baseType)] = exponent;
    }

    bool contains(NumericBaseType hint) const {
        return _els[toUnderlyingType(hint)] != 0;
    }

    bool isBaseType(NumericBaseType baseType) const {
        for (usize i = 0; i < toUnderlyingType(NumericBaseType::_LEN); i++) {
            if (i == static_cast<usize>(toUnderlyingType(baseType))) {
                if (_els[i] != 1)
                    return false;
            } else {
                if (_els[i] != 0)
                    return false;
            }
        }

        return true;
    }

    bool isEmpty() const {
        for (usize i = 0; i < toUnderlyingType(NumericBaseType::_LEN); i++) {
            if (_els[i] != 0) {
                return false;
            }
        }

        return true;
    }

    // https://drafts.css-houdini.org/css-typed-om-1/#apply-the-percent-hint
    void applyPercentHint(NumericBaseType hint) {
        _els[toUnderlyingType(hint)] += _els[toUnderlyingType(NumericBaseType::PERCENT)];
        _els[toUnderlyingType(NumericBaseType::PERCENT)] = 0;
        percentHint = Some(hint);
    }

    // https://drafts.css-houdini.org/css-typed-om-1/#cssnumericvalue-add-two-types
    Opt<NumericType> add(NumericType const& other) const {
        if (percentHint and other.percentHint and *percentHint != *other.percentHint)
            return NONE;

        auto type1 = *this;
        auto type2 = other;

        if (type1.percentHint and not type2.percentHint) {
            type2.applyPercentHint(*type1.percentHint);
        } else if (type2.percentHint and not type1.percentHint) {
            type1.applyPercentHint(*type2.percentHint);
        }

        if (type1._els == type2._els) {
            auto finalType = type1;
            if (not finalType.percentHint)
                finalType.percentHint = type2.percentHint;
            return Some(finalType);
        }

        bool eitherHasPercent = type1.contains(NumericBaseType::PERCENT) or type2.contains(NumericBaseType::PERCENT);

        if (eitherHasPercent) {
            for (usize i = 0; i < toUnderlyingType(NumericBaseType::_LEN); i++) {
                auto hint = static_cast<NumericBaseType>(i);
                if (hint == NumericBaseType::PERCENT)
                    continue;

                auto t1 = type1;
                auto t2 = type2;
                t1.applyPercentHint(hint);
                t2.applyPercentHint(hint);

                if (t1._els == t2._els) {
                    t1.percentHint = Some(hint);
                    return Some(t1);
                }
            }
        }

        return NONE;
    }

    // https://drafts.css-houdini.org/css-typed-om-1/#cssnumericvalue-multiply-two-types
    Opt<NumericType> multiply(NumericType const& other) {
        if (percentHint and other.percentHint and *percentHint != *other.percentHint) {
            return NONE;
        }

        NumericType finalType = *this;

        for (usize i = 0; i < _els.len(); i++) {
            finalType._els[i] += other._els[i];
        }

        if (not finalType.percentHint) {
            finalType.percentHint = other.percentHint;
        }

        return Some(finalType);
    }

    // https://drafts.css-houdini.org/css-typed-om-1/#cssnumericvalue-invert-a-type
    NumericType invert() const {
        NumericType finalType = *this;

        for (usize i = 0; i < _els.len(); i++) {
            finalType._els[i] *= -1;
        }

        return finalType;
    }

    bool operator==(NumericType const&) const = default;

    always_inline constexpr i32 const& operator[](NumericBaseType baseType) const {
        return _els[toUnderlyingType(baseType)];
    }

    always_inline constexpr i32& operator[](NumericBaseType baseType) {
        return _els[toUnderlyingType(baseType)];
    }

    void repr(Io::Emit& e) const {
        e("(numeric-type {} hint={})", _els, percentHint);
    }
};

export struct Unary;
export struct Nary;

using Calc = Union<UnitValue, Unary, Nary>;

struct Unary {
    enum struct Op {
        NEGATE,
        INVERT,
        _LEN,
    };
    using enum Op;

    Op op;
    Box<Calc> child;

    void repr(Io::Emit& e) const {
        e("({} {})", op, child);
    }
};

struct Nary {
    enum struct Op {
        SUM,
        PRODUCT,
        MIN,
        MAX,
        _LEN,
    };
    using enum Op;

    Op op;
    Vec<Calc> children;

    void repr(Io::Emit& e) const {
        e("({} {})", op, children);
    }
};

// https://drafts.csswg.org/css-values-4/#determine-the-type-of-a-calculation
static Opt<NumericType> determineType(Calc const& calc) {
    return calc.visit(
        [&](UnitValue const& value) -> Opt<NumericType> {
            if (auto const& [baseType] = value.baseType()) {
                return Some(NumericType{baseType});
            } else {
                return Some(NumericType{});
            }
        },
        [&](Unary const& unary) -> Opt<NumericType> {
            if (unary.op == Unary::NEGATE) {
                return determineType(*unary.child);
            } else if (unary.op == Unary::INVERT) {
                auto childType = try$(determineType(*unary.child));
                return Some(childType.invert());
            } else {
                unreachable();
            }
        },
        [&](Nary const& nary) -> Opt<NumericType> {
            if (oneOf(nary.op, Nary::SUM, Nary::MIN, Nary::MAX)) {
                Opt<NumericType> type = NONE;

                for (auto const& child : nary.children) {
                    if (not type) {
                        type = Some(try$(determineType(child)));
                    } else {
                        type = type->add(try$(determineType(child)));
                    }
                }

                return type;
            } else if (nary.op == Nary::PRODUCT) {
                Opt<NumericType> type = NONE;

                for (auto const& child : nary.children) {
                    if (not type) {
                        type = Some(try$(determineType(child)));
                    } else {
                        type = type->multiply(try$(determineType(child)));
                    }
                }

                return type;
            } else {
                unreachable();
            }
        }
    );
}

export template <NumericBaseType TYPE>
TypedUnitValue<TYPE> resolve() {
    
}

}

namespace Vaev {

export template <>
struct ValueParser<Ng::Calc> {
    static Res<Ng::Calc> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Sst::FUNC) {
            auto calc = try$(parseMathFunction(c));
            if (not Ng::determineType(calc))
                return Error::invalidData("type error in calc()");
        }

        return Error::invalidData();
    }


    static Res<Ng::Calc> parseMathFunction(Cursor<Css::Sst>& c) {
        logInfo("ENTER MATH FN: {}", c);
        auto _d = Defer([&]() {
            logInfo("EXIT MATH FN: {}", c);
        });

        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto const& prefix = c.peek().prefix;
        auto prefixToken = prefix.expect()->token;

        // <calc()>  = calc( <calc-sum> )
        if (prefixToken.data == "calc(") {
            Cursor<Css::Sst> content = c.peek().content;
            auto root = try$(parseCalcSum(content));
            c.next();
            return Ok(root);
        }

        // <min()>   = min( <calc-sum># )
        // <max()>   = max( <calc-sum># )
        if (prefixToken.data == "min(" or prefixToken.data == "max(") {
            auto op = prefixToken.data == "min(" ? Ng::Nary::MIN : Ng::Nary::MAX;

            Cursor<Css::Sst> content = c.peek().content;

            Vec<Ng::Calc> children;

            children.pushBack(try$(parseCalcSum(content)));

            while (not content.ended() and content.peek() == Css::Token::COMMA) {
                content.next();
                children.pushBack(try$(parseCalcSum(content)));
            }

            c.next();

            return Ok(Ng::Nary{op, std::move(children)});
        }

        return Error::notImplemented("math function not implemented");
    }

    // <calc-sum> = <calc-product> [ [ '+' | '-' ] <calc-product> ]*
    static Res<Ng::Calc> parseCalcSum(Cursor<Css::Sst>& c) {
        logInfo("ENTER SUM: {}", c);
        auto _d = Defer([&]() {
            logInfo("EXIT SUM: {}", c);
        });

        if (c.ended())
            return Error::invalidData("unexpected end of input");

        eatWhitespace(c);

        auto fst = try$(parseCalcProduct(c));

        eatWhitespace(c);

        if (c.ended() or not oneOf(c.peek().token.data, "+", "-"))
            return Ok(fst);

        Vec<Ng::Calc> children{std::move(fst)};

        while (not c.ended() and oneOf(c.peek().token.data, "+", "-")) {
            if (c.peek().token.data == "+") {
                c.next();
                auto term = try$(parseCalcProduct(c));
                children.pushBack(term);
            } else {
                c.next();
                auto term = try$(parseCalcProduct(c));
                children.pushBack(Ng::Unary{Ng::Unary::NEGATE, makeBox<Ng::Calc>(std::move(term))});
            }

            eatWhitespace(c);
        }

        eatWhitespace(c);

        return Ok(Ng::Nary{Ng::Nary::SUM, std::move(children)});
    }

    // <calc-product> = <calc-value> [ [ '*' | / ] <calc-value> ]*
    static Res<Ng::Calc> parseCalcProduct(Cursor<Css::Sst>& c) {
        logInfo("ENTER PRODUCT: {}", c);
        auto _d = Defer([&]() {
            logInfo("EXIT PRODUCT: {}", c);
        });

        if (c.ended())
            return Error::invalidData("unexpected end of input");

        eatWhitespace(c);

        auto fst = try$(parseCalcValue(c));

        eatWhitespace(c);

        if (c.ended() or not oneOf(c.peek().token.data, "*", "/"))
            return Ok(fst);

        Vec<Ng::Calc> children{std::move(fst)};

        while (not c.ended() and oneOf(c.peek().token.data, "*", "/")) {
            if (c.peek().token.data == "*") {
                c.next();
                auto factor = try$(parseCalcValue(c));
                children.pushBack(factor);
            } else {
                c.next();
                auto factor = try$(parseCalcValue(c));
                children.pushBack(Ng::Unary{Ng::Unary::INVERT, makeBox<Ng::Calc>(std::move(factor))});
            }

            eatWhitespace(c);
        }

        eatWhitespace(c);

        return Ok(Ng::Nary{Ng::Nary::PRODUCT, std::move(children)});
    }

    // <calc-value> = <number> | <dimension> | <percentage> |
    //           <calc-keyword> | ( <calc-sum> ) | <math-function>
    static Res<Ng::Calc> parseCalcValue(Cursor<Css::Sst>& c) {
        logInfo("ENTER VALUE: {}", c);
        auto _d = Defer([&]() {
            logInfo("EXIT VALUE: {}", c);
        });

        if (c.ended())
            return Error::invalidData("unexpected end of input");

        eatWhitespace(c);

        if (auto unitValue = parseValue<UnitValue>(c)) {
            return Ok(unitValue.take());
        }

        if (c.peek() == Css::Sst::BLOCK and c.peek().token == Css::Token::LEFT_PARENTHESIS) {
            Cursor<Css::Sst> content = c.peek().content;
            auto sum = try$(parseCalcSum(content));
            c.next();
            eatWhitespace(c);
            return Ok(sum);
        }

        if (c.peek() == Css::Sst::FUNC) {
            auto func = try$(parseMathFunction(c));
            eatWhitespace(c);
            return Ok(func);
        }

        auto keyword = parseCalcKeyword(c);
        eatWhitespace(c);
        return keyword;
    }

    // <calc-keyword> = e | pi | infinity | -infinity | NaN
    static Res<UnitValue> parseCalcKeyword(Cursor<Css::Sst>& c) {
        Number value = 0.0;
        if (c.skip(Css::Token::ident("e"))) {
            value = Math::E;
        } else if (c.skip(Css::Token::ident("pi"))) {
            value = Math::PI;
        } else if (c.skip(Css::Token::ident("infinity"))) {
            value = Math::INF;
        } else if (c.skip(Css::Token::ident("-infinity"))) {
            value = Math::NEG_INF;
        } else if (c.skip(Css::Token::ident("NaN"))) {
            value = Math::NAN;
        } else {
            return Error::invalidData("unknown calc keyword");
        }

        return Ok(UnitValue{value, UnitValue::Unit::NONE});
    }
};

struct Lab {
    Lab() {
        Css::Lexer lex = {"calc(2px + 1px * (5deg / 2deg))"};
        auto diags = Diag::Collector::ignore();

        auto content = Css::consumeComponentValue(lex, diags);
        auto c = Cursor<Css::Sst>(content);
        logInfo("{}", c);

        auto parsed = parseValue<Ng::Calc>(c);
        logInfo("{}", parsed);

        auto type = Ng::determineType(parsed.expect());
        logInfo("{}", type);
    }
};

static Lab _lab{};

}
