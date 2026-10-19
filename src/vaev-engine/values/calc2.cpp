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

    static NumericType simple(NumericBaseType baseType, Opt<NumericBaseType> percentHint = NONE) {
        auto type = NumericType{};
        type._els[toUnderlyingType(baseType)] = 1;
        type.percentHint = percentHint;
        return type;
    }

    bool contains(NumericBaseType baseType) const {
        return _els[toUnderlyingType(baseType)] != 0;
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

    // https://drafts.csswg.org/css-values-4/#css-contain-a-percentage
    bool containsPercentage() const {
        return contains(NumericBaseType::PERCENT) or percentHint;
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

export struct NegateNode;
export struct InvertNode;
export struct SumNode;
export struct ProductNode;
export struct MinNode;
export struct MaxNode;

using Calc = Union<UnitValue, NegateNode, InvertNode, SumNode, ProductNode, MinNode, MaxNode>;

struct NegateNode {
    Box<Calc> child;

    void repr(Io::Emit& e) const {
        e("(NEGATE {})", child);
    }
};

struct InvertNode {
    Box<Calc> child;

    void repr(Io::Emit& e) const {
        e("(INVERT {})", child);
    }
};

struct SumNode {
    Vec<Calc> children;

    void repr(Io::Emit& e) const {
        e("(SUM {})", children);
    }
};

struct ProductNode {
    Vec<Calc> children;

    void repr(Io::Emit& e) const {
        e("(PRODUCT {})", children);
    }
};

struct MinNode {
    Vec<Calc> children;

    void repr(Io::Emit& e) const {
        e("(MIN {})", children);
    }
};

struct MaxNode {
    Vec<Calc> children;

    void repr(Io::Emit& e) const {
        e("(MAX {})", children);
    }
};

// https://drafts.csswg.org/css-values-4/#determine-the-type-of-a-calculation
static Opt<NumericType> determineType(Calc const& calc) {
    auto addTypes = [](Slice<Calc> calculations) -> Opt<NumericType> {
        Opt<NumericType> type = NONE;

        for (auto const& calculation : calculations) {
            if (not type) {
                type = Some(try$(determineType(calculation)));
            } else {
                type = type->add(try$(determineType(calculation)));
            }
        }

        return type;
    };

    auto multiplyTypes = [](Slice<Calc> calculations) -> Opt<NumericType> {
        Opt<NumericType> type = NONE;

        for (auto const& calculation : calculations) {
            if (not type) {
                type = Some(try$(determineType(calculation)));
            } else {
                type = type->multiply(try$(determineType(calculation)));
            }
        }

        return type;
    };

    return calc.visit(
        [&](UnitValue const& value) -> Opt<NumericType> {
            if (auto const& [baseType] = value.baseType()) {
                return Some(NumericType::simple(baseType));
            } else {
                return Some(NumericType{});
            }
        },
        [&](NegateNode const& negate) -> Opt<NumericType> {
            return determineType(*negate.child);
        },
        [&](InvertNode const& invert) -> Opt<NumericType> {
            auto childType = try$(determineType(*invert.child));
            return Some(childType.invert());
        },
        [&](SumNode const& sum) {
            return addTypes(sum.children);
        },
        [&](ProductNode const& product) {
            return multiplyTypes(product.children);
        },
        [&](MinNode const& min) {
            return addTypes(min.children);
        },
        [&](MaxNode const& max) {
            return addTypes(max.children);
        }
    );
}

// https://drafts.csswg.org/css-values-4/#simplify-a-calculation-tree
static Calc simplify(Calc const& calc, LengthContext auto const& ctx) {
    auto simplifyChild = [&](Calc const& child) -> Calc {
        return simplify(child, ctx);
    };

    auto simplifyChildren = [&](Slice<Calc> children) -> Vec<Calc> {
        auto simplified = Vec<Calc>{children.len()};

        for (auto const& child : children) {
            simplified.emplaceBack(simplify(child, ctx));
        }

        return simplified;
    };

    calc.visit(
        // 1. If root is a numeric value:
        [](UnitValue const& numeric) -> Calc {
            // FIXME
            return numeric.toComputed();
        },
        //
        [&](MinNode const& min) -> Calc {
            // 1. For each node child of root’s children:
            for (auto const& child : simplifyChildren(min.children)) {
                if (auto [numeric] = child.template is<UnitValue>()) {
                }
            }

            unreachable();
        },
        // 6. If root is a Negate node:
        [&](NegateNode const& negate) -> Calc {
            auto simplifiedChild = simplifyChild(negate.child);

            return simplifiedChild.visit(
                // 1. If root’s child is a numeric value,
                [](UnitValue const& child) -> Calc {
                    // return an equivalent numeric value, but with the value negated (0 - value).
                    return UnitValue{-child.value(), child.unit()};
                },
                // 2. If root’s child is a Negate node,
                [](NegateNode const& child) -> Calc {
                    // return the child’s child.
                    return std::move(*child.child);
                },
                // 3. If root’s child is a Sum node:
                [&](SumNode& child) -> Calc {
                    // 1. Let negated grandchildren be an empty list
                    auto negatedGrandChildren = Vec<Calc>{child.children.len()};

                    // 2. For each grandchild of the child’s children:
                    for (auto& grandchild : child.children) {
                        // 1. If grandchild is a numeric value,
                        if (auto [numeric] = grandchild.is<UnitValue>()) {
                            // create an equivalent numeric value, but with the value negated (0 - value),
                            // and append the result to negated grandchildren.
                            negatedGrandChildren.pushBack(UnitValue{-numeric.value(), numeric.unit()});
                            continue;
                        }

                        // 2. If grandchild is a Negate node
                        if (auto [negate] = grandchild.is<NegateNode>()) {
                            // append grandchild’s child to negated grandchildren
                            negatedGrandChildren.pushBack(std::move(*negate.child));
                            continue;
                        }

                        // Otherwise, create a Negate node with grandchild as its child,
                        // and append the result to negated grandchildren
                        negatedGrandChildren.pushBack(NegateNode{makeBox<Calc>(std::move(grandchild))});
                    }

                    // 3. Return a Sum node with negated grandchildren as its children
                    return SumNode{std::move(negatedGrandChildren)};
                },
                // Return root.
                [](auto& other) -> Calc {
                    return NegateNode{makeBox<Calc>(std::move(other))};
                }
            );
        },
        // 7. If root is an Invert node:
        [&](InvertNode const& invert) -> Calc {
            auto simplifiedChild = simplifyChild(invert.child);

            // 1. If root’s child is a number (not a percentage or dimension)
            if (auto child = simplifiedChild.template is<UnitValue>; child and not child->baseType()) {
                // return the reciprocal of the child’s value.
                return UnitValue{1.0/child->value(), UnitValue::Unit::NONE};
            }

            // 2. If root’s child is an Invert node,
            if (auto [child] = simplifiedChild.template is<InvertNode>()) {
                // Return root.
                return std::move(*child.child);
            }

            // 3. Return root.
            return InvertNode{makeBox<Calc>(std::move(simplifiedChild))};
        },
        [](auto const&) {
            unreachable();
        }
    );
}

export template <typename T>
struct TypedCalc {
    Calc _inner;

    void repr(Io::Emit& e) const {
        e("{}", _inner);
    }

    // void simplify(LengthContext auto& ctx) {
    // }
};

} // namespace Vaev::Ng

namespace Vaev {

export template <typename T>
struct ValueParser<Ng::TypedCalc<T>> {
    static Res<Ng::TypedCalc<T>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Sst::FUNC) {
            auto calc = try$(parseMathFunction(c));

            auto type = try$(Ng::determineType(calc));

            logInfo("type: {}", type);
            logInfo("Accepts percents: {}", T::ACCEPTS_PERCENTS);
            logInfo("Base type: {}", T::BASE_TYPE);

            if (not type.isBaseType(T::BASE_TYPE) or (type.containsPercentage() and T::ACCEPTS_PERCENTS == AcceptsPercent::NO))
                return Error::invalidData("calc() type error");

            return Ok(Ng::TypedCalc<T>{
                ._inner = std::move(calc),
            });
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
            Cursor<Css::Sst> content = c.peek().content;

            Vec<Ng::Calc> children;

            children.pushBack(try$(parseCalcSum(content)));

            while (not content.ended() and content.peek() == Css::Token::COMMA) {
                content.next();
                children.pushBack(try$(parseCalcSum(content)));
            }

            c.next();

            if (prefixToken.data == "min(")
                return Ok(Ng::MinNode{std::move(children)});
            else
                return Ok(Ng::MaxNode{std::move(children)});
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
                children.pushBack(Ng::NegateNode{makeBox<Ng::Calc>(std::move(term))});
            }

            eatWhitespace(c);
        }

        eatWhitespace(c);

        return Ok(Ng::SumNode{std::move(children)});
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
                children.pushBack(Ng::InvertNode{makeBox<Ng::Calc>(std::move(factor))});
            }

            eatWhitespace(c);
        }

        eatWhitespace(c);

        return Ok(Ng::ProductNode{std::move(children)});
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
        Css::Lexer lex = {"calc(0px - (0px - 2px))"};
        auto diags = Diag::Collector::ignore();

        auto content = Css::consumeComponentValue(lex, diags);
        auto c = Cursor<Css::Sst>(content);
        logInfo("{}", c);

        auto parsed = parseValue<Ng::TypedCalc<Length>>(c);
        logInfo("{}", parsed);


    }
};

static Lab _lab{};

} // namespace Vaev
