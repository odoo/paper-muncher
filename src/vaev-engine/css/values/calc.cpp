module;

#include <karm/macros>

export module Vaev.Engine:css.values.calc;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.serializer;
import :css.values.base;
import :css.values.keywords;
import :css.values.length;
import :css.values.primitives;
import :css.values.unitValue;
import :values.calc;
import :values.keywords;
import :values.unitValue;

using namespace Karm;

namespace Vaev::Css {

Res<> serializeCalculationTree(Io::TextWriter& w, Calc const& root, bool stripParens = false);

// https://drafts.csswg.org/css-values-4/#serialize-a-math-function
export Res<> serializeMathFunction(Io::TextWriter& w, Calc const& root) {
    // TODO
    // 1. If the root of the calculation tree fn represents is an unresolved numeric function
    //    that is not a math function, serialize that function as normal and return the result.

    // 2. If the root of the calculation tree fn represents is a numeric value (number, percentage, or dimension),
    //    and the serialization being produced is of a computed value or later, then clamp the value to the range
    //    allowed for its context (if necessary), then serialize the value as normal and return the result.
    // FIXME: We should pass context to serializers about the stage of the value that is being serialized,
    //        treating everything as a computed value for now.
    if (auto [numeric] = root.is<UnitValue>()) {
        // TODO: Clamping
        return serializeValue(w, numeric);
    }

    // TODO
    // 3. If fn represents an infinite or NaN value:

    // 4. If the calculation tree’s root node is a numeric value, or a calc-operator node,
    //    let s be a string initially containing "calc(".
    //    Otherwise, let s be a string initially containing the name of the root node,
    //    lowercased (such as "sin" or "max"), followed by a "(" (open parenthesis).
    // 5. For each child of the root node, serialize the calculation tree. If a result of this
    //    serialization starts with a "(" (open parenthesis) and ends with a ")" (close parenthesis),
    //    remove those characters from the result. Concatenate all of the results using ", " (comma followed by space),
    //    then append the result to s.
    try$(root.visit(
        [&](FnNode const& f) -> Res<> {
            bool first = true;

            switch (f.name) {
            case FnNode::Name::ROUND_NEAREST:
                try$(w.writeStr("round("s));
                break;
            case FnNode::Name::ROUND_UP:
                try$(w.writeStr("round(up"s));
                first = false;
                break;
            case FnNode::Name::ROUND_DOWN:
                try$(w.writeStr("round(down"s));
                first = false;
                break;
            case FnNode::Name::ROUND_TO_ZERO:
                try$(w.writeStr("round(to-zero"s));
                first = false;
                break;
            default:
                try$(serializeValue(w, f.name));
                try$(w.writeRune('('));
                break;
            }

            for (auto const& child : f.children) {
                if (first) {
                    first = false;
                } else {
                    try$(w.writeStr(", "s));
                }

                try$(serializeCalculationTree(w, child, true));
            }

            return Ok();
        },
        [&](auto const& other) -> Res<> {
            try$(w.writeStr("calc("s));
            return serializeCalculationTree(w, other, true);
        }
    ));

    // 6. Append ")" (close parenthesis) to s.
    try$(w.writeRune(')'));

    // 7. Return s.
    return Ok();
}

// https://drafts.csswg.org/css-values-4/#serialize-a-calculation-tree
Res<> serializeCalculationTree(Io::TextWriter& w, Calc const& root, bool stripParens) {
    // 1. Let root be the root node of the calculation tree.

    return root.visit(
        // 2. If root is a numeric value, or a non-math function,
        [&](UnitValue const& numeric) -> Res<> {
            // serialize root per the normal rules for it and return the result.
            return serializeValue(w, numeric);
        },
        // 4. If root is a Negate node:
        [&](NegateNode const& negate) -> Res<> {
            // 1. Let s be a string initially containing "(-1 * ".
            if (not stripParens)
                try$(w.writeRune('('));
            try$(w.writeStr("-1 * "s));

            // 2. Serialize root’s child, and append it to s.
            try$(serializeCalculationTree(w, *negate.child));

            // 3. Append ")" to s, then return it.
            if (not stripParens)
                return w.writeRune(')');

            return Ok();
        },
        // 5. If root is an Invert node:
        [&](InvertNode const& invert) -> Res<> {
            // 1. Let s be a string initially containing "(1 / ".
            if (not stripParens)
                try$(w.writeRune('('));
            try$(w.writeStr("1 / "s));

            // 2. Serialize root’s child, and append it to s.
            try$(serializeCalculationTree(w, *invert.child));

            // 3. Append ")" to s, then return it.
            if (not stripParens)
                return w.writeRune(')');

            return Ok();
        },
        // 6. If root is a Sum node:
        [&](SumNode const& sum) -> Res<> {
            // 1. Let s be a string initially containing "(".
            if (not stripParens)
                try$(w.writeRune('('));

            // 2. Sort root’s children.
            // NOTE: Children are already sorted by simplification step.

            // 3. Serialize root’s first child, and append it to s.
            if (isEmpty(sum.children))
                return Error::invalidData("serializing sum with no children");
            try$(serializeCalculationTree(w, sum.children[0]));

            // 4. For each child of root beyond the first:
            for (usize i = 1; i < sum.children.len(); i++) {
                auto const& child = sum.children[i];

                // 1. If child is a Negate node,
                if (auto [negate] = child.is<NegateNode>()) {
                    // append " - " to s,
                    try$(w.writeStr(" - "s));

                    // then serialize the Negate’s child and append the result to s.
                    try$(serializeCalculationTree(w, *negate.child));
                }

                // 2. If child is a negative numeric value,
                else if (auto numeric = child.is<UnitValue>(); numeric and numeric->value() < 0) {
                    // append " - " to s,
                    try$(w.writeStr(" - "s));

                    // then serialize the negation of child as normal and append the result to s.
                    try$(serializeCalculationTree(w, UnitValue{-numeric->value(), numeric->unit()}));
                }

                // 3. Otherwise,
                else {
                    // append " + " to s,
                    try$(w.writeStr(" + "s));

                    // then serialize child and append the result to s.
                    try$(serializeCalculationTree(w, child));
                }
            }

            // 5. Append ")" to s and return it.
            if (not stripParens)
                return w.writeRune(')');

            return Ok();
        },
        // 7. If root is a Product node:
        [&](ProductNode const& product) -> Res<> {
            // 1. Let s be a string initially containing "(".
            if (not stripParens)
                try$(w.writeRune('('));

            // 2. Sort root’s children.
            // NOTE: Children are already sorted by simplification step.

            // 3. Serialize root’s first child, and append it to s.
            if (isEmpty(product.children))
                return Error::invalidData("serializing product with no children");
            try$(serializeCalculationTree(w, product.children[0]));

            // 4. For each child of root beyond the first:
            for (usize i = 1; i < product.children.len(); i++) {
                auto const& child = product.children[i];

                // 1. If child is an Invert node,
                if (auto [invert] = child.is<InvertNode>()) {
                    // append " / " to s,
                    try$(w.writeStr(" / "s));

                    // then serialize the Invert’s child and append the result to s.
                    try$(serializeCalculationTree(w, *invert.child));
                }

                // 2. Otherwise,
                else {
                    // append " * " to s,
                    try$(w.writeStr(" * "s));

                    // then serialize child and append the result to s.
                    try$(serializeCalculationTree(w, child));
                }
            }

            // 5. Append ")" to s and return it.
            if (not stripParens)
                return w.writeRune(')');

            return Ok();
        },
        // 3. If root is anything but a Sum, Negate, Product, or Invert node,
        [&](auto const& other) -> Res<> {
            //  serialize a math function for the function corresponding to the node type,
            //  treating the node’s children as the function’s comma-separated calculation arguments,
            //  and return the result.
            return serializeMathFunction(w, other);
        }
    );
}

export template <Opt<NumericBaseType> TYPE, AcceptsPercent WITH_PERCENT = AcceptsPercent::NO>
struct TypedCalc {
    static_assert(WITH_PERCENT == AcceptsPercent::NO or TYPE);

    Calc _calc;

    TypedCalc(Unchecked, Calc calc) : _calc(std::move(calc)) {}

    static TypedCalc fromCanonical(f64 value) {
        return {UNCHECKED, UnitValue{value, UnitValue::canonicalUnitOf(TYPE)}};
    }

    static TypedCalc fromPercentage(f64 value)
        requires(WITH_PERCENT == AcceptsPercent::YES)
    {
        return {UNCHECKED, UnitValue{value, UnitValue::Unit::PERCENT}};
    }

    Calc const& calcTree() const {
        return _calc;
    }

    void repr(Io::Emit& e) const {
        e("{}", _calc);
    }
};

// https://drafts.csswg.org/css-values-4/#numbers
export using Number = TypedCalc<NONE>;

// https://drafts.csswg.org/css-values-4/#lengths
export using Length = TypedCalc<Some(NumericBaseType::LENGTH)>;

// https://drafts.csswg.org/css-values-4/#angles
export using Angle = TypedCalc<Some(NumericBaseType::ANGLE)>;

// https://drafts.csswg.org/css-values-4/#frequency
export using Frequency = TypedCalc<Some(NumericBaseType::FREQUENCY)>;

// https://drafts.csswg.org/css-values-4/#resolution
export using Resolution = TypedCalc<Some(NumericBaseType::RESOLUTION)>;

// https://drafts.csswg.org/css-values-4/#percentages
export using Percentage = TypedCalc<Some(NumericBaseType::PERCENT)>;

// https://drafts.csswg.org/css-values-4/#mixed-percentages
export using LengthPercentage = TypedCalc<Some(NumericBaseType::LENGTH), AcceptsPercent::YES>;
export using FrequencyPercentage = TypedCalc<Some(NumericBaseType::FREQUENCY), AcceptsPercent::YES>;
export using AnglePercentage = TypedCalc<Some(NumericBaseType::ANGLE), AcceptsPercent::YES>;
export using TimePercentage = TypedCalc<Some(NumericBaseType::TIME), AcceptsPercent::YES>;

template <>
struct ValueParser<Calc> {
};

template <Opt<NumericBaseType> TYPE, AcceptsPercent WITH_PERCENT>
struct ValueParser<TypedCalc<TYPE, WITH_PERCENT>> {
    static Res<TypedCalc<TYPE, WITH_PERCENT>> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto numeric = parseValue<TypedUnitValue<TYPE, WITH_PERCENT>>(c);
        if (numeric) {
            return Ok(TypedCalc<TYPE, WITH_PERCENT>{UNCHECKED, numeric.take().unitValue()});
        }

        if (c.peek() == Sst::FUNC) {
            auto save = c;
            auto res = _parseTypedMathFunction(c);

            if (not res)
                c = save;
            return res;
        }

        return Error::invalidData();
    }

    // https://drafts.csswg.org/css-values-4/#calc-syntax
    static Res<TypedCalc<TYPE, WITH_PERCENT>> _parseTypedMathFunction(Cursor<Sst>& c) {
        auto calc = try$(parseMathFunction(c));

        auto type = determineType(calc, WITH_PERCENT == AcceptsPercent::YES ? TYPE : NONE);
        if (not type)
            return Error::invalidData("calc() type error");

        if (!type->matches(TYPE, WITH_PERCENT == AcceptsPercent::YES))
            return Error::invalidData("calc() does not produce the right type");

        return Ok(TypedCalc<TYPE, WITH_PERCENT>{UNCHECKED, std::move(calc)});
    }

    struct ParamParser {
        Cursor<Sst> _c;
        bool _isFirst = true;

        bool _atParamEnd() {
            auto c = _c;
            eatWhitespace(c);
            return c.ended() or c.peek() == Token::COMMA;
        }

        auto _param(auto f) -> decltype(f()) {
            auto save = *this;

            eatWhitespace(_c);
            if (not _isFirst) {
                if (not _c.skip(Token::COMMA))
                    return Error::invalidData("expected comma in math function");
                eatWhitespace(_c);
            }

            auto res = f();
            if (res and not _atParamEnd())
                res = Error::invalidData("unexpected tokens in math function parameter");

            if (not res) {
                *this = save;
                return res;
            }

            _isFirst = false;
            return res;
        }

        bool ended() {
            eatWhitespace(_c);
            return _c.ended();
        }

        Res<> end() {
            if (not ended())
                return Error::invalidData("unexpected tokens in math function");
            return Ok();
        }

        // <calc-sum>
        Res<Calc> sum() {
            return _param([&] {
                return parseCalcSum(_c);
            });
        }

        // [ <calc-sum> | none ]
        Res<Opt<Calc>> sumOrNone() {
            return _param([&] -> Res<Opt<Calc>> {
                if (parseValue<Keywords::None>(_c))
                    return Ok(NONE);
                return Ok(Some(try$(parseCalcSum(_c))));
            });
        }

        // <calc-sum>#
        Res<Vec<Calc>> sums() {
            Vec<Calc> calcs;
            do {
                calcs.pushBack(try$(sum()));
            } while (not ended());
            return Ok(std::move(calcs));
        }

        template <typename T>
        Opt<T> maybe() {
            return _param([&] {
                return parseValue<T>(_c);
            }).ok();
        }
    };

    using RoundingStrategy = Union<Keywords::Nearest, Keywords::Up, Keywords::Down, Keywords::ToZero>;

    static Res<Calc> parseMathFunction(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto const parseFn = [](Str prefix, ParamParser& params) -> Res<Calc> {
            // <calc()>  = calc( <calc-sum> )
            if (eqCi(prefix, "calc("s))
                return params.sum();

            // <min()>   = min( <calc-sum># )
            if (eqCi(prefix, "min("s))
                return Ok(FnNode::min(try$(params.sums())));

            // <max()>   = max( <calc-sum># )
            if (eqCi(prefix, "max("s))
                return Ok(FnNode::max(try$(params.sums())));

            // <clamp()> = clamp( [ <calc-sum> | none ], <calc-sum>, [ <calc-sum> | none ] )
            if (eqCi(prefix, "clamp("s)) {
                auto min = try$(params.sumOrNone());
                auto val = try$(params.sum());
                auto max = try$(params.sumOrNone());

                if (min and max)
                    return Ok(FnNode::clamp(min.take(), std::move(val), max.take()));

                // clamp(MIN, VAL, none) is equivalent to max(MIN, VAL)
                if (min and not max)
                    return Ok(FnNode::max({min.take(), std::move(val)}));

                // clamp(none, VAL, MAX) is equivalent to min(VAL, MAX)
                if (not min and max)
                    return Ok(FnNode::min({std::move(val), max.take()}));

                // clamp(none, VAL, none) is equivalent to just calc(VAL)
                return Ok(std::move(val));
            }

            // <round()> = round( <rounding-strategy>?, <calc-sum>, <calc-sum>? )
            if (eqCi(prefix, "round("s)) {
                auto strategy = params.template maybe<RoundingStrategy>();

                auto a = try$(params.sum());
                Opt<Calc> b = NONE;
                if (not params.ended())
                    b = Some(try$(params.sum()));

                if (not strategy)
                    return Ok(FnNode::roundNearest(std::move(a), std::move(b)));

                return Ok(strategy->visit(
                    [&](Keywords::Nearest) {
                        return FnNode::roundNearest(std::move(a), std::move(b));
                    },
                    [&](Keywords::Up) {
                        return FnNode::roundUp(std::move(a), std::move(b));
                    },
                    [&](Keywords::Down) {
                        return FnNode::roundDown(std::move(a), std::move(b));
                    },
                    [&](Keywords::ToZero) {
                        return FnNode::roundToZero(std::move(a), std::move(b));
                    }
                ));
            }

            // <mod()>   = mod( <calc-sum>, <calc-sum> )
            if (eqCi(prefix, "mod("s)) {
                auto a = try$(params.sum());
                auto b = try$(params.sum());
                return Ok(FnNode::mod(std::move(a), std::move(b)));
            }

            // <rem()>   = rem( <calc-sum>, <calc-sum> )
            if (eqCi(prefix, "rem("s)) {
                auto a = try$(params.sum());
                auto b = try$(params.sum());
                return Ok(FnNode::rem(std::move(a), std::move(b)));
            }

            // <sin()>   = sin( <calc-sum> )
            if (eqCi(prefix, "sin("s))
                return Ok(FnNode::sin(try$(params.sum())));

            // <cos()>   = cos( <calc-sum> )
            if (eqCi(prefix, "cos("s))
                return Ok(FnNode::cos(try$(params.sum())));

            // <tan()>   = tan( <calc-sum> )
            if (eqCi(prefix, "tan("s))
                return Ok(FnNode::tan(try$(params.sum())));

            // <asin()>  = asin( <calc-sum> )
            if (eqCi(prefix, "asin("s))
                return Ok(FnNode::asin(try$(params.sum())));

            // <acos()>  = acos( <calc-sum> )
            if (eqCi(prefix, "acos("s))
                return Ok(FnNode::acos(try$(params.sum())));

            // <atan()>  = atan( <calc-sum> )
            if (eqCi(prefix, "atan("s))
                return Ok(FnNode::atan(try$(params.sum())));

            // <atan2()> = atan2( <calc-sum>, <calc-sum> )
            if (eqCi(prefix, "atan2("s)) {
                auto a = try$(params.sum());
                auto b = try$(params.sum());
                return Ok(FnNode::atan2(std::move(a), std::move(b)));
            }

            // <pow()>   = pow( <calc-sum>, <calc-sum> )
            if (eqCi(prefix, "pow("s)) {
                auto a = try$(params.sum());
                auto b = try$(params.sum());
                return Ok(FnNode::pow(std::move(a), std::move(b)));
            }

            // <sqrt()>  = sqrt( <calc-sum> )
            if (eqCi(prefix, "sqrt("s))
                return Ok(FnNode::sqrt(try$(params.sum())));

            // <hypot()> = hypot( <calc-sum># )
            if (eqCi(prefix, "hypot("s))
                return Ok(FnNode::hypot(try$(params.sums())));

            // <log()>   = log( <calc-sum>, <calc-sum>? )
            if (eqCi(prefix, "log("s)) {
                auto a = try$(params.sum());
                if (params.ended())
                    return Ok(FnNode::log(std::move(a), NONE));
                return Ok(FnNode::log(std::move(a), Some(try$(params.sum()))));
            }

            // <exp()>   = exp( <calc-sum> )
            if (eqCi(prefix, "exp("s))
                return Ok(FnNode::exp(try$(params.sum())));

            // <abs()>   = abs( <calc-sum> )
            if (eqCi(prefix, "abs("s))
                return Ok(FnNode::abs(try$(params.sum())));

            // <sign()>  = sign( <calc-sum> )
            if (eqCi(prefix, "sign("s))
                return Ok(FnNode::sign(try$(params.sum())));

            return Error::invalidData("unknown math function");
        };

        if (c.peek() != Sst::FUNC)
            return Error::invalidData("expected function");

        auto params = ParamParser{c.peek().content};
        auto fn = try$(parseFn(c.peek().prefix.expect()->token.data, params));
        try$(params.end());
        c.next();
        return Ok(std::move(fn));
    }

    static bool _isWhitespace(Cursor<Sst> const& c) {
        return not c.ended() and c.peek() == Token::WHITESPACE;
    }

    static bool _isOperator(Cursor<Sst> const& c, auto... ops) {
        return not c.ended() and c.peek() == Sst::TOKEN and oneOf(c.peek().token.data, ops...);
    }

    // <calc-sum> = <calc-product> [ [ '+' | '-' ] <calc-product> ]*
    // https://drafts.csswg.org/css-values-4/#calc-syntax
    // In addition, whitespace is required on both sides of the + and - operators.
    // (The * and / operators can be used without white space around them.)
    static Res<Calc> parseCalcSum(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        eatWhitespace(c);

        auto fst = try$(parseCalcProduct(c));
        Vec<Calc> children{};

        while (true) {
            auto save = c;
            bool wsBefore = _isWhitespace(c);
            eatWhitespace(c);

            if (not _isOperator(c, "+", "-")) {
                c = save;
                break;
            }

            if (not wsBefore)
                return Error::invalidData("missing whitespace before + or - in calc()");

            bool negate = c.peek().token.data == "-";
            c.next();

            if (not _isWhitespace(c))
                return Error::invalidData("missing whitespace after + or - in calc()");

            auto term = try$(parseCalcProduct(c));

            if (isEmpty(children))
                children.pushBack(std::move(fst));

            if (negate)
                children.pushBack(NegateNode{makeBox<Calc>(std::move(term))});
            else
                children.pushBack(std::move(term));
        }

        if (isEmpty(children))
            return Ok(std::move(fst));

        return Ok(SumNode{std::move(children)});
    }

    // <calc-product> = <calc-value> [ [ '*' | / ] <calc-value> ]*
    static Res<Calc> parseCalcProduct(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        eatWhitespace(c);

        auto fst = try$(parseCalcValue(c));
        Vec<Calc> children{};

        while (true) {
            auto save = c;
            eatWhitespace(c);

            if (not _isOperator(c, "*", "/")) {
                c = save;
                break;
            }

            bool invert = c.peek().token.data == "/";
            c.next();

            auto factor = try$(parseCalcValue(c));

            if (isEmpty(children))
                children.pushBack(std::move(fst));

            if (invert)
                children.pushBack(InvertNode{makeBox<Calc>(std::move(factor))});
            else
                children.pushBack(std::move(factor));
        }

        if (isEmpty(children))
            return Ok(std::move(fst));

        return Ok(ProductNode{std::move(children)});
    }

    // <calc-value> = <number> | <dimension> | <percentage> |
    //           <calc-keyword> | ( <calc-sum> ) | <math-function>
    static Res<Calc> parseCalcValue(Cursor<Sst>& c) {
        eatWhitespace(c);

        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (auto unitValue = parseValue<UnitValue>(c)) {
            return Ok(unitValue.take());
        }

        if (c.peek() == Sst::BLOCK and c.peek().token == Token::LEFT_PARENTHESIS) {
            Cursor<Sst> content = c.peek().content;
            auto sum = try$(parseCalcSum(content));

            eatWhitespace(content);
            if (not content.ended())
                return Error::invalidData("unexpected tokens");

            c.next();
            return Ok(sum);
        }

        if (c.peek() == Sst::FUNC) {
            return parseMathFunction(c);
        }

        return parseCalcKeyword(c);
    }

    // <calc-keyword> = e | pi | infinity | -infinity | NaN
    static Res<UnitValue> parseCalcKeyword(Cursor<Sst>& c) {
        f64 value = 0.0;
        if (c.skip(Token::ident("e"))) {
            value = Math::E;
        } else if (c.skip(Token::ident("pi"))) {
            value = Math::PI;
        } else if (c.skip(Token::ident("infinity"))) {
            value = Math::INF;
        } else if (c.skip(Token::ident("-infinity"))) {
            value = Math::NEG_INF;
        } else if (c.skip(Token::ident("NaN"))) {
            // FIXME: NaN should be matched with exact casing.
            value = Math::NAN;
        } else {
            return Error::invalidData("unknown calc keyword");
        }

        return Ok(UnitValue{value, UnitValue::Unit::NONE});
    }
};

template <Opt<NumericBaseType> TYPE, AcceptsPercent WITH_PERCENT>
struct ValueSerializer<TypedCalc<TYPE, WITH_PERCENT>> {
    static Res<> serialize(Io::TextWriter& w, TypedCalc<TYPE, WITH_PERCENT> const& value) {
        return serializeMathFunction(w, value.calcTree());
    }
};

} // namespace Vaev::Css
