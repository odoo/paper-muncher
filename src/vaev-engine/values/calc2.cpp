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

namespace Vaev::Css {

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

    // https://drafts.csswg.org/css-values-4/#css-contain-a-percentage
    bool containsPercentage() const {
        // A value contains a percentage if its type is «[ "percent" → 1 ]», or its type’s percent hint is non-null.
        return contains(NumericBaseType::PERCENT) or percentHint;
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
    // FIXME: Doesn't follow CSS math semantics.
    case ROUND_NEAREST: {
        auto a = v(0);
        auto b = count > 1 ? v(1) : 1.0;
        return Math::round(a / b) * b;
    }
    // FIXME: Doesn't follow CSS math semantics.
    case ROUND_UP: {
        auto a = v(0);
        auto b = count > 1 ? v(1) : 1.0;
        return Math::ceil(a / b) * b;
    }
    // FIXME: Doesn't follow CSS math semantics.
    case ROUND_DOWN: {
        auto a = v(0);
        auto b = count > 1 ? v(1) : 1.0;
        return Math::floor(a / b) * b;
    }
    // FIXME: Doesn't follow CSS math semantics.
    case ROUND_TO_ZERO: {
        auto a = v(0);
        auto b = count > 1 ? v(1) : 1.0;
        return Math::trunc(a / b) * b;
    }
    // FIXME: Doesn't follow CSS math semantics.
    case MOD:
    case REM:
        return Math::fmod(v(0), v(1));
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
    case ABS:
        return Math::abs(v(0));
    case SIGN: {
        auto val = v(0);

        if (val > 0)
            return 1;

        if (val < 0)
            return -1;

        if (val == 0)
            return val;

        return Math::NAN;
    }
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

            auto type = determineType(product, NONE);
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

} // namespace Vaev::Css

namespace Vaev::Css {

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

namespace Vaev::Style {

// FIXME: Make Pixel and Percentage backed by f32.
export struct LinearPixelPercentage {
    Pixel px;
    Percent percentage;

    void repr(Io::Emit& e) const {
        e("({} + {})", px, percentage);
    }
};

export f64 toComputed(Css::Number const& value) {
    auto simplified = simplify(value.calcTree(), NONE, NONE);

    if (auto n = simplified.is<Css::UnitValue>(); n and n->unit() == Css::UnitValue::Unit::NONE) {
        return n->value();
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export Css::Rad toComputed(Css::Angle const& value) {
    auto simplified = simplify(
        value.calcTree(),
        NONE,
        Some(Css::NumericBaseType::ANGLE),
        NONE
    );

    if (auto n = simplified.is<Css::UnitValue>(); n and n->baseType() == Css::NumericBaseType::ANGLE) {
        return Css::Rad{n->to(Css::UnitValue::Unit::RAD)};
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export Css::Angle toSpecified(Css::Rad const& angle) {
    return Css::Angle{UNCHECKED, Css::UnitValue{angle.value(), Css::UnitValue::Unit::RAD}};
}

export Percent toComputed(Css::Percentage const& value) {
    auto simplified = simplify(
        value.calcTree(),
        NONE,
        Some(Css::NumericBaseType::PERCENT),
        NONE
    );

    if (auto n = simplified.is<Css::UnitValue>(); n and n->unit() == Css::UnitValue::Unit::PERCENT) {
        return Percent{n->value()};
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export f64 toComputed(Css::Percentage const& value, f64 percentBasis) {
    auto simplified = simplify(
        value.calcTree(),
        NONE,
        NONE,
        Some(percentBasis)
    );

    if (auto n = simplified.is<Css::UnitValue>(); n and n->unit() == Css::UnitValue::Unit::NONE) {
        return n->value();
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export Pixel toComputed(Css::Length const& value, Css::LengthContext auto const& ctx) {
    auto simplified = simplify(
        value.calcTree(),
        ctx,
        Some(Css::NumericBaseType::LENGTH),
        NONE
    );

    if (auto n = simplified.template is<Css::UnitValue>(); n and n->unit() == Css::UnitValue::Unit::PX) {
        return Pixel{n->value()};
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export Css::Length toSpecified(Pixel const& px) {
    return Css::Length{UNCHECKED, Css::UnitValue{px.value(), Css::UnitValue::Unit::PX}};
}

export Pixel toComputed(Css::LengthPercentage const& value, Css::LengthContext auto const& ctx, f64 percentBasis) {
    auto simplified = simplify(
        value.calcTree(),
        ctx,
        Some(Css::NumericBaseType::LENGTH),
        Some(percentBasis)
    );

    if (auto n = simplified.template is<Css::UnitValue>(); n and n->unit() == Css::UnitValue::Unit::PX) {
        return Pixel{n->value()};
    } else {
        panic("calc didn't simplify to a single value of expected unit");
    }
}

export using PixelPercentage = Union<Pixel, Percent, LinearPixelPercentage, Box<Css::Calc>>;

// https://drafts.csswg.org/css-values-4/#calc-computed-value
export PixelPercentage toComputed(Css::LengthPercentage const& value, Css::LengthContext auto const& ctx) {
    auto simplified = simplify(
        value.calcTree(),
        ctx,
        Some(Css::NumericBaseType::LENGTH),
        NONE
    );

    if (auto [numeric] = simplified.template is<Css::UnitValue>()) {
        if (numeric.unit() == Css::UnitValue::Unit::PX) {
            return Pixel{numeric.value()};
        }

        if (numeric.unit() == Css::UnitValue::Unit::PERCENT) {
            return Percent{numeric.value()};
        }
    }

    // NOTE: Children are sorted by simplification, so the percentage comes first.
    if (auto sum = simplified.template is<Css::SumNode>(); sum and sum->children.len() == 2) {
        auto maybePercent = sum->children[0].template is<Css::UnitValue>();
        auto maybePx = sum->children[1].template is<Css::UnitValue>();
        if (maybePercent and maybePx and maybePercent->unit() == Css::UnitValue::Unit::PERCENT and maybePx->unit() == Css::UnitValue::Unit::PX) {
            return LinearPixelPercentage{Pixel{maybePx->value()}, Percent{maybePercent->value()}};
        }
    }

    return makeBox<Css::Calc>(std::move(simplified));
}

export Css::LengthPercentage toSpecified(PixelPercentage const& value) {
    return value.visit(
        [](Pixel const& px) -> Css::LengthPercentage {
            return Css::LengthPercentage{UNCHECKED, Css::UnitValue{px.value(), Css::UnitValue::Unit::PX}};
        },
        [](Percent const& percentage) -> Css::LengthPercentage {
            return Css::LengthPercentage{UNCHECKED, Css::UnitValue{percentage.value(), Css::UnitValue::Unit::PERCENT}};
        },
        [](LinearPixelPercentage const& lin) -> Css::LengthPercentage {
            return Css::LengthPercentage{
                UNCHECKED,
                Css::SumNode{
                    .children = {
                        Css::UnitValue{lin.percentage.value(), Css::UnitValue::Unit::PERCENT},
                        Css::UnitValue{lin.px.value(), Css::UnitValue::Unit::PX},
                    },
                }
            };
        },
        [](Box<Css::Calc> const& calc) -> Css::LengthPercentage {
            return Css::LengthPercentage{UNCHECKED, *calc};
        }
    );
}

// https://www.w3.org/TR/css-values-4/#calc-computed-value
export Au resolve(PixelPercentage const& value, Au relative) {
    return value.visit(
        [](Pixel const& px) {
            return Au::fromFloatNearest(px.value());
        },
        [&](Percent const& percentage) {
            return Au::fromFloatNearest(relative.cast<f32>() * (percentage.value() / 100.));
        },
        [&](LinearPixelPercentage const& lin) {
            return Au::fromFloatNearest(lin.px.value() + relative.cast<f32>() * (lin.percentage.value() / 100.));
        },
        [&](Box<Css::Calc> const& calc) {
            auto simplified = simplify(
                *calc,
                NONE,
                Some(Css::NumericBaseType::LENGTH),
                Some(relative.cast<f64>())
            );

            if (auto n = simplified.is<Css::UnitValue>(); n and n->unit() == Css::UnitValue::Unit::PX) {
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
        [&](Percent const& percentage) -> Opt<Au> {
            if (auto const [r] = relative)
                return Some(Au::fromFloatNearest(r.cast<f32>() * (percentage.value() / 100.)));

            return NONE;
        },
        [&](LinearPixelPercentage const& lin) -> Opt<Au> {
            if (auto const [r] = relative)
                return Some(Au::fromFloatNearest(lin.px.value() + r.cast<f32>() * (lin.percentage.value() / 100.)));

            return NONE;
        },
        [&](Box<Css::Calc> const& calc) -> Opt<Au> {
            auto simplified = simplify(
                *calc,
                NONE,
                Some(Css::NumericBaseType::LENGTH),
                relative.map([](Au au) {
                    return au.cast<f64>();
                })
            );

            if (auto n = simplified.is<Css::UnitValue>(); n and n->unit() == Css::UnitValue::Unit::PX) {
                return Some(Au{n->value()});
            } else {
                return NONE;
            }
        }
    );
}

} // namespace Vaev::Style

