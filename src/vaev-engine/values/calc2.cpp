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

    bool isBaseType(NumericBaseType baseType) const {
        for (usize i = 0; i < toUnderlyingType(NumericBaseType::_LEN); i++) {
            if (i == static_cast<usize>(toUnderlyingType(baseType))) {
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

        return Some(static_cast<NumericBaseType>(exponentLocation));
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

// https://drafts.csswg.org/css-values-4/#sort-a-calculations-children
static void sortCalculationChildren(MutSlice<Calc> children) {
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

static Res<> serializeCalculationTree(Io::TextWriter& w, Calc const& root, bool stripParens = false);

// https://drafts.csswg.org/css-values-4/#serialize-a-math-function
static Res<> serializeMathFunction(Io::TextWriter& w, Calc const& root) {
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
        [&](MinNode const& min) -> Res<> {
            try$(w.writeStr("min("s));

            bool first = true;
            for (auto const& child : min.children) {
                if (first) {
                    first = false;
                } else {
                    try$(w.writeStr(", "s));
                }

                try$(serializeCalculationTree(w, child, true));
            }

            return Ok();
        },
        [&](MaxNode const& max) -> Res<> {
            try$(w.writeStr("max("s));

            bool first = true;
            for (auto const& child : max.children) {
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
static Calc simplify(Calc const& calc) {
    auto simplifyChild = [&](Calc const& child) -> Calc {
        return simplify(child);
    };

    auto simplifyChildren = [&](Slice<Calc> children) -> Vec<Calc> {
        auto simplified = Vec<Calc>{children.len()};

        for (auto const& child : children) {
            simplified.emplaceBack(simplify(child));
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
        [](UnitValue const& numeric) -> Calc {
            // FIXME
            return numeric.toComputed();
        },
        // 5. If root is a Min or Max node, attempt to partially simplify it:
        [&](MinNode const& min) -> Calc {
            auto simplifiedChildren = simplifyChildren(min.children);

            // 1. For each node child of root’s children:
            //   If child is a numeric value with enough information to compare magnitudes
            //   with another child of the same unit (see note in previous step),
            //   and there are other children of root that are numeric values with the same unit,
            //   combine all such children with the appropriate operator per root, and replace child with the result,
            //   removing all other child nodes involved.
            // NOSPEC: Chrome doesn't fold out of order childrens, so we don't do it too to stay O(n).
            foldNumericSiblings(
                simplifiedChildren,
                [](UnitValue::Unit unit) {
                    return unit != UnitValue::Unit::PERCENT;
                },
                [](f64 a, f64 b) -> f64 {
                    return Karm::min(a, b);
                }
            );

            // 2. If root has only one child,
            if (simplifiedChildren.len() == 1) {
                // return the child.
                return std::move(simplifiedChildren[0]);
            }

            // Otherwise, return root.
            return MinNode{std::move(simplifiedChildren)};
        },
        [&](MaxNode const& max) -> Calc {
            auto simplifiedChildren = simplifyChildren(max.children);

            // 1. For each node child of root’s children:
            //   If child is a numeric value with enough information to compare magnitudes
            //   with another child of the same unit (see note in previous step),
            //   and there are other children of root that are numeric values with the same unit,
            //   combine all such children with the appropriate operator per root, and replace child with the result,
            //   removing all other child nodes involved.
            // NOSPEC: Chrome doesn't fold out of order childrens, so we don't do it too to stay O(n).
            foldNumericSiblings(
                simplifiedChildren,
                [](UnitValue::Unit unit) {
                    return unit != UnitValue::Unit::PERCENT;
                },
                [](f64 a, f64 b) -> f64 {
                    return Karm::max(a, b);
                }
            );

            // 2. If root has only one child,
            if (simplifiedChildren.len() == 1) {
                // return the child.
                return std::move(simplifiedChildren[0]);
            }

            // Otherwise, return root.
            return MaxNode{std::move(simplifiedChildren)};
        },
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
            if (auto child = simplifiedChild.is<UnitValue>(); child and not child->baseType()) {
                // return the reciprocal of the child’s value.
                return UnitValue{1.0 / child->value(), UnitValue::Unit::NONE};
            }

            // 2. If root’s child is an Invert node,
            if (auto [child] = simplifiedChild.is<InvertNode>()) {
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
                if (auto [child] = simplifiedChild.is<SumNode>()) {
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
                if (auto [child] = simplifiedChild.is<ProductNode>()) {
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

                auto fstAsUnit = simplifiedChildren[0].is<UnitValue>();
                auto sndAsSum = simplifiedChildren[1].is<SumNode>();

                if (fstAsUnit and isNumber(*fstAsUnit) and sndAsSum and onlyNumericChildren(*sndAsSum)) {
                    for (auto& child : sndAsSum->children) {
                        auto numeric = *child.is<UnitValue>();
                        numeric = UnitValue{numeric.value() * fstAsUnit->value(), numeric.unit()};
                    }

                    return std::move(*sndAsSum);
                }

                auto fstAsSum = simplifiedChildren[0].is<SumNode>();
                auto sndAsUnit = simplifiedChildren[1].is<UnitValue>();

                if (sndAsUnit and isNumber(*sndAsUnit) and fstAsSum and onlyNumericChildren(*fstAsSum)) {
                    for (auto& child : fstAsSum->children) {
                        auto numeric = *child.is<UnitValue>();
                        numeric = UnitValue{numeric.value() * sndAsUnit->value(), numeric.unit()};
                    }

                    return std::move(*fstAsSum);
                }
            }

            // 4. If root contains only numeric values and/or Invert nodes containing numeric values, and multiplying the types of all the children
            //    (noting that the type of an Invert node is the inverse of its child’s type) results in a type that matches any of the types that a math
            //    function can resolve to, return the result of multiplying all the values of the children (noting that the value of an Invert node is the
            //    reciprocal of its child’s value), expressed in the result’s canonical unit.
            bool containsOnlyCanonicalNumerics = true;
            for (auto const& child : product.children) {
                if (auto numeric = child.is<UnitValue>(); numeric and numeric->isCanonical())
                    continue;

                if (auto invert = child.is<InvertNode>(); invert and invert->child->is<UnitValue>())
                    continue;

                containsOnlyCanonicalNumerics = false;
                break;
            }

            auto resultBaseType = determineType(product)->matchesAnyMathFunction();

            if (containsOnlyCanonicalNumerics and resultBaseType) {
                f64 accumulator = 1.0;

                for (auto const& child : product.children) {
                    if (auto numeric = child.is<UnitValue>()) {
                        accumulator *= numeric->value();
                    } else if (auto invert = child.is<InvertNode>()) {
                        accumulator *= invert->child->is<UnitValue>()->value();
                    }
                }

                return UnitValue{accumulator, UnitValue::canonicalUnitOf(resultBaseType)};
            }

            // 5. Return root.
            return ProductNode{std::move(simplifiedChildren)};
        }
    );
}

export template <typename T>
struct _Computed;

// export template <typename T>
// using Computed = _Computed<T>::TYPE;
//
// struct Linear {
//     f64 pixel;
//     f64 percent;
// };
//
// using PixelPercentage = Union<Linear, Calc>;
//
// struct _Computed<TypedUnitValue<NumericBaseType::LENGTH>> {
//     using TYPE = Pixel;
// };
//
// struct _Computed<TypedUnitValue<NumericBaseType::LENGTH, AcceptsPercent::YES>> {
//     using TYPE = PixelPercentage;
// };

export template <NumericBaseType TYPE, AcceptsPercent WITH_PERCENT = AcceptsPercent::NO>
struct TypedCalc {
    using UnitType = TypedUnitValue<TYPE, WITH_PERCENT>;

    Calc _inner;

    void repr(Io::Emit& e) const {
        e("{}", _inner);
    }
};

// https://drafts.csswg.org/css-values-4/#lengths
export using Length = TypedCalc<NumericBaseType::LENGTH>;

// https://drafts.csswg.org/css-values-4/#angles
export using Angle = TypedCalc<NumericBaseType::ANGLE>;

// https://drafts.csswg.org/css-values-4/#frequency
export using Frequency = TypedCalc<NumericBaseType::FREQUENCY>;

// https://drafts.csswg.org/css-values-4/#resolution
export using Resolution = TypedCalc<NumericBaseType::RESOLUTION>;

// https://drafts.csswg.org/css-values-4/#percentages
export using Percentage = TypedCalc<NumericBaseType::PERCENT>;

// https://drafts.csswg.org/css-values-4/#mixed-percentages
export using LengthPercentage = TypedCalc<NumericBaseType::LENGTH, AcceptsPercent::YES>;
export using FrequencyPercentage = TypedCalc<NumericBaseType::FREQUENCY, AcceptsPercent::YES>;
export using AnglePercentage = TypedCalc<NumericBaseType::ANGLE, AcceptsPercent::YES>;
export using TimePercentage = TypedCalc<NumericBaseType::TIME, AcceptsPercent::YES>;

} // namespace Vaev::Ng

namespace Vaev {

template <NumericBaseType TYPE, AcceptsPercent WITH_PERCENT>
struct ValueParser<Ng::TypedCalc<TYPE, WITH_PERCENT>> {
    static Res<Ng::TypedCalc<TYPE, WITH_PERCENT>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto numeric = parseValue<TypedUnitValue<TYPE, WITH_PERCENT>>(c);
        if (numeric) {
            return Ok(numeric.take().unitValue());
        }

        if (c.peek() == Css::Sst::FUNC) {
            auto calc = try$(parseMathFunction(c));

            auto type = try$(Ng::determineType(calc));

            logInfo("type: {}", type);

            if (not type.isBaseType(TYPE) or (type.containsPercentage() and WITH_PERCENT == AcceptsPercent::NO))
                return Error::invalidData("calc() type error");

            return Ok(Ng::TypedCalc<TYPE, WITH_PERCENT>{
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

            eatWhitespace(content);

            while (not content.ended()) {
                if (not content.skip(Css::Token::COMMA))
                    return Error::invalidData("unexpected token inside math function");

                eatWhitespace(content);
                children.pushBack(try$(parseCalcSum(content)));
                eatWhitespace(content);
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
            // FIXME: NaN should be matched with exact casing.
            value = Math::NAN;
        } else {
            return Error::invalidData("unknown calc keyword");
        }

        return Ok(UnitValue{value, UnitValue::Unit::NONE});
    }
};

template <NumericBaseType TYPE, AcceptsPercent WITH_PERCENT>
struct ValueSerializer<Ng::TypedCalc<TYPE, WITH_PERCENT>> {
    static Res<> serialize(Io::TextWriter& w, Ng::TypedCalc<TYPE, WITH_PERCENT> const& value) {
        return Ng::serializeMathFunction(w, value._inner);
    };
};

struct Lab {
    Lab() {
        Css::Lexer lex = {"1px"};
        auto diags = Diag::Collector::ignore();

        auto content = Css::consumeComponentValue(lex, diags);
        auto c = Cursor<Css::Sst>(content);

        auto dump = [](Ng::Calc const& calc) {
            Io::StringWriter sw{};
            Ng::serializeMathFunction(sw, calc).expect();
            logInfo("{}", sw.take());
        };

        logInfo("{}", c);

        auto parsed = parseValue<Ng::LengthPercentage>(c).expect();
        dump(parsed._inner);

        auto computed = Ng::simplify(parsed._inner);
        dump(computed);
    }
};

static Lab _lab{};

} // namespace Vaev
