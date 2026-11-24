export module Vaev.Engine:layout2.box;

import :layout2.sizing;
import :layout.values;

namespace Vaev::Layout2 {

export Opt<Au> _resolveMargin(Layout::Tree const& tree, Layout::Box const& box, auto const& margin, Au relative) {
    return margin.visit(
        [](Keywords::Auto) -> Opt<Au> {
            return NONE;
        },
        [&](Calc<PercentOr<Length>> const& calc) -> Opt<Au> {
            return Some(Layout::resolve(tree, box, calc, relative));
        }
    );
}

// FIXME: The writing mode translation is wrong, and should take into account direction too.

Pair<Opt<Au>> resolveInlineMargins(Layout::Tree const& tree, Layout::Box const& box, LogicalSize<Opt<Au>> const& containingBlock) {
    auto const& style = *box.style;
    if (style.writingMode == WritingMode::HORIZONTAL_TB) {
        return {
            _resolveMargin(tree, box, style.margin->start, containingBlock.inline_.unwrapOr(0_au)),
            _resolveMargin(tree, box, style.margin->end, containingBlock.inline_.unwrapOr(0_au)),
        };
    } else {
        return {
            _resolveMargin(tree, box, style.margin->top, containingBlock.inline_.unwrapOr(0_au)),
            _resolveMargin(tree, box, style.margin->bottom, containingBlock.inline_.unwrapOr(0_au)),
        };
    }
}

Pair<Opt<Au>> resolveBlockMargins(Layout::Tree const& tree, Layout::Box const& box, LogicalSize<Opt<Au>> const& containingBlock) {
    auto const& style = *box.style;
    if (style.writingMode == WritingMode::HORIZONTAL_TB) {
        return {
            _resolveMargin(tree, box, style.margin->top, containingBlock.inline_.unwrapOr(0_au)),
            _resolveMargin(tree, box, style.margin->bottom, containingBlock.inline_.unwrapOr(0_au)),
        };
    } else {
        return {
            _resolveMargin(tree, box, style.margin->start, containingBlock.inline_.unwrapOr(0_au)),
            _resolveMargin(tree, box, style.margin->end, containingBlock.inline_.unwrapOr(0_au)),
        };
    }
}

LogicalInsets<Opt<Au>> resolveMargins(Layout::Tree const& tree, Layout::Box const& box, LogicalSize<Opt<Au>> const& containingBlock) {
    auto [inlineStart, inlineEnd] = resolveInlineMargins(tree, box, containingBlock);
    auto [blockStart, blockEnd] = resolveBlockMargins(tree, box, containingBlock);
    return {inlineStart, inlineEnd, blockStart, blockEnd};
}

} // namespace Vaev::Layout2
