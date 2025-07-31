module;

#include <karm-gfx/borders.h>
#include <karm-logger/logger.h>
#include <karm-text/prose.h>

module Vaev.Engine;

import Karm.Image;
import :values;
import :layout.block;
import :layout.flex;
import :layout.grid;
import :layout.inline_;
import :layout.replaced;
import :layout.positioned;
import :layout.table;
import :layout.values;

namespace Vaev::Layout {

static Opt<Rc<FormatingContext>> _constructFormatingContext(Box& box) {
    auto display = box.style->display;

    if (box.isReplaced()) {
        return constructReplacedFormatingContext(box);
    } else if (box.content.is<InlineBox>()) {
        return constructInlineFormatingContext(box);
    } else if (
        display == Display::FLOW or
        display == Display::FLOW_ROOT or
        display == Display::TABLE_CELL or
        display == Display::TABLE_CAPTION or
        display == Display::TABLE
    ) {
        return constructBlockFormatingContext(box);
    } else if (display == Display::FLEX) {
        return constructFlexFormatingContext(box);
    } else if (display == Display::GRID) {
        return constructGridFormatingContext(box);
    } else if (display == Display::TABLE_BOX) {
        return constructTableFormatingContext(box);
    } else if (display == Display::INTERNAL) {
        return NONE;
    } else {
        return constructBlockFormatingContext(box);
    }
}

Output _contentLayout(Tree& tree, Box& box, Input input, usize startAt, Opt<usize> stopAt) {
    if (box.formatingContext == NONE) {
        box.formatingContext = _constructFormatingContext(box);
        if (box.formatingContext)
            box.formatingContext.unwrap()->build(tree, box);
    }
    if (not box.formatingContext)
        return Output{};
    return box.formatingContext.unwrap()->run(tree, box, input, startAt, stopAt);
}

InsetsAu computeMargins(Tree& tree, Box& box, Input input) {
    InsetsAu res;
    auto margin = box.style->margin;

    res.top = resolve(tree, box, margin->top, input.containingBlock.height);
    res.end = resolve(tree, box, margin->end, input.containingBlock.width);
    res.bottom = resolve(tree, box, margin->bottom, input.containingBlock.height);
    res.start = resolve(tree, box, margin->start, input.containingBlock.width);

    return res;
}

InsetsAu computeBorders(Tree& tree, Box& box) {
    if (box.style->display == Display::TABLE_BOX and box.style->table->collapse == BorderCollapse::COLLAPSE) {
        return InsetsAu{};
    }

    InsetsAu res;
    auto borders = box.style->borders;

    if (borders->top.style != Gfx::BorderStyle::NONE)
        res.top = resolve(tree, box, borders->top.width);

    if (borders->end.style != Gfx::BorderStyle::NONE)
        res.end = resolve(tree, box, borders->end.width);

    if (borders->bottom.style != Gfx::BorderStyle::NONE)
        res.bottom = resolve(tree, box, borders->bottom.width);

    if (borders->start.style != Gfx::BorderStyle::NONE)
        res.start = resolve(tree, box, borders->start.width);

    return res;
}

InsetsAu computePaddings(Tree& tree, Box& box, Vec2Au containingBlock) {
    InsetsAu res;
    auto padding = box.style->padding;

    res.top = resolve(tree, box, padding->top, containingBlock.height);
    res.end = resolve(tree, box, padding->end, containingBlock.width);
    res.bottom = resolve(tree, box, padding->bottom, containingBlock.height);
    res.start = resolve(tree, box, padding->start, containingBlock.width);

    return res;
}

Math::Radii<Au> computeRadii(Tree& tree, Box& box, Vec2Au size) {
    auto radii = box.style->borders->radii;
    Math::Radii<Au> res;

    res.a = resolve(tree, box, radii.a, size.height);
    res.b = resolve(tree, box, radii.b, size.width);
    res.c = resolve(tree, box, radii.c, size.width);
    res.d = resolve(tree, box, radii.d, size.height);
    res.e = resolve(tree, box, radii.e, size.height);
    res.f = resolve(tree, box, radii.f, size.width);
    res.g = resolve(tree, box, radii.g, size.width);
    res.h = resolve(tree, box, radii.h, size.height);

    return res;
}

Vec2Au computeIntrinsicSize(Tree& tree, Box& box, IntrinsicSize intrinsic, Vec2Au containingBlock) {
    if (intrinsic == IntrinsicSize::AUTO) {
        panic("bad argument");
    }

    auto borders = computeBorders(tree, box);
    auto padding = computePaddings(tree, box, containingBlock);

    auto output = _contentLayout(
        tree,
        box,
        {
            .intrinsic = intrinsic,
            .knownSize = {NONE, NONE},
        },
        0, NONE
    );

    return output.size + padding.all() + borders.all();
}

Opt<Au> computeSpecifiedSize(Tree& tree, Box& box, Size size, Vec2Au containingBlock, bool isWidth) {
    if (size.is<Keywords::MinContent>()) {
        auto intrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MIN_CONTENT, containingBlock);
        return isWidth ? Opt<Au>{intrinsicSize.x} : Opt<Au>{intrinsicSize.y};
    } else if (size.is<Keywords::MaxContent>()) {
        auto intrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MAX_CONTENT, containingBlock);
        return isWidth ? Opt<Au>{intrinsicSize.x} : Opt<Au>{intrinsicSize.y};
    } else if (size.is<FitContent>()) {
        auto minIntrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MIN_CONTENT, containingBlock);
        auto maxIntrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MAX_CONTENT, containingBlock);
        auto stretchIntrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::STRETCH_TO_FIT, containingBlock);

        if (isWidth)
            return clamp(stretchIntrinsicSize.x, minIntrinsicSize.x, maxIntrinsicSize.x);
        else
            return clamp(stretchIntrinsicSize.y, minIntrinsicSize.y, maxIntrinsicSize.y);
    } else if (size.is<Keywords::Auto>()) {
        return NONE;
    } else if (auto calc = size.is<CalcValue<PercentOr<Length>>>()) {
        return resolve(tree, box, *calc, isWidth ? containingBlock.x : containingBlock.y);
    } else {
        logWarn("unknown specified size: {}", size);
        return 0_au;
    }
}

static Res<None, Output> _shouldAbortFragmentingBeforeLayout(Fragmentainer& fc, Input input) {
    if (not fc.acceptsFit(
            input.position.y,
            0_au,
            input.pendingVerticalSizes
        ))
        return Output{
            .size = Vec2Au{0_au, 0_au},
            .completelyLaidOut = false,
            .breakpoint = Breakpoint::overflow()
        };

    return Ok(NONE);
}

static void _maybeSetMonolithicBreakpoint(Fragmentainer& fc, bool isMonolticDisplay, bool childCompletelyLaidOut, usize boxChildrenLen, Opt<Breakpoint>& outputBreakpoint) {
    if (not fc.isMonolithicBox() or
        not isMonolticDisplay or
        fc.hasInfiniteDimensions())
        return;

    if (not childCompletelyLaidOut)
        panic("monolitic blocks should always be completly laid out");

    // NOTE: wont abstract this since this is currently a workaround since we dont have fragmentation for table,flex
    Breakpoint bottomOfContentBreakForTopMonolitic{
        .endIdx = boxChildrenLen,
        .appeal = Breakpoint::Appeal::CLASS_B,
        .advance = Breakpoint::Advance::WITHOUT_CHILDREN
    };

    outputBreakpoint = bottomOfContentBreakForTopMonolitic;
}

Output layoutContentBox(Tree& tree, Box& box, Input input) {
    bool isMonolithicDisplay =
        box.style->display == Display::Inside::FLEX or
        box.style->display == Display::Inside::GRID;

    usize startAt = tree.fc.allowBreak() ? input.breakpointTraverser.getStart().unwrapOr(0) : 0;

    if (tree.fc.isDiscoveryMode()) {
        try$(_shouldAbortFragmentingBeforeLayout(tree.fc, input));

        if (isMonolithicDisplay)
            tree.fc.enterMonolithicBox();

        // TODO: Class C breakpoint

        auto out = _contentLayout(tree, box, input, startAt, NONE);

        // NOTE: assert since algo is still a bit experimental
        if (not out.completelyLaidOut and out.breakpoint == NONE)
            panic("if it was not completely laid out, there should be a breakpoint");

        _maybeSetMonolithicBreakpoint(
            tree.fc,
            isMonolithicDisplay,
            out.completelyLaidOut,
            box.children().len(),
            out.breakpoint
        );

        if (isMonolithicDisplay)
            tree.fc.leaveMonolithicBox();

        return {
            .size = out.size,
            .completelyLaidOut = out.completelyLaidOut,
            .breakpoint = out.breakpoint,
            .firstBaselineSet = out.firstBaselineSet,
            .lastBaselineSet = out.lastBaselineSet,
        };
    } else {
        Opt<usize> stopAt = tree.fc.allowBreak()
                                ? input.breakpointTraverser.getEnd()
                                : NONE;

        auto parentFrag = input.fragment;
        Frag currFrag(&box);
        input.fragment = input.fragment ? &currFrag : nullptr;

        if (isMonolithicDisplay)
            tree.fc.enterMonolithicBox();

        auto out = _contentLayout(tree, box, input, startAt, stopAt);

        if (isMonolithicDisplay)
            tree.fc.leaveMonolithicBox();

        if (parentFrag) {
            currFrag.metrics.outlineOffset = resolve(tree, box, box.style->outline->offset);
            currFrag.metrics.outlineWidth = resolve(tree, box, box.style->outline->width);

            parentFrag->add(std::move(currFrag));
        }

        return {
            .size = out.size,
            .completelyLaidOut = out.completelyLaidOut,
            .firstBaselineSet = out.firstBaselineSet,
            .lastBaselineSet = out.lastBaselineSet,
        };
    }
}

Output layout(Tree& tree, Input input) {
    auto borders = computeBorders(tree, tree.root);
    auto padding = computePaddings(tree, tree.root, input.containingBlock);
    
    if(auto specifiedWidth = computeSpecifiedSize(
        tree, tree.root, tree.root.style->sizing->width, input.containingBlock, true
    )){
        input.knownSize.width = specifiedWidth.unwrap() - borders.horizontal() - padding.horizontal();
    }

    if(auto specifiedHeight = computeSpecifiedSize(
        tree, tree.root, tree.root.style->sizing->width, input.containingBlock, true
    )){
        input.knownSize.height = specifiedHeight.unwrap() - borders.vertical() - padding.vertical();
    }

    
    auto output = layoutContentBox(tree, tree.root, input);
    if (input.fragment) {

        auto& child = input.fragment->children()[0];
        child.metrics = Metrics::commitContentBox(
            tree, tree.root,
            output.size, input.position,
            borders, padding
        );

        layoutPositioned(tree, input.fragment->children()[0], input.containingBlock);
    }
    return output;
}

Tuple<Output, Frag> layoutCreateFragment(Tree& tree, Input input) {
    auto root = Layout::Frag();
    input.fragment = &root;
    auto out = layout(tree, input);
    return {out, std::move(root.children()[0])};
}

Metrics Metrics::commitContentBox(Tree& tree, Box& box, Vec2Au contentBoxSize, Vec2Au contentBoxPosition, InsetsAu borders, InsetsAu padding) {
    return {
        .padding = padding,
        .borders = borders,
        .outlineOffset = resolve(tree, box, box.style->outline->offset),
        .outlineWidth = resolve(tree, box, box.style->outline->width),
        .position = contentBoxPosition - borders.topStart() - padding.topStart(),
        .borderSize = contentBoxSize + borders.all() + padding.all(),
        .margin = {},
        .radii = computeRadii(tree, box, contentBoxSize + borders.all() + padding.all()),
    };
}

} // namespace Vaev::Layout
