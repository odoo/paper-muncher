module;

#include <vaev-style/specified.h>
#include <vaev-values/insets.h>
#include <karm-math/au.h>

export module Vaev.Engine:layout.positioned;

import :values;
import :layout.layout;
import :layout.values;

namespace Vaev::Layout {

Vec2Au _resolveOrigin(Vec2Au fragPosition, Vec2Au containingBlockOrigin, Position position) {
    if (position == Keywords::RELATIVE) {
        return fragPosition;
    } else if (position == Keywords::ABSOLUTE) {
        return containingBlockOrigin;
    } else {
        return {0_au, 0_au};
    }
}

static void registerRunningPosition(Input& input, Style::SpecifiedValues const& style, Frag& frag) {

    if (not input.runningPosition) {
        return;
    }
    auto& running = input.runningPosition.peek();

    if (auto pos = style.position.is<RunningPosition>()) {
        auto position = pos.peek();

        auto copy = frag.box;
        copy.peek().style->position = Keywords::STATIC;
        yap("changing position to static {}", copy.peek().style->position);
        RunningPositionInfo info = {input.page.number, position, copy};
        yap("running found {}", position);
        if (running.has(position.customIdent)) {
            yap("running already exists");
            running.take(position.customIdent).pushBack(info);
        } else {
            yap("running doesnt exists yet");
            running.put(position.customIdent, {info});
        }
        yap("running position is now {}", running);
        // TODO get every running pos before first page layout
        // TODO match correctly the running pos
    }
}

export void layoutPositioned(Tree& tree, Frag& frag, RectAu containingBlock, Input input) {

    auto& style = frag.style();
    registerRunningPosition(input, style, frag);

    auto& metrics = frag.metrics;

    if (impliesRemovingFromFlow(style.position) or style.position == Keywords::RELATIVE) {
        auto origin = _resolveOrigin(metrics.position, containingBlock.topStart(), style.position);
        auto relativeTo = style.position == Keywords::FIXED
                              ? tree.viewport.small
                              : containingBlock;

        auto top = metrics.position.y;
        auto start = metrics.position.x;

        auto topOffset = style.offsets->top;
        if (auto topOffsetCalc = topOffset.is<CalcValue<PercentOr<Length>>>()) {
            top = origin.y + resolve(tree, *frag.box, *topOffsetCalc, relativeTo.height);
        }

        auto startOffset = style.offsets->start;
        if (auto startOffsetCalc = startOffset.is<CalcValue<PercentOr<Length>>>()) {
            start = origin.x + resolve(tree, *frag.box, *startOffsetCalc, relativeTo.width);
        }

        auto endOffset = frag.style().offsets->end;
        if (auto endOffsetCalc = endOffset.is<CalcValue<PercentOr<Length>>>()) {
            start = (origin.x + relativeTo.width) - resolve(tree, *frag.box, *endOffsetCalc, relativeTo.width) - metrics.borderSize.width;
        }

        Vec2Au newPositionOffset = Vec2Au{start, top} - metrics.position;
        frag.offset(newPositionOffset);

        containingBlock = metrics.contentBox();
    }

    for (auto& c : frag.children()) {
        layoutPositioned(tree, c, containingBlock, input);
    }
}

} // namespace Vaev::Layout
