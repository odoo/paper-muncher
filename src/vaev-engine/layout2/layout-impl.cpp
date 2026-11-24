module Vaev.Engine;

import :layout.box;

import :layout2.block;
import :layout.values;

namespace Vaev::Layout2 {

// FIXME: This should be removed after the calc refactor because it'll be unnecessary.
// static bool _calcContainsPercents(Calc<PercentOr<Length>> const& calc) {
//     auto resolveUnion = Visitor{
//         [&](PercentOr<Length> const& v) -> bool {
//             return static_cast<bool>(v.is<Percent>());
//         },
//         [&](Calc<PercentOr<Length>>::Leaf const& v) -> bool {
//             return _calcContainsPercents(*v);
//         },
//         [](Number const&) -> bool {
//             return false;
//         }
//     };
//
//     return calc.visit(
//         [&](Calc<PercentOr<Length>>::Value const& v) -> bool {
//             return v.visit(resolveUnion);
//         },
//         [&](Calc<PercentOr<Length>>::Unary const& u) -> bool {
//             return u.val.visit(resolveUnion);
//         },
//         [&](Calc<PercentOr<Length>>::Binary const& b) -> bool {
//             return b.lhs.visit(resolveUnion) or b.rhs.visit(resolveUnion);
//         }
//     );
// }

Metrics computeMetrics(Layout::Box const& box, LogicalSize<Opt<Au>> const& containingBlock) {
    auto paddingLeft = Layout::resolve(
        box.style->padding->start,
        containingBlock.inline_.unwrapOr(0_au)
    );

    auto paddingRight = Layout::resolve(
        box.style->padding->end,
        containingBlock.inline_.unwrapOr(0_au)
    );

    auto paddingTop = Layout::resolve(
        box.style->padding->top,
        containingBlock.inline_.unwrapOr(0_au)
    );

    auto paddingBottom = Layout::resolve(
        box.style->padding->bottom,
        containingBlock.inline_.unwrapOr(0_au)
    );

    // FIXME: Implement writing-mode translation
    auto paddings = LogicalInsets{
        .inlineStart = paddingLeft,
        .inlineEnd = paddingRight,
        .blockStart = paddingTop,
        .blockEnd = paddingBottom,
    };

    // FIXME: Move the resolution of borders for computed values, this should just be a cast to Au.
    auto physicalBorders = Layout::computeBorders(box);

    // FIXME: Implement writing-mode translation
    auto borders = LogicalInsets{
        .inlineStart = physicalBorders.start,
        .inlineEnd = physicalBorders.end,
        .blockStart = physicalBorders.top,
        .blockEnd = physicalBorders.bottom,
    };

    LogicalSize<Opt<Au>> size = {NONE, NONE};

    // FIXME: Intrinsic sizing.
    if (auto [calc] = box.style->sizing->width.is<Style::PxPercent>()) {
        if (auto const [width] = Layout::resolve(calc, NONE)) {
            if (box.style->writingMode == Keywords::HORIZONTAL_TB) {
                size.inline_ = Some(width);
            } else {
                size.block = Some(width);
            }
            if (box.style->writingMode == Keywords::HORIZONTAL_TB) {
                if (box.style->boxSizing == Keywords::CONTENT_BOX) {
                    size.inline_ = Some(width + paddings.inlineSum() + borders.inlineSum());
                } else {
                    size.inline_ = Some(max(width, paddings.inlineSum() + borders.inlineSum()));
                }
            } else {
                if (box.style->boxSizing == Keywords::CONTENT_BOX) {
                    size.block = Some(width + paddings.blockSum() + borders.blockSum());
                } else {
                    size.block = Some(max(width, paddings.blockSum() + borders.blockSum()));
                }
            }
        }
    }

    // FIXME: Intrinsic sizing.
    if (auto [calc] = box.style->sizing->height.is<Style::PxPercent>()) {
        if (auto const [height] = Layout::resolve(calc, NONE)) {
            if (box.style->writingMode == Keywords::HORIZONTAL_TB) {
                if (box.style->boxSizing == Keywords::CONTENT_BOX) {
                    size.block = Some(height + paddings.blockSum() + borders.blockSum());
                } else {
                    size.block = Some(max(height, paddings.blockSum() + borders.blockSum()));
                }
            } else {
                if (box.style->boxSizing == Keywords::CONTENT_BOX) {
                    size.inline_ = Some(height + paddings.inlineSum() + borders.inlineSum());
                } else {
                    size.inline_ = Some(max(height, paddings.inlineSum() + borders.inlineSum()));
                }
            }
        }
    }

    return Metrics{
        .borders = {}, //< FIXME
        .paddings = paddings,
        .size = size,
    };
}


Output layout(Layout::Box& box, Constraints const& constraints, Opt<BreakOpportunity const&> breakOpportunity, Opt<BreakNode const&> breakTree) {
    auto input = Input{
        .constraints = constraints,
        .metrics = computeMetrics(box, constraints.containingBlock),
        .breakOpportunity = breakOpportunity,
        .breakTree = breakTree,
    };

    if (box.style->display == Style::Display::BLOCK) {
        return BlockFormattingContext{}.run(box, input);
    } else {
        logWarn("falling back to bfc");
        return BlockFormattingContext{}.run(box, input);
    }
}

} // namespace Vaev::Layout2
