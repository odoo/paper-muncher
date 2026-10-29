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
import :values.keywords;

using namespace Karm;
using namespace Vaev::Literals;

namespace Vaev::Ng {

struct NumericType {
    Array<i32, toUnderlyingType(NumericBaseType::_LEN)> _exponents{};

    Opt<NumericBaseType> percentHint = NONE;

    NumericType() = default;

    static NumericType simple(NumericBaseType baseType, Opt<NumericBaseType> percentHint = NONE) {
        auto type = NumericType{};
        type._exponents[toUnderlyingType(baseType)] = 1;
        type.percentHint = percentHint;
        return type;
    }

    bool contains(NumericBaseType baseType) const {
        return _exponents[toUnderlyingType(baseType)] != 0;
    }

    bool isBaseType(Opt<NumericBaseType> baseType) const {
        if (not baseType)
            return isEmpty();

        for (usize i = 0; i < toUnderlyingType(NumericBaseType::_LEN); i++) {
            if (i == static_cast<usize>(toUnderlyingType(*baseType))) {
                if (_exponents[i] != 1)
                    return false;
            } else {
                if (_exponents[i] != 0)
                    return false;
            }
        }

        return true;
    }

    bool isEmpty() const {
        for (usize i = 0; i < toUnderlyingType(NumericBaseType::_LEN); i++) {
            if (_exponents[i] != 0) {
                return false;
            }
        }

        return true;
    }

    // https://drafts.css-houdini.org/css-typed-om-1/#apply-the-percent-hint
    void applyPercentHint(NumericBaseType hint) {
        _exponents[toUnderlyingType(hint)] += _exponents[toUnderlyingType(NumericBaseType::PERCENT)];
        _exponents[toUnderlyingType(NumericBaseType::PERCENT)] = 0;
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

        if (type1._exponents == type2._exponents) {
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

                if (t1._exponents == t2._exponents) {
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

        for (usize i = 0; i < _exponents.len(); i++) {
            finalType._exponents[i] += other._exponents[i];
        }

        if (not finalType.percentHint) {
            finalType.percentHint = other.percentHint;
        }

        return Some(finalType);
    }

    // https://drafts.css-houdini.org/css-typed-om-1/#cssnumericvalue-invert-a-type
    NumericType invert() const {
        NumericType finalType = *this;

        for (usize i = 0; i < _exponents.len(); i++) {
            finalType._exponents[i] *= -1;
        }

        return finalType;
    }

    // https://drafts.csswg.org/css-values-4/#css-contain-a-percentage
    bool containsPercentage() const {
        return contains(NumericBaseType::PERCENT) or percentHint;
    }

    // https://drafts.csswg.org/css-values-4/#math-function
    // https://drafts.css-houdini.org/css-typed-om-1/#cssnumericvalue-match
    Opt<NumericBaseType> matchesAnyMathFunction() const {
        isize exponentLocation = -1;
        for (usize i = 0; i < toUnderlyingType(NumericBaseType::_LEN); i++) {
            if (_exponents[i] == 0)
                continue;

            if (_exponents[i] == 1) {
                if (exponentLocation >= 0)
                    return NONE;

                exponentLocation = i;
                continue;
            }

            return NONE;
        }

        // NOTE: A <number> has no base type to return, callers must check isEmpty() first.
        if (exponentLocation < 0)
            return NONE;

        return Some(static_cast<NumericBaseType>(exponentLocation));
    }

    // https://drafts.css-houdini.org/css-typed-om-1/#cssnumericvalue-match
    bool matches(Opt<NumericBaseType> baseType, bool acceptPercents) const {
        // A type is said to match a CSS production in some circumstances:

        // - A type matches <length> if its only non-zero entry is «[ "length" → 1 ]». Similarly for <angle>, <time>,
        //   <frequency>, <resolution>, and <flex>.
        //
        //   If the context in which the value is used allows <percentage> values, and those percentages are resolved
        //   against another type, then for the type to be considered matching it must either have a null percent hint,
        //   or the percent hint must match the other type.
        //
        //   If the context does not allow <percentage> values to be mixed with <length>/etc values (or doesn’t allow
        //   <percentage> values at all, such as border-width), then for the type to be considered matching the percent
        //   hint must be null.

        // TODO
        return false;
    }

    bool operator==(NumericType const&) const = default;

    always_inline constexpr i32 const& operator[](NumericBaseType baseType) const {
        return _exponents[toUnderlyingType(baseType)];
    }

    always_inline constexpr i32& operator[](NumericBaseType baseType) {
        return _exponents[toUnderlyingType(baseType)];
    }

    void repr(Io::Emit& e) const {
        e("(numeric-type {} hint={})", _exponents, percentHint);
    }
};

export struct NegateNode;
export struct InvertNode;
export struct SumNode;
export struct ProductNode;
export struct FnNode;

using Calc = Union<
    UnitValue,
    NegateNode,
    InvertNode,
    SumNode,
    ProductNode,
    FnNode>;

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

struct FnNode {
    enum struct Name {
        MIN,
        MAX,
        CLAMP,
        SIN,
        COS,
        TAN,
        ASIN,
        ACOS,
        ATAN,
        ATAN2,
        POW,
        SQRT,
        HYPOT,
        LOG,
        EXP,
        _LEN,
    };

    Name name;
    Vec<Calc> children;

    FnNode(Unchecked, Name name, Vec<Calc> children) : name(name), children(std::move(children)) {}

    // https://drafts.csswg.org/css-values-4/#funcdef-min
    static FnNode min(Vec<Calc> children) {
        return {UNCHECKED, Name::MIN, std::move(children)};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-max
    static FnNode max(Vec<Calc> children) {
        return {UNCHECKED, Name::MAX, std::move(children)};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-clamp
    static FnNode clamp(Calc min, Calc val, Calc max) {
        return {UNCHECKED, Name::CLAMP, {std::move(min), std::move(val), std::move(max)}};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-sin
    static FnNode sin(Calc a) {
        return {UNCHECKED, Name::SIN, {std::move(a)}};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-cos
    static FnNode cos(Calc a) {
        return {UNCHECKED, Name::COS, {std::move(a)}};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-tan
    static FnNode tan(Calc a) {
        return {UNCHECKED, Name::TAN, {std::move(a)}};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-asin
    static FnNode asin(Calc a) {
        return {UNCHECKED, Name::ASIN, {std::move(a)}};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-acos
    static FnNode acos(Calc a) {
        return {UNCHECKED, Name::ACOS, {std::move(a)}};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-atan
    static FnNode atan(Calc a) {
        return {UNCHECKED, Name::ATAN, {std::move(a)}};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-atan2
    static FnNode atan2(Calc a, Calc b) {
        return {UNCHECKED, Name::ATAN2, {std::move(a), std::move(b)}};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-pow
    static FnNode pow(Calc a, Calc b) {
        return {UNCHECKED, Name::POW, {std::move(a), std::move(b)}};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-sqrt
    static FnNode sqrt(Calc a) {
        return {UNCHECKED, Name::SQRT, {std::move(a)}};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-hypot
    static FnNode hypot(Vec<Calc> children) {
        return {UNCHECKED, Name::HYPOT, std::move(children)};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-log
    static FnNode log(Calc a, Opt<Calc> b) {
        if (b) {
            return {UNCHECKED, Name::LOG, {std::move(a), b.take()}};
        }

        return {UNCHECKED, Name::LOG, {std::move(a)}};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-exp
    static FnNode exp(Calc a) {
        return {UNCHECKED, Name::EXP, {std::move(a)}};
    }

    Opt<UnitValue::Unit> canonicalUnit() const {
        switch (name) {
        case Name::MIN:
        case Name::MAX:
        case Name::CLAMP:
        case Name::HYPOT:
            return NONE;

        case Name::ASIN:
        case Name::ACOS:
        case Name::ATAN:
        case Name::ATAN2:
            return Some(UnitValue::canonicalUnitOf(Some(NumericBaseType::ANGLE)));

        case Name::SIN:
        case Name::COS:
        case Name::TAN:
        case Name::POW:
        case Name::SQRT:
        case Name::LOG:
        case Name::EXP:
            return Some(UnitValue::Unit::NONE);
        case Name::_LEN:
            unreachable();
        }
    }

    void repr(Io::Emit& e) const {
        e("({} {})", name, children);
    }
};

f64 apply(FnNode::Name name, usize count, auto get) {
    using enum FnNode::Name;

    auto const v = [&](usize i) {
        return get(i).value();
    };

    auto const rad = [&](usize i) {
        auto x = get(i);
        if (x.unit() == UnitValue::Unit::DEG)
            return x.value() * Math::DEG2RAD;
        else if (x.unit() == UnitValue::Unit::NONE)
            return x.value();
        else
            panic("unexpected unit");
    };

    switch (name) {
    case MIN: {
        f64 res = v(0);
        for (usize i = 1; i < count; i++)
            res = Karm::min(res, v(i));
        return res;
    }
    case MAX: {
        f64 res = v(0);
        for (usize i = 1; i < count; i++)
            res = Karm::max(res, v(i));
        return res;
    }
    case CLAMP:
        return Karm::max(v(0), Karm::min(v(1), v(2)));
    case SIN:
        return Math::sin(rad(0));
    case COS:
        return Math::cos(rad(0));
    case TAN:
        return Math::tan(rad(0));
    case ASIN:
        return Math::asin(v(0)) * Math::RAD2DEG;
    case ACOS:
        return Math::acos(v(0)) * Math::RAD2DEG;
    case ATAN:
        return Math::atan(v(0)) * Math::RAD2DEG;
    case ATAN2:
        return Math::atan2(v(0), v(1)) * Math::RAD2DEG;
    case POW:
        return Math::pow(v(0), v(1));
    case SQRT:
        return Math::sqrt(v(0));
    case HYPOT: {
        f64 res = Math::abs(v(0));
        for (usize i = 1; i < count; i++)
            res = Math::hypot(res, v(i));
        return res;
    }
    case LOG: {
        f64 res = Math::log(v(0));
        if (count > 1)
            res /= Math::log(v(1));
        return res;
    }
    case EXP:
        return Math::exp(v(0));
    case _LEN:
        unreachable();
    }
}

// https://drafts.csswg.org/css-values-4/#calc-type-checking
export Opt<NumericType> determineType(Calc const& calc, Opt<NumericBaseType> mainType) {
    auto addTypes = [&](Slice<Calc> calculations) -> Opt<NumericType> {
        Opt<NumericType> type = NONE;

        for (auto const& calculation : calculations) {
            if (not type) {
                type = Some(try$(determineType(calculation, mainType)));
            } else {
                type = type->add(try$(determineType(calculation, mainType)));
            }
        }

        return type;
    };

    auto multiplyTypes = [&](Slice<Calc> calculations) -> Opt<NumericType> {
        Opt<NumericType> type = NONE;

        for (auto const& calculation : calculations) {
            if (not type) {
                type = Some(try$(determineType(calculation, mainType)));
            } else {
                type = type->multiply(try$(determineType(calculation, mainType)));
            }
        }

        return type;
    };

    return calc.visit(
        [&](UnitValue const& value) -> Opt<NumericType> {
            if (auto const& [baseType] = value.baseType()) {
                Opt<NumericBaseType> percentHint = NONE;

                // -> <percentage>
                // If, in the context in which the math function containing this calculation is placed,
                // <percentage>s are resolved relative to another type of value (such as in width,
                // where <percentage> is resolved against a <length>), and that other type is not <number>,
                // the type is determined as the other type, but with a percent hint set to that other type.
                // Otherwise, the type is «[ "percent" → 1 ]», with a percent hint of "percent".
                if (baseType == NumericBaseType::PERCENT)
                    percentHint = mainType;

                return Some(NumericType::simple(baseType, percentHint));
            } else {
                return Some(NumericType{});
            }
        },

        // - At a + or - sub-expression, attempt to add the types of the left and right arguments.
        //   If this returns failure, the entire calculation’s type is failure.
        //   Otherwise, the sub-expression’s type is the returned type.
        [&](SumNode const& sum) {
            return addTypes(sum.children);
        },
        [&](NegateNode const& negate) -> Opt<NumericType> {
            return determineType(*negate.child, mainType);
        },

        // - At a * sub-expression, multiply the types of the left and right arguments.
        //   The sub-expression’s type is the returned result.
        [&](ProductNode const& product) {
            return multiplyTypes(product.children);
        },

        // - At a / sub-expression, let left type be the result of finding the types of its left argument,
        //   and right type be the result of finding the types of its right argument and then inverting it.
        //
        //   The sub-expression’s type is the result of multiplying the left type and right type.
        [&](InvertNode const& invert) -> Opt<NumericType> {
            auto childType = try$(determineType(*invert.child, mainType));
            return Some(childType.invert());
        },

        // Math functions themselves have types, according to their contained calculations:
        [&](FnNode const& f) -> Opt<NumericType> {
            switch (f.name) {
            // calc(), abs()
            // The type of its contained calculation.

            // min(), max(), clamp(), hypot(), round(), mod(), rem()
            // The result of adding the types of its comma-separated calculations.
            case FnNode::Name::MIN:
            case FnNode::Name::MAX:
            case FnNode::Name::CLAMP:
            case FnNode::Name::HYPOT: {
                // https://drafts.csswg.org/css-values-4/#comp-func
                // https://drafts.csswg.org/css-values-4/#exponent-funcs
                // The argument calculations can resolve to any <number>, <dimension>, or <percentage>,
                // but must have a consistent type or else the function is invalid;
                return addTypes(f.children);
            }

            // asin(), acos(), atan(), atan2()
            // «[ "angle" → 1 ]».
            case FnNode::Name::ASIN:
            case FnNode::Name::ACOS:
            case FnNode::Name::ATAN: {
                // https://drafts.csswg.org/css-values-4/#trig-funcs
                // The asin(A), acos(A), and atan(A) functions are the "arc" or "inverse"
                // trigonometric functions, representing the inverse function to their
                // corresponding "normal" trig functions. All of them contain a single
                // calculation which must resolve to a <number>
                for (auto const& child : f.children) {
                    if (not try$(determineType(child, mainType)).isBaseType(NONE))
                        return NONE;
                }

                return Some(NumericType::simple(NumericBaseType::ANGLE));
            }
            case FnNode::Name::ATAN2: {
                // https://drafts.csswg.org/css-values-4/#trig-funcs
                // The atan2(A, B) function contains two comma-separated calculations,
                // A and B. A and B can resolve to any <number>, <dimension>, or <percentage>,
                // but must have a consistent type or else the function is invalid
                try$(addTypes(f.children));
                return Some(NumericType::simple(NumericBaseType::ANGLE));
            }
            // TODO: sign()
            // sign(), sin(), cos(), tan(), pow(), sqrt(), log(), exp()
            // «[ ]» (empty map).
            case FnNode::Name::SIN:
            case FnNode::Name::COS:
            case FnNode::Name::TAN: {
                // https://drafts.csswg.org/css-values-4/#trig-funcs
                // The sin(A), cos(A), and tan(A) functions all contain a single calculation
                // which must resolve to either a <number> or an <angle>
                auto childType = try$(determineType(f.children[0], mainType));

                if (not (childType.isBaseType(NONE) or childType.isBaseType(Some(NumericBaseType::ANGLE))))
                    return NONE;

                return Some(NumericType{});
            }
            case FnNode::Name::POW:
            case FnNode::Name::SQRT:
            case FnNode::Name::LOG:
            case FnNode::Name::EXP: {
                // https://drafts.csswg.org/css-values-4/#exponent-funcs
                // The pow(A, B) function contains two comma-separated calculations A and B, both of which must resolve to <number>s
                // The sqrt(A) function contains a single calculation which must resolve to a <number>
                // The log(A, B?) function contains one or two calculations, which must resolve to <number>s
                // The exp(A) function contains one calculation which must resolve to a <number>
                for (auto const& child : f.children) {
                    if (not try$(determineType(child, mainType)).isBaseType(NONE))
                        return NONE;
                }

                return Some(NumericType{});
            }
            case FnNode::Name::_LEN:
                unreachable();
            }
        }
    );
}

// https://drafts.csswg.org/css-values-4/#sort-a-calculations-children
export void sortCalculationChildren(MutSlice<Calc> children) {
    // FIXME: Sort dimension units alphabetically in the .inc file.
    stableSort(children, [](Calc const& a, Calc const& b) {
        auto weight = [](Calc const& calc) {
            if (auto [number] = calc.is<UnitValue>()) {
                return toUnderlyingType(number.unit());
            } else {
                return toUnderlyingType(UnitValue::Unit::_LEN);
            }
        };

        return weight(a) <=> weight(b);
    });
}

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
            try$(serializeValue(w, f.name));
            try$(w.writeRune('('));

            bool first = true;
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

// https://drafts.csswg.org/css-values-4/#simplify-a-calculation-tree
export Calc simplify(Calc const& calc, auto const& ctx, Opt<NumericBaseType> mainType, Opt<f64> canonicalPercentBasis = NONE) {
    auto simplifyChild = [&](Calc const& child) -> Calc {
        return simplify(child, ctx, mainType, canonicalPercentBasis);
    };

    auto simplifyChildren = [&](Slice<Calc> children) -> Vec<Calc> {
        auto simplified = Vec<Calc>{children.len()};

        for (auto const& child : children) {
            simplified.emplaceBack(simplify(child, ctx, mainType, canonicalPercentBasis));
        }

        return simplified;
    };

    auto foldNumericSiblings = [&](Vec<Calc>& children, auto pred, auto op) {
        usize newLen = 0;

        for (usize i = 0; i < children.len(); i++) {
            auto& child = children[i];

            if (auto n = child.is<UnitValue>(); n and newLen > 0) {
                auto& prev = children[newLen - 1];
                if (auto p = prev.is<UnitValue>(); p and p->unit() == n->unit() and pred(p->unit())) {
                    *p = UnitValue{op(p->value(), n->value()), p->unit()};
                    continue;
                }
            }

            if (i != newLen)
                children[newLen] = std::move(child);

            newLen++;
        }

        children.resize(newLen, UnitValue{});
    };

    return calc.visit(
        // 1. If root is a numeric value:
        [&](UnitValue const& numeric) -> Calc {
            // 1. If root is a percentage that will be resolved against another value,
            //    and there is enough information available to resolve it,
            if (numeric.unit() == UnitValue::Unit::PERCENT and canonicalPercentBasis) {
                // do so, and express the resulting numeric value in the appropriate canonical unit. Return the value.
                return UnitValue{(numeric.value() * canonicalPercentBasis.expect()) / 100, UnitValue::canonicalUnitOf(mainType)};
            }

            // 2. If root is a dimension that is not expressed in its canonical unit,
            //    and there is enough information available to convert it to the canonical unit, do so, and return the value.
            // TODO: 3 & 4
            return numeric.canonical(ctx);
        },
        // 4. If root is an operator node that’s not one of the calc-operator nodes, and all of its calculation children
        //    are numeric values with enough information to compute the operation root represents, return the result of
        //    running root’s operation using its children, expressed in the result’s canonical unit.
        [&](FnNode const& f) -> Calc {
            auto simplifiedChildren = simplifyChildren(f.children);

            for (auto const& simplifiedChild : simplifiedChildren) {
                if (auto [numeric] = simplifiedChild.template is<UnitValue>()) {
                    // If a percentage is left at this point, it will usually block simplification of the node,
                    // since it needs to be resolved against another value using information not currently available.
                    // (Otherwise, it would have been converted to a different value in an earlier step.) This includes operations such as "min",
                    // since percentages might resolve against a negative basis, and thus end up with an opposite comparative relationship than
                    // the raw percentage value would seem to indicate.
                    if (numeric.unit() == UnitValue::Unit::PERCENT and not(mainType and mainType.expect() == NumericBaseType::PERCENT))
                        return FnNode{UNCHECKED, f.name, std::move(simplifiedChildren)};
                } else {
                    return FnNode{UNCHECKED, f.name, std::move(simplifiedChildren)};
                }
            }

            auto result = apply(f.name, simplifiedChildren.len(), [&](usize i) {
                return simplifiedChildren[i].template expect<UnitValue>();
            });

            if (auto const& [unit] = f.canonicalUnit())
                return UnitValue{result, unit};

            return UnitValue{result, simplifiedChildren[0].template expect<UnitValue>().unit()};
        },
        // 5. If root is a Min or Max node, attempt to partially simplify it:
        // NOSPEC: We simplified min/max like any other math function in the previous step.

        // 6. If root is a Negate node:
        [&](NegateNode const& negate) -> Calc {
            auto simplifiedChild = simplifyChild(*negate.child);

            return simplifiedChild.visit(
                // 1. If root’s child is a numeric value,
                [](UnitValue& child) -> Calc {
                    // return an equivalent numeric value, but with the value negated (0 - value).
                    return UnitValue{-child.value(), child.unit()};
                },
                // 2. If root’s child is a Negate node,
                [](NegateNode& child) -> Calc {
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
            auto simplifiedChild = simplifyChild(*invert.child);

            // 1. If root’s child is a number (not a percentage or dimension)
            if (auto child = simplifiedChild.template is<UnitValue>(); child and not child->baseType()) {
                // return the reciprocal of the child’s value.
                return UnitValue{1.0 / child->value(), UnitValue::Unit::NONE};
            }

            // 2. If root’s child is an Invert node,
            if (auto [child] = simplifiedChild.template is<InvertNode>()) {
                // Return root.
                return std::move(*child.child);
            }

            // 3. Return root.
            return InvertNode{makeBox<Calc>(std::move(simplifiedChild))};
        },
        // 8. If root is a Sum node:
        [&](SumNode const& sum) -> Calc {
            // 1. For each of root’s children that are Sum nodes, replace them with their children.
            auto simplifiedChildren = Vec<Calc>{sum.children.len()};
            for (auto& child : sum.children) {
                auto simplifiedChild = simplifyChild(child);
                if (auto [child] = simplifiedChild.template is<SumNode>()) {
                    for (auto& grandchild : child.children) {
                        simplifiedChildren.pushBack(std::move(grandchild));
                    }
                } else {
                    simplifiedChildren.pushBack(std::move(simplifiedChild));
                }
            }

            // For each set of root’s children that are numeric values with identical units,
            // remove those children and replace them with a single numeric value containing
            // the sum of the removed nodes, and with the same unit.
            sortCalculationChildren(simplifiedChildren);
            foldNumericSiblings(
                simplifiedChildren,
                [](UnitValue::Unit) {
                    return true;
                },
                [](f64 a, f64 b) -> f64 {
                    return a + b;
                }
            );

            // 3. If root has only a single child at this point,
            if (simplifiedChildren.len() == 1) {
                // return the child.
                return std::move(simplifiedChildren[0]);
            }

            // Otherwise, return root.
            return SumNode{std::move(simplifiedChildren)};
        },
        // 9. If root is a Product node:
        [&](ProductNode const& product) -> Calc {
            // 1. For each of root’s children that are Product nodes, replace them with their children.
            auto simplifiedChildren = Vec<Calc>{product.children.len()};
            for (auto& child : product.children) {
                auto simplifiedChild = simplifyChild(child);
                if (auto [child] = simplifiedChild.template is<ProductNode>()) {
                    for (auto& grandchild : child.children) {
                        simplifiedChildren.pushBack(std::move(grandchild));
                    }
                } else {
                    simplifiedChildren.pushBack(std::move(simplifiedChild));
                }
            }

            // 2. If root has multiple children that are numbers (not percentages or dimensions),
            //    remove them and replace them with a single number containing the product of the removed nodes.
            sortCalculationChildren(simplifiedChildren);
            foldNumericSiblings(
                simplifiedChildren,
                [](UnitValue::Unit unit) {
                    return unit == UnitValue::Unit::NONE;
                },
                [](f64 a, f64 b) -> f64 {
                    return a * b;
                }
            );

            // 3. If root contains only two children, one of which is a number (not a percentage or dimension) and the other
            //    of which is a Sum whose children are all numeric values, multiply all of the Sum’s children by the number,
            //    then return the Sum.
            if (simplifiedChildren.len() == 2) {
                auto const isNumber = [](UnitValue const& numeric) {
                    return numeric.unit() == UnitValue::Unit::NONE;
                };

                auto const onlyNumericChildren = [](SumNode const& sum) {
                    for (auto const& child : sum.children) {
                        if (not child.is<UnitValue>())
                            return false;
                    }
                    return true;
                };

                // NOTE: We know it must happen in this order because children are sorted.
                auto maybeNumber = simplifiedChildren[0].is<UnitValue>();
                auto maybeSum = simplifiedChildren[1].is<SumNode>();

                if (maybeNumber and isNumber(*maybeNumber) and maybeSum and onlyNumericChildren(*maybeSum)) {
                    for (auto& child : maybeSum->children) {
                        auto numeric = *child.is<UnitValue>();
                        child = UnitValue{numeric.value() * maybeNumber->value(), numeric.unit()};
                    }

                    return std::move(*maybeSum);
                }
            }

            // 4. If root contains only numeric values and/or Invert nodes containing numeric values, and multiplying the types of all the children
            //    (noting that the type of an Invert node is the inverse of its child’s type) results in a type that matches any of the types that a math
            //    function can resolve to, return the result of multiplying all the values of the children (noting that the value of an Invert node is the
            //    reciprocal of its child’s value), expressed in the result’s canonical unit.
            bool containsOnlyCanonicalNumerics = true;
            for (auto const& child : simplifiedChildren) {
                if (auto numeric = child.is<UnitValue>(); numeric and numeric->isCanonical())
                    continue;

                if (auto invert = child.is<InvertNode>(); invert and invert->child->is<UnitValue>() and invert->child->is<UnitValue>()->isCanonical())
                    continue;

                containsOnlyCanonicalNumerics = false;
                break;
            }

            auto type = determineType(product, mainType);
            Opt<UnitValue::Unit> resultUnit = NONE;
            if (type and type->isEmpty())
                resultUnit = Some(UnitValue::Unit::NONE);
            else if (auto baseType = type ? type->matchesAnyMathFunction() : NONE)
                resultUnit = Some(UnitValue::canonicalUnitOf(baseType));

            if (containsOnlyCanonicalNumerics and resultUnit) {
                f64 accumulator = 1.0;

                for (auto const& child : simplifiedChildren) {
                    if (auto numeric = child.is<UnitValue>()) {
                        accumulator *= numeric->value();
                    } else if (auto invert = child.is<InvertNode>()) {
                        accumulator /= invert->child->is<UnitValue>()->value();
                    }
                }

                return UnitValue{accumulator, *resultUnit};
            }

            // 5. Return root.
            return ProductNode{std::move(simplifiedChildren)};
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

} // namespace Vaev::Ng

namespace Vaev {

template <>
struct ValueParser<Ng::Calc> {
};

template <Opt<NumericBaseType> TYPE, AcceptsPercent WITH_PERCENT>
struct ValueParser<Ng::TypedCalc<TYPE, WITH_PERCENT>> {
    static Res<Ng::TypedCalc<TYPE, WITH_PERCENT>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto numeric = parseValue<TypedUnitValue<TYPE, WITH_PERCENT>>(c);
        if (numeric) {
            return Ok(Ng::TypedCalc<TYPE, WITH_PERCENT>{UNCHECKED, numeric.take().unitValue()});
        }

        if (c.peek() == Css::Sst::FUNC) {
            auto save = c;
            auto res = _parseTypedMathFunction(c);

            if (not res)
                c = save;
            return res;
        }

        return Error::invalidData();
    }

    // https://drafts.csswg.org/css-values-4/#calc-syntax
    static Res<Ng::TypedCalc<TYPE, WITH_PERCENT>> _parseTypedMathFunction(Cursor<Css::Sst>& c) {
        auto calc = try$(parseMathFunction(c));

        auto type = Ng::determineType(calc, TYPE);
        if (not type)
            return Error::invalidData("calc() type error");

        // NOTE: A bare percentage in a <*-percentage> context resolves to the base type.
        if (TYPE and type->contains(NumericBaseType::PERCENT) and WITH_PERCENT == AcceptsPercent::YES)
            type->applyPercentHint(*TYPE);

        if (not type->isBaseType(TYPE) or (type->containsPercentage() and WITH_PERCENT == AcceptsPercent::NO))
            return Error::invalidData("calc() type error");

        return Ok(Ng::TypedCalc<TYPE, WITH_PERCENT>{UNCHECKED, std::move(calc)});
    }

    struct ParamParser {
        Cursor<Css::Sst> _c;

        bool _isFirst = true;

        auto _maybeParseNext(auto f) -> Opt<decltype(f())> {
            eatWhitespace(_c);

            if (not _isFirst) {
                if (not _c.skip(Css::Token::COMMA))
                    return NONE;
                eatWhitespace(_c);
            } else {
                _isFirst = false;
            }

            return Some(f());
        }

        Opt<Res<Ng::Calc>> maybeParseNextSum() {
            return _maybeParseNext([&] {
                return parseCalcSum(_c);
            });
        }

        Res<Ng::Calc> parseNextSum() {
            auto next = maybeParseNextSum();
            if (not next)
                return Error::invalidData("missing comma in math function");
            return next.take();
        }

        Res<> endedOrErr() {
            eatWhitespace(_c);
            if (not _c.ended())
                return Error::invalidData("unexpected tokens");

            return Ok();
        }

        Res<Opt<Ng::Calc>> parseNextSumOrNone() {
            auto next = _maybeParseNext([&] -> Res<Opt<Ng::Calc>> {
                if (parseValue<Keywords::None>(_c))
                    return Ok(NONE);
                return Ok(Some(try$(parseCalcSum(_c))));
            });
            if (not next)
                return Error::invalidData("missing comma in math function");
            return next.take();
        }

        Res<Vec<Ng::Calc>> parseOneOrMoreSums() {
            auto calcs = Vec<Ng::Calc>{};

            while (true) {
                auto maybeNext = maybeParseNextSum();

                if (not maybeNext) {
                    break;
                }

                calcs.pushBack(try$(maybeNext.take()));
            }

            if (isEmpty(calcs))
                return Error::invalidData("expected one or more parameters in math function");

            return Ok(std::move(calcs));
        }
    };

    static Res<Ng::Calc> parseMathFunction(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto const parseFn = [](Str prefix, ParamParser& params) -> Res<Ng::Calc> {
            // <calc()>  = calc( <calc-sum> )
            if (prefix == "calc(") {
                return Ok(std::move(try$(params.parseNextSum())));
            }

            // <min()>   = min( <calc-sum># )
            if (prefix == "min(") {
                return Ok(Ng::FnNode::min(try$(params.parseOneOrMoreSums())));
            }

            // <max()>   = max( <calc-sum># )
            if (prefix == "max(") {
                return Ok(Ng::FnNode::max(try$(params.parseOneOrMoreSums())));
            }

            // <clamp()> = clamp( [ <calc-sum> | none ], <calc-sum>, [ <calc-sum> | none ] )
            if (prefix == "clamp(") {
                auto min = try$(params.parseNextSumOrNone());
                auto val = try$(params.parseNextSum());
                auto max = try$(params.parseNextSumOrNone());

                if (min and max)
                    return Ok(Ng::FnNode::clamp(min.take(), std::move(val), max.take()));

                // clamp(MIN, VAL, none) is equivalent to max(MIN, VAL)
                if (min and not max)
                    return Ok(Ng::FnNode::max({min.take(), std::move(val)}));

                // clamp(none, VAL, MAX) is equivalent to min(VAL, MAX)
                if (not min and max)
                    return Ok(Ng::FnNode::min({std::move(val), max.take()}));

                // clamp(none, VAL, none) is equivalent to just calc(VAL)
                return Ok(std::move(val));
            }

            // <sin()>   = sin( <calc-sum> )
            if (prefix == "sin(") {
                return Ok(Ng::FnNode::sin(try$(params.parseNextSum())));
            }

            // <cos()>   = cos( <calc-sum> )
            if (prefix == "cos(") {
                return Ok(Ng::FnNode::cos(try$(params.parseNextSum())));
            }

            // <tan()>   = tan( <calc-sum> )
            if (prefix == "tan(") {
                return Ok(Ng::FnNode::tan(try$(params.parseNextSum())));
            }

            // <asin()>  = asin( <calc-sum> )
            if (prefix == "asin(") {
                return Ok(Ng::FnNode::asin(try$(params.parseNextSum())));
            }

            // <acos()>  = acos( <calc-sum> )
            if (prefix == "acos(") {
                return Ok(Ng::FnNode::acos(try$(params.parseNextSum())));
            }

            // <atan()>  = atan( <calc-sum> )
            if (prefix == "atan(") {
                return Ok(Ng::FnNode::atan(try$(params.parseNextSum())));
            }

            // <atan2()> = atan2( <calc-sum>, <calc-sum> )
            if (prefix == "atan2(") {
                auto a = try$(params.parseNextSum());
                auto b = try$(params.parseNextSum());

                return Ok(Ng::FnNode::atan2(std::move(a), std::move(b)));
            }

            // <pow()>   = pow( <calc-sum>, <calc-sum> )
            if (prefix == "pow(") {
                auto a = try$(params.parseNextSum());
                auto b = try$(params.parseNextSum());

                return Ok(Ng::FnNode::pow(std::move(a), std::move(b)));
            }

            // <sqrt()>  = sqrt( <calc-sum> )
            if (prefix == "sqrt(") {
                return Ok(Ng::FnNode::sqrt(try$(params.parseNextSum())));
            }

            // <hypot()> = hypot( <calc-sum># )
            if (prefix == "hypot(") {
                return Ok(Ng::FnNode::hypot(try$(params.parseOneOrMoreSums())));
            }

            // <log()>   = log( <calc-sum>, <calc-sum>? )
            if (prefix == "log(") {
                auto a = try$(params.parseNextSum());
                if (auto b = params.maybeParseNextSum())
                    return Ok(Ng::FnNode::log(std::move(a), Some(try$(b.take()))));

                return Ok(Ng::FnNode::log(std::move(a), NONE));
            }

            return Error::invalidData("unknown math function");
        };

        if (c.peek() != Css::Sst::FUNC)
            return Error::invalidData("expected function");

        auto params = ParamParser{c.peek().content};
        auto fn = try$(parseFn(c.peek().prefix.expect()->token.data, params));
        try$(params.endedOrErr());
        c.next();
        return Ok(std::move(fn));
    }

    // <calc-sum> = <calc-product> [ [ '+' | '-' ] <calc-product> ]*
    static Res<Ng::Calc> parseCalcSum(Cursor<Css::Sst>& c) {
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
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        eatWhitespace(c);

        if (auto unitValue = parseValue<UnitValue>(c)) {
            return Ok(unitValue.take());
        }

        if (c.peek() == Css::Sst::BLOCK and c.peek().token == Css::Token::LEFT_PARENTHESIS) {
            Cursor<Css::Sst> content = c.peek().content;
            auto sum = try$(parseCalcSum(content));

            if (not content.ended())
                return Error::invalidData("unexpected tokens");

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
            // FIXME: NaN should be matched with exact casing.
            value = Math::NAN;
        } else {
            return Error::invalidData("unknown calc keyword");
        }

        return Ok(UnitValue{value, UnitValue::Unit::NONE});
    }
};

template <Opt<NumericBaseType> TYPE, AcceptsPercent WITH_PERCENT>
struct ValueSerializer<Ng::TypedCalc<TYPE, WITH_PERCENT>> {
    static Res<> serialize(Io::TextWriter& w, Ng::TypedCalc<TYPE, WITH_PERCENT> const& value) {
        return Ng::serializeMathFunction(w, value.calcTree());
    }
};

// FIXME: Make Pixel and Percentage backed by f32.
export struct LinearPixelPercentage {
    Pixel px;
    Percentage percentage;

    void repr(Io::Emit& e) const {
        e("({} + {})", px, percentage);
    }
};

export Number resolve(Ng::Number const& value) {
    auto simplified = Ng::simplify(value.calcTree(), NONE, NONE);

    if (auto n = simplified.is<UnitValue>(); n and n->unit() == UnitValue::Unit::NONE) {
        return n->value();
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export Percentage resolve(Ng::Percentage const& value) {
    auto simplified = Ng::simplify(
        value.calcTree(),
        NONE,
        Some(NumericBaseType::PERCENT),
        NONE
    );

    if (auto n = simplified.is<UnitValue>(); n and n->unit() == UnitValue::Unit::PERCENT) {
        return Percentage{n->value()};
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export Number resolve(Ng::Percentage const& value, f64 percentBasis) {
    auto simplified = Ng::simplify(
        value.calcTree(),
        NONE,
        NONE,
        Some(percentBasis)
    );

    if (auto n = simplified.is<UnitValue>(); n and n->unit() == UnitValue::Unit::NONE) {
        return n->value();
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export Pixel resolve(Ng::Length const& value, LengthContext auto const& ctx) {
    auto simplified = Ng::simplify(
        value.calcTree(),
        ctx,
        Some(NumericBaseType::LENGTH),
        NONE
    );

    logInfo("{}", simplified);

    if (auto n = simplified.template is<UnitValue>(); n and n->unit() == UnitValue::Unit::PX) {
        return Pixel{n->value()};
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export using PixelPercentage = Union<Pixel, Percentage, LinearPixelPercentage, Box<Ng::Calc>>;

// https://drafts.csswg.org/css-values-4/#calc-computed-value
export PixelPercentage resolve(Ng::LengthPercentage const& value, LengthContext auto const& ctx) {
    auto simplified = Ng::simplify(
        value.calcTree(),
        ctx,
        Some(NumericBaseType::LENGTH),
        NONE
    );

    if (auto [numeric] = simplified.template is<UnitValue>()) {
        if (numeric.unit() == UnitValue::Unit::PX) {
            return Pixel{numeric.value()};
        }

        if (numeric.unit() == UnitValue::Unit::PERCENT) {
            return Percentage{numeric.value()};
        }
    }

    // NOTE: Children are sorted by simplification, so the percentage comes first.
    if (auto sum = simplified.template is<Ng::SumNode>(); sum and sum->children.len() == 2) {
        auto maybePercent = sum->children[0].template is<UnitValue>();
        auto maybePx = sum->children[1].template is<UnitValue>();
        if (maybePercent and maybePx and maybePercent->unit() == UnitValue::Unit::PERCENT and maybePx->unit() == UnitValue::Unit::PX) {
            return LinearPixelPercentage{Pixel{maybePx->value()}, Percentage{maybePercent->value()}};
        }
    }

    return makeBox<Ng::Calc>(std::move(simplified));
}

export Ng::LengthPercentage toSpecified(PixelPercentage const& value) {
    return value.visit(
        [](Pixel const& px) -> Ng::LengthPercentage {
            return Ng::LengthPercentage{UNCHECKED, UnitValue{px.value(), UnitValue::Unit::PX}};
        },
        [](Percentage const& percentage) -> Ng::LengthPercentage {
            return Ng::LengthPercentage{UNCHECKED, UnitValue{percentage.value(), UnitValue::Unit::PERCENT}};
        },
        [](LinearPixelPercentage const& lin) -> Ng::LengthPercentage {
            return Ng::LengthPercentage{
                UNCHECKED,
                Ng::SumNode{
                    .children = {
                        UnitValue{lin.percentage.value(), UnitValue::Unit::PERCENT},
                        UnitValue{lin.px.value(), UnitValue::Unit::PX},
                    },
                }
            };
        },
        [](Box<Ng::Calc> const& calc) -> Ng::LengthPercentage {
            return Ng::LengthPercentage{UNCHECKED, *calc};
        }
    );
}

// https://www.w3.org/TR/css-values-4/#calc-computed-value
export Au resolve(PixelPercentage const& value, Au relative) {
    return value.visit(
        [](Pixel const& px) {
            return Au::fromFloatNearest(px.value());
        },
        [&](Percentage const& percentage) {
            return Au::fromFloatNearest(relative.cast<f32>() * (percentage.value() / 100.));
        },
        [&](LinearPixelPercentage const& lin) {
            return Au::fromFloatNearest(lin.px.value() + relative.cast<f32>() * (lin.percentage.value() / 100.));
        },
        [&](Box<Ng::Calc> const& calc) {
            auto simplified = Ng::simplify(
                *calc,
                NONE,
                Some(NumericBaseType::LENGTH),
                Some(relative.cast<f64>())
            );

            if (auto n = simplified.is<UnitValue>(); n and n->unit() == UnitValue::Unit::PX) {
                return Au{n->value()};
            } else {
                panic("calc didn't simplify to a single value of expected unit");
            }
        }
    );
}

export Opt<Au> resolve(PixelPercentage const& value, Opt<Au> relative) {
    return value.visit(
        [](Pixel const& px) {
            return Some(Au::fromFloatNearest(px.value()));
        },
        [&](Percentage const& percentage) -> Opt<Au> {
            if (auto const [r] = relative)
                return Some(Au::fromFloatNearest(r.cast<f32>() * (percentage.value() / 100.)));

            return NONE;
        },
        [&](LinearPixelPercentage const& lin) -> Opt<Au> {
            if (auto const [r] = relative)
                return Some(Au::fromFloatNearest(lin.px.value() + r.cast<f32>() * (lin.percentage.value() / 100.)));

            return NONE;
        },
        [&](Box<Ng::Calc> const& calc) -> Opt<Au> {
            auto simplified = Ng::simplify(
                *calc,
                NONE,
                Some(NumericBaseType::LENGTH),
                relative.map([](Au au) {
                    return au.cast<f64>();
                })
            );

            if (auto n = simplified.is<UnitValue>(); n and n->unit() == UnitValue::Unit::PX) {
                return Some(Au{n->value()});
            } else {
                return NONE;
            }
        }
    );
}

} // namespace Vaev
