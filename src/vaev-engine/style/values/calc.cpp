export module Vaev.Engine:style.values.calc;

import Karm.Core;
import Karm.Math;

import :css.values.calc;
import :style.values.unitValue;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-values/#calc-computed-value
// A math function that can't be fully simplified at compute time stays a
// calculation tree in the computed value.
export using Css::Calc;

// MARK: Simplification --------------------------------------------------------
// NOTE: Vaev doesn't simplify parsed values: calculation trees are simplified
//       when computed (relative units) and when used (percentages).

f64 apply(Css::FnNode::Name name, usize count, auto get) {
    using enum Css::FnNode::Name;

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

// https://drafts.csswg.org/css-values-4/#sort-a-calculations-children
void sortCalculationChildren(MutSlice<Calc> children) {
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
            return toCanonical(numeric, ctx);
        },
        // 4. If root is an operator node that’s not one of the calc-operator nodes, and all of its calculation children
        //    are numeric values with enough information to compute the operation root represents, return the result of
        //    running root’s operation using its children, expressed in the result’s canonical unit.
        [&](Css::FnNode const& f) -> Calc {
            auto simplifiedChildren = simplifyChildren(f.children);

            for (auto const& simplifiedChild : simplifiedChildren) {
                if (auto [numeric] = simplifiedChild.template is<UnitValue>()) {
                    // If a percentage is left at this point, it will usually block simplification of the node,
                    // since it needs to be resolved against another value using information not currently available.
                    // (Otherwise, it would have been converted to a different value in an earlier step.) This includes operations such as "min",
                    // since percentages might resolve against a negative basis, and thus end up with an opposite comparative relationship than
                    // the raw percentage value would seem to indicate.
                    if (numeric.unit() == UnitValue::Unit::PERCENT and not(mainType and mainType.expect() == NumericBaseType::PERCENT))
                        return Css::FnNode{UNCHECKED, f.name, std::move(simplifiedChildren)};
                } else {
                    return Css::FnNode{UNCHECKED, f.name, std::move(simplifiedChildren)};
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
        [&](Css::NegateNode const& negate) -> Calc {
            auto simplifiedChild = simplifyChild(*negate.child);

            return simplifiedChild.visit(
                // 1. If root’s child is a numeric value,
                [](UnitValue& child) -> Calc {
                    // return an equivalent numeric value, but with the value negated (0 - value).
                    return UnitValue{-child.value(), child.unit()};
                },
                // 2. If root’s child is a Negate node,
                [](Css::NegateNode& child) -> Calc {
                    // return the child’s child.
                    return std::move(*child.child);
                },
                // 3. If root’s child is a Sum node:
                [&](Css::SumNode& child) -> Calc {
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
                        if (auto [negate] = grandchild.is<Css::NegateNode>()) {
                            // append grandchild’s child to negated grandchildren
                            negatedGrandChildren.pushBack(std::move(*negate.child));
                            continue;
                        }

                        // Otherwise, create a Negate node with grandchild as its child,
                        // and append the result to negated grandchildren
                        negatedGrandChildren.pushBack(Css::NegateNode{makeBox<Calc>(std::move(grandchild))});
                    }

                    // 3. Return a Sum node with negated grandchildren as its children
                    return Css::SumNode{std::move(negatedGrandChildren)};
                },
                // Return root.
                [](auto& other) -> Calc {
                    return Css::NegateNode{makeBox<Calc>(std::move(other))};
                }
            );
        },
        // 7. If root is an Invert node:
        [&](Css::InvertNode const& invert) -> Calc {
            auto simplifiedChild = simplifyChild(*invert.child);

            // 1. If root’s child is a number (not a percentage or dimension)
            if (auto child = simplifiedChild.template is<UnitValue>(); child and not child->baseType()) {
                // return the reciprocal of the child’s value.
                return UnitValue{1.0 / child->value(), UnitValue::Unit::NONE};
            }

            // 2. If root’s child is an Invert node,
            if (auto [child] = simplifiedChild.template is<Css::InvertNode>()) {
                // Return root.
                return std::move(*child.child);
            }

            // 3. Return root.
            return Css::InvertNode{makeBox<Calc>(std::move(simplifiedChild))};
        },
        // 8. If root is a Sum node:
        [&](Css::SumNode const& sum) -> Calc {
            // 1. For each of root’s children that are Sum nodes, replace them with their children.
            auto simplifiedChildren = Vec<Calc>{sum.children.len()};
            for (auto& child : sum.children) {
                auto simplifiedChild = simplifyChild(child);
                if (auto [child] = simplifiedChild.template is<Css::SumNode>()) {
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
            return Css::SumNode{std::move(simplifiedChildren)};
        },
        // 9. If root is a Product node:
        [&](Css::ProductNode const& product) -> Calc {
            // 1. For each of root’s children that are Product nodes, replace them with their children.
            auto simplifiedChildren = Vec<Calc>{product.children.len()};
            for (auto& child : product.children) {
                auto simplifiedChild = simplifyChild(child);
                if (auto [child] = simplifiedChild.template is<Css::ProductNode>()) {
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

                auto const onlyNumericChildren = [](Css::SumNode const& sum) {
                    for (auto const& child : sum.children) {
                        if (not child.is<UnitValue>())
                            return false;
                    }
                    return true;
                };

                // NOTE: We know it must happen in this order because children are sorted.
                auto maybeNumber = simplifiedChildren[0].is<UnitValue>();
                auto maybeSum = simplifiedChildren[1].is<Css::SumNode>();

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

                if (auto invert = child.is<Css::InvertNode>(); invert and invert->child->is<UnitValue>() and invert->child->is<UnitValue>()->isCanonical())
                    continue;

                containsOnlyCanonicalNumerics = false;
                break;
            }

            auto type = Css::determineType(product, NONE);
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
                    } else if (auto invert = child.is<Css::InvertNode>()) {
                        accumulator /= invert->child->is<UnitValue>()->value();
                    }
                }

                return UnitValue{accumulator, *resultUnit};
            }

            // 5. Return root.
            return Css::ProductNode{std::move(simplifiedChildren)};
        }
    );
}

} // namespace Vaev::Style
