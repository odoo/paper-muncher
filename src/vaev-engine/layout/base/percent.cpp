export module Vaev.Engine:layout.percent;

import :values;

namespace Vaev::Layout {

static Au _resolveUnary(CalcOp, Au) {
    notImplemented();
}

static Au _resolveInfix(CalcOp op, Au lhs, Au rhs) {
    switch (op) {
    case CalcOp::ADD:
        return lhs + rhs;
    case CalcOp::SUBTRACT:
        return lhs - rhs;
    case CalcOp::MULTIPLY:
        return Au(f64{lhs} * f64{rhs});
    case CalcOp::DIVIDE:
        return Au(f64{lhs} / f64{rhs});
    default:
        panic("unexpected operator");
    }
}

export Au resolveLength(Px length) {
    return Au{length.value()};
}

export Au resolvePercent(Percent percent, Au relative) {
    return relative * percent.value();
}

export Au resolvePercentLength(PercentOr<Px> const& percentLength, Au relative) {
    return percentLength.visit(Visitor{
        [&](Percent const& percent) {
            return resolvePercent(percent, relative);
        },
        [](Px const& length) {
            return resolveLength(length);
        }
    });
}

export Au resolvePercentLength(CalcValue<PercentOr<Px>> const& calc, Au relative) {
    auto resolveUnion = Visitor{
        [&](PercentOr<Px> const& v) {
            return resolvePercentLength(v, relative);
        },
        [&](CalcValue<PercentOr<Px>>::Leaf const& v) {
            return resolvePercentLength(*v, relative);
        },
        [&](Number const& v){
            return Au(v);
        }
    };

    return calc.visit(Visitor{
        [&](CalcValue<PercentOr<Px>>::Value const& v) {
            return v.visit(resolveUnion);
        },
        [&](CalcValue<PercentOr<Px>>::Unary const& u) {
            return _resolveUnary(
                u.op,
                u.val.visit(resolveUnion)
            );
        },
        [&](CalcValue<PercentOr<Px>>::Binary const& b) {
            return _resolveInfix(
                b.op,
                b.lhs.visit(resolveUnion),
                b.rhs.visit(resolveUnion)
            );
        },
    });
}

} // namespace Vaev::Layout
