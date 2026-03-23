module;

#include <karm/macros>

export module Vaev.Engine:values.calc;

import Karm.Core;

import :css;
import :values.base;
import :values.length;
import :values.angle;
// import :values.frequency;
// import :values.resolution;
// import :values.time;
import :values.resolved;
import :values.percent;
import :values.primitives;

using namespace Karm;

namespace Vaev {

enum struct NumericBaseType {
    LENGTH,
    ANGLE,
    TIME,
    FREQUENCY,
    RESOLUTION,
    FLEX,
    PERCENT,

    _LEN,
};

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
        percentHint = hint;
    }

    // https://drafts.css-houdini.org/css-typed-om-1/#cssnumericvalue-add-two-types
    Opt<NumericType> add(NumericType const& other) const {
        if (percentHint and other.percentHint and *percentHint != *other.percentHint)
            return NONE;

        auto type1 = *this, type2 = other;

        if (type1.percentHint and not type2.percentHint) {
            type2.applyPercentHint(*type1.percentHint);
        } else if (type2.percentHint and not type1.percentHint) {
            type1.applyPercentHint(*type2.percentHint);
        }

        if (type1._els == type2._els) {
            auto finalType = type1;
            if (not finalType.percentHint)
                finalType.percentHint = type2.percentHint;
            return finalType;
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
                    t1.percentHint = hint;
                    return t1;
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

        for (usize i = 0; i < toUnderlyingType(NumericBaseType::_LEN); i++) {
            finalType._els[i] += other._els[i];
        }

        if (not finalType.percentHint) {
            finalType.percentHint = other.percentHint;
        }

        return finalType;
    }

    NumericType invert() const {
        NumericType finalType = *this;

        for (usize i = 0; i < toUnderlyingType(NumericBaseType::_LEN); i++) {
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
};

using _NumericValue = Union<Length, Angle, Percent, Number, Integer>;

struct NumericValue : _NumericValue {
    using _NumericValue::_NumericValue;

    Opt<NumericValue> add(NumericValue other) {
        if (index() != other.index()) {
            return NONE;
        }

        return visit(Visitor{
            [&](Number number) -> Opt<NumericValue> {
                return number + other.unwrap<Number>();
            },

            [&](Integer integer) -> Opt<NumericValue> {
                return integer + other.unwrap<Integer>();
            },

            [&](Percent const& percent) -> Opt<NumericValue> {
                return Percent(percent.value() + other.unwrap<Percent>().value());
            },

            [&](Length const& length) -> Opt<NumericValue> {
                if (length.unit() != other.unwrap<Length>().unit()) {
                    return NONE;
                }

                return Length(length.val() + other.unwrap<Length>().val(), Length::PX);
            },

            [&](Angle const& angle) -> Opt<NumericValue> {
                if (angle.unit() != other.unwrap<Angle>().unit()) {
                    return NONE;
                }

                return Angle::fromDegree(angle.val() + other.unwrap<Angle>().val());
            },
        });
    }

    Opt<NumericValue> multiply(NumericValue other) {
        if (index() != other.index()) {
            return NONE;
        }

        return visit(Visitor{
            [&](Number number) -> Opt<NumericValue> {
                return number * other.unwrap<Number>();
            },

            [&](Integer integer) -> Opt<NumericValue> {
                return integer * other.unwrap<Integer>();
            },

            [&](Percent const& percent) -> Opt<NumericValue> {
                return Percent(percent.value() * other.unwrap<Percent>().value());
            },

            [&](Length const& length) -> Opt<NumericValue> {
                if (length.unit() != other.unwrap<Length>().unit()) {
                    return NONE;
                }

                return Length(length.val() * other.unwrap<Length>().val(), Length::PX);
            },

            [&](Angle const& angle) -> Opt<NumericValue> {
                if (angle.unit() != other.unwrap<Angle>().unit()) {
                    return NONE;
                }

                return Angle::fromDegree(angle.val() * other.unwrap<Angle>().val());
            },
        });
    }

    NumericValue negate() const {
        return visit(Visitor{
            [](Length const& length) {
                return length.negated();
            },
            [](Number number) {
                return -number;
            },
            [](auto&&) -> NumericValue {
                notImplemented();
            },
        });
    }

    Opt<NumericValue> min(NumericValue other) const {
        if (index() != other.index()) {
            return NONE;
        }

        return visit(Visitor{
            [&](Number number) -> Opt<NumericValue> {
                return Karm::min(number, other.unwrap<Number>());
            },

            [&](Integer integer) -> Opt<NumericValue> {
                return Karm::min(integer, other.unwrap<Integer>());
            },

            [](Percent) -> Opt<NumericValue> {
                return NONE;
            },

            [&](Length const& length) -> Opt<NumericValue> {
                if (length.unit() != other.unwrap<Length>().unit()) {
                    return NONE;
                }

                return Karm::min(length, other.unwrap<Length>());
            },

            [&](Angle const& angle) -> Opt<NumericValue> {
                if (angle.unit() != other.unwrap<Angle>().unit()) {
                    return NONE;
                }

                return Karm::min(angle, other.unwrap<Angle>());
            },
        });
    }

    Opt<NumericValue> max(NumericValue other) const {
        if (index() != other.index()) {
            return NONE;
        }

        return visit(Visitor{
            [&](Number number) -> Opt<NumericValue> {
                return Karm::max(number, other.unwrap<Number>());
            },

            [&](Integer integer) -> Opt<NumericValue> {
                return Karm::max(integer, other.unwrap<Integer>());
            },

            [](Percent) -> Opt<NumericValue> {
                return NONE;
            },

            [&](Length const& length) -> Opt<NumericValue> {
                if (length.unit() != other.unwrap<Length>().unit()) {
                    return NONE;
                }

                return Karm::max(length, other.unwrap<Length>());
            },

            [&](Angle const& angle) -> Opt<NumericValue> {
                if (angle.unit() != other.unwrap<Angle>().unit()) {
                    return NONE;
                }

                return Karm::max(angle, other.unwrap<Angle>());
            },
        });
    }

    void repr(Io::Emit& e) const {
        visit([&](auto&& val) {
            e("{}", val);
        });
    }
};

struct CalcNode {
    NumericType type;

    CalcNode(NumericType type) : type(type) {}

    virtual ~CalcNode() = default;

    virtual Opt<Rc<CalcNode>> computeMathFunc() const {
        return NONE;
    };

    virtual void repr(Io::Emit& e) const = 0;
};

struct NumericNode : CalcNode {
    NumericValue value;

    NumericNode(NumericType type, NumericValue value) : CalcNode(type), value(value) {}

    static Rc<NumericNode> create(NumericValue value) {
        // https://drafts.css-houdini.org/css-typed-om-1/#cssnumericvalue-create-a-type
        NumericType type = value.visit(Visitor{
            [](Number) {
                return NumericType();
            },

            [](Integer) {
                return NumericType();
            },

            [](Percent) {
                return NumericType(NumericBaseType::PERCENT);
            },

            [](Length) {
                return NumericType(NumericBaseType::LENGTH);
            },

            [](Angle) {
                return NumericType(NumericBaseType::ANGLE);
            },
        });

        return makeRc<NumericNode>(type, value);
    }

    Number coefficient() const {
        return this->value.visit(Visitor{
            [&](Length const& length) {
                return length.val();
            },
            [&](Angle const& angle) {
                return angle.val();
            },
            [&](Number const& number) {
                return number;
            },
            [](auto&) -> bool {
                notImplemented();
            },
        });
    }

    bool isSameUnit(NumericNode const& other) const {
        if (this->value.index() != other.value.index()) {
            return false;
        }

        return this->value.visit(Visitor{
            [&](Length const& length) {
                return length.unit() == other.value.unwrap<Length>().unit();
            },
            [&](Angle const& angle) {
                return angle.unit() == other.value.unwrap<Angle>().unit();
            },
            [&](Number const&) {
                return true;
            },
            [](auto&) -> bool {
                notImplemented();
            },
        });
    }

    MutCursor<NumericNode> findNodeWithSameUnit(MutSlice<Rc<CalcNode>> nodes) {
        for (auto& node : nodes) {
            if (auto it = node.is<NumericNode>(); isSameUnit(*it)) {
                return it;
            }
        }
        return NONE;
    }

    bool isCanonical() const {
        return value.visit(Visitor{
            [](Length const& length) {
                return length.unit() == Length::PX;
            },
            [](Angle const& angle) {
                return angle.unit() == Angle::Unit::DEGREE;
            },
            [&](Number const&) {
                return true;
            },
            [](const auto) -> bool {
                notImplemented();
            },
        });
    }

    Opt<Rc<NumericNode>> canonicalize() const {
        return value.visit(Visitor{
            [](Length const& length) -> Opt<Rc<NumericNode>> {
                if (auto l = length.canonicalized()) {
                    return NumericNode::create(*l);
                }

                return NONE;
            },
            [](const auto) {
                return NONE;
            },
        });
    }

    void repr(Io::Emit& e) const override {
        e("{}", value);
    }
};

struct NegateNode : CalcNode {
    Rc<CalcNode> child;

    NegateNode(NumericType type, Rc<CalcNode> child) : CalcNode(type), child(child) {}

    static Rc<NegateNode> create(Rc<CalcNode> child) {
        return makeRc<NegateNode>(child->type, child);
    }

    void repr(Io::Emit& e) const override {
        e("(neg {})", child);
    }
};

struct InvertNode : CalcNode {
    Rc<CalcNode> child;

    InvertNode(NumericType type, Rc<CalcNode> child) : CalcNode(type), child(child) {}

    static Res<Rc<InvertNode>> create(Rc<CalcNode> child) {
        auto type = child->type.invert();
        return Ok(makeRc<InvertNode>(type, child));
    }

    void repr(Io::Emit& e) const override {
        e("(inv {})", child);
    }
};

struct SumNode : CalcNode {
    Vec<Rc<CalcNode>> childrens;

    SumNode(NumericType type, Vec<Rc<CalcNode>> childrens)
        : CalcNode(type),
          childrens(std::move(childrens)) {}

    static Res<Rc<SumNode>> create(Vec<Rc<CalcNode>> childrens) {
        Opt<NumericType> type = NONE;

        for (auto const& child : childrens) {
            if (not type) {
                type = child->type;
            } else {
                type = try$(type->add(child->type).okOr(Error::other("type error")));
            }
        }

        if (not type)
            return Error::other("empty sum");

        return Ok(makeRc<SumNode>(type.unwrap(), std::move(childrens)));
    }

    void repr(Io::Emit& e) const override {
        e("(sum {})", childrens);
    }
};

struct ProductNode : CalcNode {
    Vec<Rc<CalcNode>> childrens;

    ProductNode(NumericType type, Vec<Rc<CalcNode>> childrens)
        : CalcNode(type),
          childrens(std::move(childrens)) {}

    static Res<Rc<ProductNode>> create(Vec<Rc<CalcNode>> childrens) {
        Opt<NumericType> type = NONE;

        for (auto const& child : childrens) {
            if (not type) {
                type = child->type;
            } else {
                type = try$(type->multiply(child->type).okOr(Error::other("type error")));
            }
        }

        if (not type)
            return Error::other("empty product");

        return Ok(makeRc<ProductNode>(type.unwrap(), std::move(childrens)));
    }

    void repr(Io::Emit& e) const override {
        e("(prod {})", childrens);
    }
};

struct MinMaxNode : CalcNode {
    Vec<Rc<CalcNode>> childrens{};

    enum class Op {
        MIN,
        MAX,
    };

    Op op;

    MinMaxNode(NumericType type, Vec<Rc<CalcNode>> childrens, Op op)
        : CalcNode(type),
          childrens(std::move(childrens)),
          op(op) {}

    static Res<Rc<MinMaxNode>> create(Op op, Vec<Rc<CalcNode>> childrens) {
        Opt<NumericType> type = NONE;

        for (auto const& child : childrens) {
            if (not type) {
                type = child->type;
            } else {
                type = try$(type->add(child->type).okOr(Error::other("type error")));
            }
        }

        if (not type)
            return Error::other("empty product");

        return Ok(makeRc<MinMaxNode>(type.unwrap(), std::move(childrens), op));
    }

    Opt<Rc<CalcNode>> computeMathFunc() const override {
        Cursor<NumericNode> reference = NONE;
        NumericValue accumulator = 0.0;

        for (auto const& child : childrens) {
            auto num = child.is<NumericNode>();

            // FIXME: Does input need to be canonical?
            if (not num or not num->isCanonical() or num->type.percentHint) {
                return NONE;
            }

            if (not reference) {
                reference = num;
                accumulator = num->value;
                continue;
            }

            accumulator = op == Op::MIN ? try$(accumulator.min(num->value)) : try$(accumulator.max(num->value));
        }

        return NumericNode::create(accumulator);
    }

    void repr(Io::Emit& e) const override {
        if (op == Op::MIN) {
            e("(min {})", childrens);
        } else {
            e("(max {})", childrens);
        }
    }
};

static Rc<CalcNode> simplify(Rc<CalcNode> const root);

static Rc<CalcNode> simplifyChildren(Rc<CalcNode> const& root) {
    if (auto it = root.cast<NumericNode>()) {
        return it.unwrap();
    }

    if (auto it = root.is<MinMaxNode>()) {
        Vec<Rc<CalcNode>> newChildren;

        bool newSubtree = false;

        for (auto child : it->childrens) {
            auto simplifiedChild = simplify(child);

            if (not newSubtree and &simplifiedChild.unwrap() != &child.unwrap()) {
                newSubtree = true;
            }

            newChildren.pushBack(simplifiedChild);
        }

        if (newSubtree) {
            return MinMaxNode::create(it->op, std::move(newChildren)).unwrap();
        } else {
            return root;
        }
    }

    if (auto it = root.is<NegateNode>()) {
        auto simplifiedChild = simplify(it->child);

        if (&simplifiedChild.unwrap() != &it->child.unwrap()) {
            return NegateNode::create(simplifiedChild);
        } else {
            return root;
        }
    }

    if (auto it = root.is<InvertNode>()) {
        auto simplifiedChild = simplify(it->child);

        if (&simplifiedChild.unwrap() != &it->child.unwrap()) {
            return InvertNode::create(simplifiedChild).unwrap();
        } else {
            return root;
        }
    }

    if (auto it = root.is<SumNode>()) {
        Vec<Rc<CalcNode>> newChildren;

        bool newSubtree = false;

        for (auto child : it->childrens) {
            auto simplifiedChild = simplify(child);

            if (not newSubtree and &simplifiedChild.unwrap() != &child.unwrap()) {
                newSubtree = true;
            }

            newChildren.pushBack(simplifiedChild);
        }

        if (newSubtree) {
            return SumNode::create(std::move(newChildren)).unwrap();
        } else {
            return root;
        }
    }

    if (auto it = root.is<ProductNode>()) {
        Vec<Rc<CalcNode>> newChildren;

        bool newSubtree = false;

        for (auto child : it->childrens) {
            auto simplifiedChild = simplify(child);

            if (not newSubtree and &simplifiedChild.unwrap() != &child.unwrap()) {
                newSubtree = true;
            }

            newChildren.pushBack(simplifiedChild);
        }

        if (newSubtree) {
            return ProductNode::create(std::move(newChildren)).unwrap();
        } else {
            return root;
        }
    }

    unreachable();
}

static Rc<CalcNode> simplifyInner(Rc<CalcNode> const node) {
    auto const root = simplifyChildren(node);

    // 1. If root is a numeric value:
    if (auto it = root.is<NumericNode>()) {
        // TODO
        // 1. If root is a percentage that will be resolved against another value,
        // and there is enough information available to resolve it, do so, and express
        // the resulting numeric value in the appropriate canonical unit. Return the value.

        // 2. If root is a dimension that is not expressed in its canonical unit,
        //    and there is enough information available to convert it to the canonical unit,
        //    do so, and return the value.
        if (auto canonicalized = it->canonicalize()) {
            return *canonicalized;
        }

        // TODO
        // 3. If root is a <calc-keyword> that can be resolved, return what it resolves to, simplified.

        // Otherwise, return root.
        return root;
    }

    // 2. If root is any other leaf node (not an operator node):
    // NOTE: There is no such node.

    // 3. At this point, root is an operator node. Simplify all the calculation children of root.
    // NOTE: We've already simplified above.

    // 4. If root is an operator node that’s not one of the calc-operator nodes, and all of its calculation
    //    children are numeric values with enough information to compute the operation root represents,
    //    return the result of running root’s operation using its children, expressed in the result’s canonical unit.
    if (auto it = root->computeMathFunc()) {
        return *it;
    }

    // 7. If root is a Min or Max node, attempt to partially simplify it:
    if (auto node = root.is<MinMaxNode>()) {
        (void)(node);
        // TODO
        return root;
    }

    // 6. If root is a Negate node:
    if (auto node = root.is<NegateNode>()) {
        // 1. If root’s child is a numeric value, return an equivalent numeric value,
        //    but with the value negated (0 - value).
        if (auto child = node->child.is<NumericNode>()) {
            return NumericNode::create(child->value.negate());
        }

        // 2. If root’s child is a Negate node, return the child’s child.
        if (auto child = node->child.is<NegateNode>()) {
            return child->child;
        }

        // 3. Return root.
        return root;
    }

    // 7. If root is an Invert node:
    if (auto node = root.is<InvertNode>()) {
        // 1. If root’s child is a number (not a percentage or dimension)
        //    return the reciprocal of the child’s value.
        if (auto child = node->child.is<NumericNode>()) {
            if (auto number = child->value.is<Number>()) {
                return NumericNode::create(1.0 / *number);
            }
        }

        // 2. If root’s child is an Invert node, return the child’s child.
        if (auto child = node->child.is<InvertNode>()) {
            return child->child;
        }

        // 3. Return root.
        return root;
    };

    // 8. If root is a Sum node:
    if (auto node = root.is<SumNode>()) {
        // 1. For each of root’s children that are Sum nodes, replace them with their children.
        Vec<Rc<CalcNode>> flattenedChildrens(node->childrens.len());
        for (auto const& child : node->childrens) {
            if (auto it = child.is<SumNode>()) {
                flattenedChildrens.pushBack(it->childrens);
            } else {
                flattenedChildrens.pushBack(child);
            }
        }

        // 2. For each set of root’s children that are numeric values with identical units, remove those
        // children and replace them with a single numeric value containing the sum of the removed
        // nodes, and with the same unit.
        Vec<Rc<CalcNode>> simplifiedChildrens;

        for (auto& child : flattenedChildrens) {
            if (auto it = child.is<NumericNode>()) {
                if (auto dest = it->findNodeWithSameUnit(simplifiedChildrens)) {
                    auto result = dest->value.add(it->value);

                    if (not result)
                        continue;

                    dest->value = *result;
                } else {
                    simplifiedChildrens.pushBack(child);
                }
            } else {
                simplifiedChildrens.pushBack(child);
            }
        }

        // 3. If root has only a single child at this point, return the child.
        if (simplifiedChildrens.len() == 1) {
            return first(simplifiedChildrens);
        }

        // Otherwise, return root.
        return SumNode::create(std::move(simplifiedChildrens)).unwrap();
    }

    // 9. If root is a Product node:
    if (auto node = root.is<ProductNode>()) {
        // 1. For each of root’s children that are Product nodes, replace them with their children.
        Vec<Rc<CalcNode>> flattenedChildrens(node->childrens.len());
        for (auto const& child : node->childrens) {
            if (auto it = child.is<ProductNode>()) {
                flattenedChildrens.pushBack(it->childrens);
            } else {
                flattenedChildrens.pushBack(child);
            }
        }

        // 2. If root has multiple children that are numbers (not percentages or dimensions),
        //    remove them and replace them with a single number containing the product of the removed nodes.
        Vec<Rc<CalcNode>> numberFoldedChildrens;

        Opt<Number> accumulator = NONE;

        for (auto& child : flattenedChildrens) {
            auto numericNode = child.is<NumericNode>();
            auto number = numericNode ? numericNode->value.is<Number>() : NONE;

            if (number) {
                if (not accumulator) {
                    accumulator = *number;
                } else {
                    *accumulator *= *number;
                }
            } else {
                numberFoldedChildrens.pushBack(child);
            }
        }

        if (accumulator) {
            numberFoldedChildrens.pushBack(NumericNode::create(*accumulator));
        }

        // 3. If root contains only two children, one of which is a number (not a percentage or dimension)
        //    and the other of which is a Sum whose children are all numeric values, multiply all of the Sum’s
        //    children by the number, then return the Sum.

        Vec<Rc<CalcNode>> distributedChildrens = std::move(flattenedChildrens);
        distributedChildrens.clear();

        // 4. If root contains only numeric values and/or Invert nodes containing numeric values, and multiplying
        //    the types of all the children (noting that the type of an Invert node is the inverse of its child’s type)
        //    results in a type that matches any of the types that a math function can resolve to, return the result of
        //    multiplying all the values of the children (noting that the value of an Invert node is the reciprocal of its child’s value),
        //    expressed in the result’s canonical unit.
        Opt<Number> multiplicationAccumulator = NONE;

        for (auto const& child : numberFoldedChildrens) {
            if (auto it = child.is<NumericNode>()) {
                if (not it->isCanonical()) {
                    return ProductNode::create(std::move(numberFoldedChildrens)).unwrap();
                }

                if (not multiplicationAccumulator) {
                    multiplicationAccumulator = it->coefficient();
                } else {
                    *multiplicationAccumulator *= it->coefficient();
                }
            } else if (auto it = child.is<InvertNode>()) {
                if (auto numeric = it->child.is<NumericNode>()) {
                    if (not numeric->isCanonical()) {
                        return ProductNode::create(std::move(numberFoldedChildrens)).unwrap();
                    }

                    if (not multiplicationAccumulator) {
                        multiplicationAccumulator = 1.0 / numeric->coefficient();
                    } else {
                        *multiplicationAccumulator *= 1.0 / numeric->coefficient();
                    }
                } else {
                    return ProductNode::create(std::move(numberFoldedChildrens)).unwrap();
                }
            }
        }

        if (multiplicationAccumulator) {
            if (node->type.isBaseType(NumericBaseType::LENGTH)) {
                return NumericNode::create(Length{*multiplicationAccumulator, Length::PX});
            } else if (node->type.isBaseType(NumericBaseType::ANGLE)) {
                return NumericNode::create(Angle::fromDegree(*multiplicationAccumulator));
            } else if (node->type.isBaseType(NumericBaseType::PERCENT)) {
                return NumericNode::create(Percent{*multiplicationAccumulator});
            } else if (node->type.isEmpty()) {
                return NumericNode::create(*multiplicationAccumulator);
            }
        }

        // 5. Return root
        // FIXME: Reuse nodes
        return ProductNode::create(std::move(numberFoldedChildrens)).unwrap();
    }

    notImplemented();
}

static Rc<CalcNode> simplify(Rc<CalcNode> const node) {
    auto res = simplifyInner(node);
    logInfo("{} => {}", node, res);
    return res;
}

export template <typename T>
struct CalcValue {
    using Inner = Union<Rc<CalcNode>, T>;
    Inner _inner;

    constexpr CalcValue()
        : CalcValue(T{}) {
    }

    constexpr CalcValue(Meta::Convertible<T> auto val)
        : _inner(T{val}) {
    }

    constexpr CalcValue(Rc<CalcNode> tree)
        : _inner(tree) {
    }

    void repr(Io::Emit& e) const {
        e("(calc {})", _inner);
    }
};

export template <typename... Ts>
struct CalcValueNg {
    Rc<CalcNode> tree;

    void repr(Io::Emit& e) const {
        e("(calc {})", tree);
    }
};

export template <typename T>
struct ValueParser<CalcValue<T>> {
    static Res<CalcValue<T>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Sst::FUNC) {
            return Ok(CalcValue<T>{try$(parseValue<CalcValueNg<T>>(c)).tree});
        }

        return parseValue<T>(c);
    }
};

export template <typename... Ts>
constexpr NumericType cppTypeToNumericType() {
    NumericType type;
    Opt<NumericType> percentHint = NONE;

    if constexpr (Meta::Contains<Length, Ts...>) {
        type[NumericBaseType::LENGTH] = 1;
    }

    if constexpr (Meta::Contains<Angle, Ts...>) {
        type[NumericBaseType::ANGLE] = 1;
    }

    return type;
}

export template <typename... Ts>
struct ValueParser<CalcValueNg<Ts...>> {
    static Res<CalcValueNg<Ts...>> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Sst::FUNC) {
            auto root = parseMathFunction(c).unwrap();
            logInfo("input: {}", CalcValueNg(root));
            root = simplify(root);
            logInfo("simpl: {}", CalcValueNg(root));

            if (root->type != cppTypeToNumericType<Ts...>()) {
                return Error::invalidData("incompatible math function return type");
            }

            return Ok(CalcValueNg<Ts...>{root});

        }

        return Error::invalidData();
    }


    static Res<Rc<CalcNode>> parseMathFunction(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        auto const& prefix = c.peek().prefix;
        auto prefixToken = prefix.unwrap()->token;

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
            auto op = prefixToken.data == "min(" ? MinMaxNode::Op::MIN : MinMaxNode::Op::MAX;

            Cursor<Css::Sst> content = c.peek().content;

            Vec<Rc<CalcNode>> children;

            children.pushBack(try$(parseCalcSum(content)));

            while (not content.ended() and content.peek() == Css::Token::COMMA) {
                content.next();
                children.pushBack(try$(parseCalcSum(content)));
            }

            c.next();

            return MinMaxNode::create(op, std::move(children));
        }

        return Error::notImplemented("math function not implemented");
    }

    // <calc-sum> = <calc-product> [ [ '+' | '-' ] <calc-product> ]*
    static Res<Rc<CalcNode>> parseCalcSum(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        eatWhitespace(c);

        auto fst = try$(parseCalcProduct(c));

        if (c.ended() or not oneOf(c.peek().token.data, "+", "-"))
            return Ok(fst);

        Vec<Rc<CalcNode>> children;

        children.pushBack(fst);

        while (not c.ended() and oneOf(c.peek().token.data, "+", "-")) {
            if (c.peek().token.data == "+") {
                c.next();
                auto term = try$(parseCalcProduct(c));
                children.pushBack(term);
            } else {
                c.next();
                auto term = try$(parseCalcProduct(c));
                children.pushBack(NegateNode::create(term));
            }
        }

        eatWhitespace(c);

        return SumNode::create(children);
    }

    // <calc-product> = <calc-value> [ [ '*' | / ] <calc-value> ]*
    static Res<Rc<CalcNode>> parseCalcProduct(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        eatWhitespace(c);

        auto fst = try$(parseCalcValue(c));

        if (c.ended() or not oneOf(c.peek().token.data, "*", "/"))
            return Ok(fst);

        Vec<Rc<CalcNode>> children;

        children.pushBack(fst);

        while (not c.ended() and oneOf(c.peek().token.data, "*", "/")) {
            if (c.peek().token.data == "*") {
                c.next();
                auto factor = try$(parseCalcValue(c));
                children.pushBack(factor);
            } else {
                c.next();
                auto factor = try$(parseCalcValue(c));
                children.pushBack(try$(InvertNode::create(factor)));
            }
        }

        eatWhitespace(c);

        return ProductNode::create(children);
    }

    // <calc-value> = <number> | <dimension> | <percentage> |
    //           <calc-keyword> | ( <calc-sum> ) | <math-function>
    static Res<Rc<CalcNode>> parseCalcValue(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        eatWhitespace(c);

        if (c.peek().token == Css::Token::NUMBER) {
            auto number = try$(parseValue<Number>(c));
            eatWhitespace(c);
            return Ok(NumericNode::create(number));
        }

        if (c.peek().token == Css::Token::DIMENSION) {
            if (auto length = parseValue<Length>(c).ok()) {
                eatWhitespace(c);
                return Ok(NumericNode::create(*length));
            }

            if (auto angle = parseValue<Angle>(c).ok()) {
                eatWhitespace(c);
                return Ok(NumericNode::create(*angle));
            }

            return Error::invalidData("unsupported dimension");
        }

        if (c.peek().token == Css::Token::PERCENTAGE) {
            auto percent = try$(parseValue<Percent>(c));
            eatWhitespace(c);
            return Ok(NumericNode::create(percent));
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
    static Res<Rc<CalcNode>> parseCalcKeyword(Cursor<Css::Sst>& c) {
        NumericValue value = 0.0;
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

        return Ok(NumericNode::create(value));
    }


};


// export template<typename T>
// T simplifyOrResolveCalc(T u) {
//     return u.visit(Visitor{
//         [](CalcValueNg const& calc) {
//             // TODO
//             return calc;
//         },
//         [](auto&& other) {
//             return other;
//         }
//     });
// }

template <typename... Ts>
using CalcUnion = Union<Ts..., CalcValueNg<Ts...>>;

export using LengthPercentage = CalcUnion<Length, Percent>;


} // namespace Vaev
