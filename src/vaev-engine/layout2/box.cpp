export module Vaev.Engine:layout2.box;

import :layout2.sizing;
import :layout.values;

using Au = Vaev::Layout::Au;

namespace Vaev::Layout2 {

export Opt<Au> _resolveMargin(auto const& margin, Au relative) {
    return margin.visit(
        [](Keywords::Auto) -> Opt<Au> {
            return NONE;
        },
        [&](Style::PxPercent const& calc) -> Opt<Au> {
            return Some(Layout::resolve(calc, relative));
        }
    );
}

// FIXME: The writing mode translation is wrong, and should take into account direction too.

Pair<Opt<Au>> resolveInlineMargins(Style::ComputedValues const& style, LogicalSize<Opt<Au>> const& containingBlock) {
    if (style.writingMode == Keywords::HORIZONTAL_TB) {
        return {
            _resolveMargin(style.margin->start, containingBlock.inline_.unwrapOr(0_au)),
            _resolveMargin(style.margin->end, containingBlock.inline_.unwrapOr(0_au)),
        };
    } else {
        return {
            _resolveMargin(style.margin->top, containingBlock.inline_.unwrapOr(0_au)),
            _resolveMargin(style.margin->bottom, containingBlock.inline_.unwrapOr(0_au)),
        };
    }
}

Pair<Opt<Au>> resolveBlockMargins(Style::ComputedValues const& style, LogicalSize<Opt<Au>> const& containingBlock) {
    if (style.writingMode == Keywords::HORIZONTAL_TB) {
        return {
            _resolveMargin(style.margin->top, containingBlock.inline_.unwrapOr(0_au)),
            _resolveMargin(style.margin->bottom, containingBlock.inline_.unwrapOr(0_au)),
        };
    } else {
        return {
            _resolveMargin(style.margin->start, containingBlock.inline_.unwrapOr(0_au)),
            _resolveMargin(style.margin->end, containingBlock.inline_.unwrapOr(0_au)),
        };
    }
}

LogicalInsets<Opt<Au>> resolveMargins(Style::ComputedValues const& style, LogicalSize<Opt<Au>> const& containingBlock) {
    auto [inlineStart, inlineEnd] = resolveInlineMargins(style, containingBlock);
    auto [blockStart, blockEnd] = resolveBlockMargins(style, containingBlock);
    return {inlineStart, inlineEnd, blockStart, blockEnd};
}

} // namespace Vaev::Layout2
