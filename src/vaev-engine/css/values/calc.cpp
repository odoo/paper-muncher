module;

#include <karm/macros>

export module Vaev.Engine:css.values.calc;

import Karm.Core;

import :css.lexer;
import :css.parser;
import :css.serializer;
import :css.values.base;
import :css.values.keywords;
import :css.values.primitives;
import :css.values.unitValue;

using namespace Karm;

namespace Vaev::Css {

// MARK: Calculation Tree ------------------------------------------------------

struct NumericType {
    Array<i32, toUnderlyingType(NumericBaseType::_LEN)> _entries{};

    Opt<NumericBaseType> percentHint = NONE;

    NumericType() = default;

    static NumericType simple(NumericBaseType baseType, Opt<NumericBaseType> percentHint = NONE) {
        auto type = NumericType{};
        type._entries[toUnderlyingType(baseType)] = 1;
        type.percentHint = percentHint;
        return type;
    }

    // https://infra.spec.whatwg.org/#map-exists
    bool contains(NumericBaseType baseType) const {
        // An ordered map contains an entry with a given key if there exists an entry whose key equals that key.
        // We can also denote this by saying that, for an ordered map map and key key, "map[key] exists".
        return _entries[toUnderlyingType(baseType)] != 0;
    }

    bool isBaseType(Opt<NumericBaseType> baseType) const {
        if (not baseType)
            return isEmpty();

        for (usize i = 0; i < toUnderlyingType(NumericBaseType::_LEN); i++) {
            if (i == static_cast<usize>(toUnderlyingType(*baseType))) {
                if (_entries[i] != 1)
                    return false;
            } else {
                if (_entries[i] != 0)
                    return false;
            }
        }

        return true;
    }

    bool isEmpty() const {
        for (usize i = 0; i < toUnderlyingType(NumericBaseType::_LEN); i++) {
            if (_entries[i] != 0) {
                return false;
            }
        }

        return true;
    }

    // https://drafts.css-houdini.org/css-typed-om-1/#apply-the-percent-hint
    void applyPercentHint(this NumericType& self, NumericBaseType hint) {
        // To apply the percent hint hint to a type without a percent hint, perform the following steps:

        // 1. Set type’s percent hint to hint.
        self.percentHint = Some(hint);

        // 2. If type doesn’t contain hint, set type[hint] to 0.
        if (not self.contains(hint))
            self[hint] = 0;

        // 3. If hint is anything other than "percent", and type contains "percent",
        if (hint != NumericBaseType::PERCENT and self.contains(NumericBaseType::PERCENT)) {
            // add type["percent"] to type[hint],
            self[hint] += self[NumericBaseType::PERCENT];
            // then set type["percent"] to 0.
            self[NumericBaseType::PERCENT] = 0;
        }

        // 4. Return type.
    }

    // https://drafts.csswg.org/css-values-4/#css-make-a-type-consistent
    Opt<NumericType> madeConsistentWith(NumericType const& input) {
        auto base = *this;
        // To make a type base consistent with another type input:

        // 1. If both base and input have different non-null percent hints, they can’t be made consistent.
        if (base.percentHint and input.percentHint and *base.percentHint != *input.percentHint) {
            // Return failure.
            return NONE;
        }

        // 2. If base has a null percent hint set base’s percent hint to input’s percent hint.
        if (not base.percentHint)
            base.percentHint = input.percentHint;

        // 3. Return base.
        return Some(base);
    }

    // https://drafts.css-houdini.org/css-typed-om-1/#cssnumericvalue-add-two-types
    Opt<NumericType> add(NumericType const& other) const {
        // To add two types type1 and type2, perform the following steps:

        // 1. Replace type1 with a fresh copy of type1, and type2 with a fresh copy of type2.
        //    Let finalType be a new type with an initially empty ordered map and an initially null percent hint.
        auto type1 = *this;
        auto type2 = other;

        // 2. -> If both type1 and type2 have non-null percent hints with different values
        if (type1.percentHint and type2.percentHint and *type1.percentHint != *type2.percentHint) {
            //The types can’t be added. Return failure.
            return NONE;
        }
        //    -> If type1 has a non-null percent hint 'hint' and type2 doesn’t
        else if (type1.percentHint and not type2.percentHint) {
            // Apply the percent hint 'hint' to type2.
            type2.applyPercentHint(*type1.percentHint);
        }
        //    -> Vice versa if type2 has a non-null percent hint and type1 doesn’t.
        else if (type2.percentHint and not type1.percentHint) {
            type1.applyPercentHint(*type2.percentHint);
        }
        //    -> Otherwise
        //       Continue to the next step.

        // 3. -> If all the entries of type1 with non-zero values are contained in type2
        //       with the same value, and vice-versa.
        if (type1._entries == type2._entries) {
            // Copy all of type1’s entries to finalType, and then copy all of type2’s
            // entries to finalType that finalType doesn’t already contain.
            // Set finalType’s percent hint to type1’s percent hint. Return finalType.
            auto finalType = type1;
            return Some(finalType);
        }

        // -> If type1 and/or type2 contain "percent" with a non-zero value,
        //    and type1 and/or type2 contain a key other than "percent" with a non-zero value
        if (type1.contains(NumericBaseType::PERCENT) or type2.contains(NumericBaseType::PERCENT)) {
            // For each base type other than "percent" hint:
            for (usize i = 0; i < toUnderlyingType(NumericBaseType::_LEN); i++) {
                auto hint = static_cast<NumericBaseType>(i);
                if (hint == NumericBaseType::PERCENT)
                    continue;

                // 1. Provisionally apply the percent hint hint to both type1 and type2.
                auto t1 = type1;
                auto t2 = type2;
                t1.applyPercentHint(hint);
                t2.applyPercentHint(hint);

                // 2. If, afterwards, all the entries of type1 with non-zero values are contained
                //    in type2 with the same value, and vice versa, then copy all of type1’s entries to finalType,
                //    and then copy all of type2’s entries to finalType that finalType doesn’t already contain.
                //    Set finalType’s percent hint to hint. Return finalType.
                if (t1._entries == t2._entries) {
                    t1.percentHint = Some(hint);
                    return Some(t1);
                }

                // 3. Otherwise, revert type1 and type2 to their state at the start of this loop.
            }

            // If the loop finishes without returning finalType, then the types can’t be added. Return failure.
            return NONE;
        }

        // -> Otherwise
        //    The types can’t be added. Return failure.
        return NONE;
    }

    // https://drafts.css-houdini.org/css-typed-om-1/#cssnumericvalue-multiply-two-types
    Opt<NumericType> multiply(NumericType const& other) {
        // To multiply two types type1 and type2, perform the following steps:

        // 1. Replace type1 with a fresh copy of type1, and type2 with a fresh copy of type2.
        //    Let finalType be a new type with an initially empty ordered map and an initially null percent hint.
        auto type1 = *this;
        auto type2 = other;

        // 2. If both type1 and type2 have non-null percent hints with different values, the types can’t be multiplied.
        //    Return failure.
        if (percentHint and other.percentHint and *percentHint != *other.percentHint) {
            return NONE;
        }

        // 3. If type1 has a non-null percent hint 'hint' and type2 doesn’t, apply the percent hint hint to type2.
        if (type1.percentHint and not type2.percentHint) {
            type2.applyPercentHint(*type1.percentHint);
        }
        //    Vice versa if type2 has a non-null percent hint and type1 doesn’t.
        else if (type2.percentHint and not type1.percentHint) {
            type1.applyPercentHint(*type2.percentHint);
        }

        // 4. Copy all of type1’s entries to finalType,then for each baseType → power of type2:
        auto finalType = type1;
        for (usize i = 0; i < _entries.len(); i++) {
            // 1. If finalType[baseType] exists, increment its value by power.
            // 2. Otherwise, set finalType[baseType] to power.
            finalType._entries[i] += type2._entries[i];
        }
        //    Set finalType’s percent hint to type1’s percent hint.
        finalType.percentHint = type1.percentHint;

        // Return finalType.
        return Some(finalType);
    }

    // https://drafts.css-houdini.org/css-typed-om-1/#cssnumericvalue-invert-a-type
    NumericType invert() const {
        // 1. Let result be a new type with an initially empty ordered map and a percent hint matching that of type.
        auto result = *this;

        // 2. For each unit → exponent of type, set result[unit] to (-1 * exponent).
        for (usize i = 0; i < _entries.len(); i++) {
            result._entries[i] *= -1;
        }

        // 3. Return result.
        return result;
    }

    // https://drafts.csswg.org/css-values-4/#math-function
    // https://drafts.css-houdini.org/css-typed-om-1/#cssnumericvalue-match
    Opt<NumericBaseType> matchesAnyMathFunction() const {
        isize exponentLocation = -1;
        for (usize i = 0; i < toUnderlyingType(NumericBaseType::_LEN); i++) {
            if (_entries[i] == 0)
                continue;

            if (_entries[i] == 1) {
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

    // // https://drafts.css-houdini.org/css-typed-om-1/#cssnumericvalue-match
    bool matches(Opt<NumericBaseType> baseType, bool allowsPercentage) const {
        if (baseType == NumericBaseType::PERCENT)
            return isBaseType(baseType);

        if (not allowsPercentage and percentHint)
            return false;

        if (allowsPercentage and percentHint and percentHint != baseType)
            return false;

        return isBaseType(baseType);
    }

    bool operator==(NumericType const&) const = default;

    always_inline constexpr i32 const& operator[](NumericBaseType baseType) const {
        return _entries[toUnderlyingType(baseType)];
    }

    always_inline constexpr i32& operator[](NumericBaseType baseType) {
        return _entries[toUnderlyingType(baseType)];
    }

    void repr(Io::Emit& e) const {
        e("(numeric-type {} hint={})", _entries, percentHint);
    }
};

export struct NegateNode;
export struct InvertNode;
export struct SumNode;
export struct ProductNode;
export struct FnNode;

export using Calc = Union<
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
        ROUND_NEAREST,
        ROUND_UP,
        ROUND_DOWN,
        ROUND_TO_ZERO,
        MOD,
        REM,
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
        ABS,
        SIGN,
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

    // https://drafts.csswg.org/css-values-4/#valdef-rounding-strategy-nearest
    static FnNode roundNearest(Calc a, Opt<Calc> b) {
        if (b)
            return {UNCHECKED, Name::ROUND_NEAREST, {std::move(a), b.take()}};

        return {UNCHECKED, Name::ROUND_NEAREST, {std::move(a)}};
    }

    // https://drafts.csswg.org/css-values-4/#valdef-rounding-strategy-up
    static FnNode roundUp(Calc a, Opt<Calc> b) {
        if (b)
            return {UNCHECKED, Name::ROUND_UP, {std::move(a), b.take()}};

        return {UNCHECKED, Name::ROUND_UP, {std::move(a)}};
    }

    // https://drafts.csswg.org/css-values-4/#valdef-rounding-strategy-down
    static FnNode roundDown(Calc a, Opt<Calc> b) {
        if (b)
            return {UNCHECKED, Name::ROUND_DOWN, {std::move(a), b.take()}};

        return {UNCHECKED, Name::ROUND_DOWN, {std::move(a)}};
    }

    // https://drafts.csswg.org/css-values-4/#valdef-rounding-strategy-to-zero
    static FnNode roundToZero(Calc a, Opt<Calc> b) {
        if (b)
            return {UNCHECKED, Name::ROUND_TO_ZERO, {std::move(a), b.take()}};

        return {UNCHECKED, Name::ROUND_TO_ZERO, {std::move(a)}};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-mod
    static FnNode mod(Calc a, Calc b) {
        return {UNCHECKED, Name::MOD, {std::move(a), std::move(b)}};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-rem
    static FnNode rem(Calc a, Calc b) {
        return {UNCHECKED, Name::REM, {std::move(a), std::move(b)}};
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

    // https://drafts.csswg.org/css-values-4/#funcdef-abs
    static FnNode abs(Calc a) {
        return {UNCHECKED, Name::ABS, {std::move(a)}};
    }

    // https://drafts.csswg.org/css-values-4/#funcdef-sign
    static FnNode sign(Calc a) {
        return {UNCHECKED, Name::SIGN, {std::move(a)}};
    }

    Opt<UnitValue::Unit> canonicalUnit() const {
        switch (name) {
        case Name::MIN:
        case Name::MAX:
        case Name::CLAMP:
        case Name::ROUND_NEAREST:
        case Name::ROUND_UP:
        case Name::ROUND_DOWN:
        case Name::ROUND_TO_ZERO:
        case Name::MOD:
        case Name::REM:
        case Name::HYPOT:
        case Name::ABS:
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
        case Name::SIGN:
            return Some(UnitValue::Unit::NONE);
        case Name::_LEN:
            unreachable();
        }
    }

    void repr(Io::Emit& e) const {
        e("({} {})", name, children);
    }
};

// https://drafts.csswg.org/css-values-4/#calc-type-checking
Opt<NumericType> determineType(Calc const& calc, Opt<NumericBaseType> mainType) {
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
                if (baseType == NumericBaseType::PERCENT) {
                    if (mainType)
                        return Some(NumericType::simple(*mainType, mainType));
                    return Some(NumericType::simple(NumericBaseType::PERCENT, Some(NumericBaseType::PERCENT)));
                }

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
            case FnNode::Name::ABS: {
                return determineType(f.children[0], mainType);
            }
            // min(), max(), clamp(), hypot(), round(), mod(), rem()
            // The result of adding the types of its comma-separated calculations.
            case FnNode::Name::ROUND_NEAREST:
            case FnNode::Name::ROUND_UP:
            case FnNode::Name::ROUND_DOWN:
            case FnNode::Name::ROUND_TO_ZERO: {
                // https://drafts.csswg.org/css-values-4/#round-func
                // If the type of A matches <number>, then B may be omitted, and defaults to 1.
                // In all other cases, omitting B is invalid.
                if (f.children.len() == 1) {
                    auto type = try$(determineType(f.children[0], mainType));
                    if (not type.isEmpty())
                        return NONE;
                    return Some(type);
                }

                return addTypes(f.children);
            }
            case FnNode::Name::MOD:
            case FnNode::Name::REM:
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
                auto childType = try$(addTypes(f.children));
                return NumericType::simple(NumericBaseType::ANGLE).madeConsistentWith(childType);
            }
            // sign(), sin(), cos(), tan(), pow(), sqrt(), log(), exp()
            // «[ ]» (empty map).
            case FnNode::Name::SIGN: {
                // https://drafts.csswg.org/css-values-4/#sign-funcs
                // The return type is a <number>, made consistent with the input calculation’s type.
                auto childType = try$(determineType(f.children[0], mainType));
                return NumericType{}.madeConsistentWith(childType);
            }
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

// MARK: Parsing ---------------------------------------------------------------
// https://drafts.csswg.org/css-values-4/#calc-syntax
// NOTE: Vaev doesn't simplify parsed values, see Style::simplify().

Res<Calc> parseCalcSum(Cursor<Sst>& c);
Res<Calc> parseCalcProduct(Cursor<Sst>& c);
Res<Calc> parseCalcValue(Cursor<Sst>& c);
Res<UnitValue> parseCalcKeyword(Cursor<Sst>& c);

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

Res<Calc> parseMathFunction(Cursor<Sst>& c) {
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

bool _isWhitespace(Cursor<Sst> const& c) {
    return not c.ended() and c.peek() == Token::WHITESPACE;
}

bool _isOperator(Cursor<Sst> const& c, auto... ops) {
    return not c.ended() and c.peek() == Sst::TOKEN and oneOf(c.peek().token.data, ops...);
}

// <calc-sum> = <calc-product> [ [ '+' | '-' ] <calc-product> ]*
// https://drafts.csswg.org/css-values-4/#calc-syntax
// In addition, whitespace is required on both sides of the + and - operators.
// (The * and / operators can be used without white space around them.)
Res<Calc> parseCalcSum(Cursor<Sst>& c) {
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
Res<Calc> parseCalcProduct(Cursor<Sst>& c) {
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
Res<Calc> parseCalcValue(Cursor<Sst>& c) {
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
Res<UnitValue> parseCalcKeyword(Cursor<Sst>& c) {
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

// MARK: Serialization ---------------------------------------------------------

Res<> serializeCalculationTree(Io::TextWriter& w, Calc const& root, bool stripParens = false);

// https://drafts.csswg.org/css-values-4/#serialize-a-math-function
Res<> serializeMathFunction(Io::TextWriter& w, Calc const& root) {
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

} // namespace Vaev::Css
