module Vaev.Engine;

import :layout.box;

import :layout2.block;
import :layout.values;

namespace Vaev::Layout2 {

// FIXME: This should be removed after the calc refactor because it'll be unnecessary.
static bool _calcContainsPercents(Calc<PercentOr<Length>> const& calc) {
    auto resolveUnion = Visitor{
        [&](PercentOr<Length> const& v) -> bool {
            return static_cast<bool>(v.is<Percent>());
        },
        [&](Calc<PercentOr<Length>>::Leaf const& v) -> bool {
            return _calcContainsPercents(*v);
        },
        [](Number const&) -> bool {
            return false;
        }
    };

    return calc.visit(
        [&](Calc<PercentOr<Length>>::Value const& v) -> bool {
            return v.visit(resolveUnion);
        },
        [&](Calc<PercentOr<Length>>::Unary const& u) -> bool {
            return u.val.visit(resolveUnion);
        },
        [&](Calc<PercentOr<Length>>::Binary const& b) -> bool {
            return b.lhs.visit(resolveUnion) or b.rhs.visit(resolveUnion);
        }
    );
}

Metrics computeMetrics(Layout::Tree& tree, Layout::Box& box, LogicalSize<Opt<Au>> const& containingBlock) {
    auto paddingLeft = Layout::resolve(
        tree, box,
        box.style->padding->start,
        containingBlock.inline_.unwrapOr(0_au)
    );

    auto paddingRight = Layout::resolve(
        tree, box,
        box.style->padding->end,
        containingBlock.inline_.unwrapOr(0_au)
    );

    auto paddingTop = Layout::resolve(
        tree, box,
        box.style->padding->top,
        containingBlock.inline_.unwrapOr(0_au)
    );

    auto paddingBottom = Layout::resolve(
        tree, box,
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

    // FIXME
    auto borders = LogicalInsets<Au>{};

    LogicalSize<Opt<Au>> size = {NONE, NONE};

    // FIXME: Intrinsic sizing.
    if (auto [calc] = box.style->sizing->width.is<Calc<PercentOr<Length>>>()) {
        if (not _calcContainsPercents(calc)) {
            auto width = resolve(tree, box, calc, containingBlock.inline_.unwrapOr(0_au));
            if (box.style->writingMode == WritingMode::HORIZONTAL_TB) {
                size.inline_ = Some(width);
            } else {
                size.block = Some(width);
            }
            if (box.style->writingMode == WritingMode::HORIZONTAL_TB) {
                if (box.style->boxSizing == BoxSizing::CONTENT_BOX) {
                    size.inline_ = Some(width + paddings.inlineSum() + borders.inlineSum());
                } else {
                    size.inline_ = Some(max(width, paddings.inlineSum() + borders.inlineSum()));
                }
            } else {
                if (box.style->boxSizing == BoxSizing::CONTENT_BOX) {
                    size.block = Some(width + paddings.blockSum() + borders.blockSum());
                } else {
                    size.block = Some(max(width, paddings.blockSum() + borders.blockSum()));
                }
            }
        }
    }

    // FIXME: Intrinsic sizing.
    if (auto [calc] = box.style->sizing->height.is<Calc<PercentOr<Length>>>()) {
        if (not _calcContainsPercents(calc)) {
            auto height = resolve(tree, box, calc, containingBlock.inline_.unwrapOr(0_au));

            if (box.style->writingMode == WritingMode::HORIZONTAL_TB) {
                if (box.style->boxSizing == BoxSizing::CONTENT_BOX) {
                    size.block = Some(height + paddings.blockSum() + borders.blockSum());
                } else {
                    size.block = Some(max(height, paddings.blockSum() + borders.blockSum()));
                }
            } else {
                if (box.style->boxSizing == BoxSizing::CONTENT_BOX) {
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

Output layout(Layout::Tree& tree, Layout::Box& box, Constraints const& constraints) {
    auto input = Input{
        .constraints = constraints,
        .metrics = computeMetrics(tree, box, constraints.containingBlock),
    };

    if (box.style->display == Display::BLOCK) {
        return BlockFormattingContext{}.run(tree, box, input);
    } else {
        logWarn("falling back to bfc");
        return BlockFormattingContext{}.run(tree, box, input);
    }
}

} // namespace Vaev::Layout2
