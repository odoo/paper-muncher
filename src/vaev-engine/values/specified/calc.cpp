module;

#include <karm/macros>
#include <stack>

export module Vaev.Engine:values.specified.calc;

import Karm.Core;

import :css;
import :values.specified.base;
import :values.specified.length;
import :values.specified.angle;

using namespace Karm;

namespace Vaev::Experimental {

struct Node {
    virtual void codegen(ComputationContext const& ctx, Style::ComputedValues const& computed, Io::BEmit& e) const = 0;
};

struct BinopNode : Node {
    enum struct Op {
        SUM,

    };
};

struct UnopNode : Node {
    enum struct Op {

    };
};

struct VariadicNode : Node {
    SmallVec<Box<Node>, 2> childrens;
};

struct SumNode : Node {
    void codegen(ComputationContext const& ctx, Style::ComputedValues const& computed, Io::BEmit& e) const override {
        bool first = true;

        for (auto const& child : childrens) {
            child->codegen(ctx, computed, e);
            if (first) {
                first = false;
            } else {
                e.writeU8ne(Ins::ADD);
            }
        }
    }
};

struct NegateNode : Node {
    Box<Node> child;

    void codegen(ComputationContext const&, Style::ComputedValues const&, Io::BEmit& e) const override {
        e.writeU8ne(Ins::NEG);
    }
};

struct ProductNode : Node {
    SmallVec<Box<Node>, 2> childrens;

    Opt<LinearConstant> codegen(ComputationContext const& ctx, Style::ComputedValues const& computed, Io::BEmit& e) const override {
        bool first = true;

        for (auto const& child : childrens) {
            child->codegen(ctx, computed, e);
            if (first) {
                first = false;
            } else {
                e.writeU8ne(Ins::MUL);
            }
        }
    }
};

struct InvertNode : Node {
    Box<Node> child;

    void codegen(ComputationContext const&, Style::ComputedValues const&, Io::BEmit& e) const override {
        e.writeU8ne(Ins::NEG);
    }
};

struct MinMax :

    struct BinopNode : Node {
        enum class Op {
            ADD,
            SUB,
            MUL,
            DIV,
            MIN,
            MAX,
        };

        Box<Node> lhs;
        Box<Node> rhs;
        Op op;

        void codegen(ComputationContext const& ctx, Style::ComputedValues const& computed, Io::BEmit& e) override {
            lhs->codegen(ctx, computed, e);
            rhs->codegen(ctx, computed, e);

            switch (op) {
            case Op::ADD:
                e.writeU8ne(Ins::ADD);
                break;
            case Op::SUB:
                e.writeU8ne(Ins::SUB);
                break;
            case Op::MUL:
                e.writeU8ne(Ins::MUL);
                break;
            case Op::DIV:
                e.writeU8ne(Ins::DIV);
                break;
            case Op::MIN:
                e.writeU8ne(Ins::MIN);
                break;
            case Op::MAX:
                e.writeU8ne(Ins::MAX);
                break;
            }
        }
    };

    struct RoundNode : Node {
        enum struct Strategy {
            NEAREST,
            UP,
            DOWN,
            TO_ZERO,
            LINE_WIDTH,
        };

        Box<Node> a;
        Opt<Box<Node>> b;
        Strategy strategy = Strategy::NEAREST;

        void codegen(ComputationContext const& ctx, Style::ComputedValues const& computed, Io::BEmit& e) override {
            a->codegen(ctx, computed, e);

            if (b) {
                (*b)->codegen(ctx, computed, e);
                e.writeU8ne(Ins::ROUND2);
            } else {
                e.writeU8ne(Ins::ROUND);
            }
        }
    };

    struct ValueNode : Node {
        using Value = Union<Length, Angle, Percentage>;
        Value value;

        void codegen(ComputationContext const& ctx, Style::ComputedValues const& computed, Io::BEmit& e) override {
            value.visit(
                [&](Length const& length) {
                    e.writeU8ne(Ins::PUSH_NUMBER);
                    e.writeF64(computeValue(length, ctx, computed).value());
                },
                [&](Angle const& angle) {
                    e.writeU8ne(Ins::PUSH_NUMBER);
                    e.writeF64(computeValue(angle, ctx, computed).value());
                },
                [&](Percent const& angle) {
                    e.writeU8ne(Ins::PUSH_NUMBER);
                    e.writeF64(computeValue(angle, ctx, computed).value());
                }
            );
            computeValue(ctx, computed);
        }
    };

} // namespace Vaev::Experimental
