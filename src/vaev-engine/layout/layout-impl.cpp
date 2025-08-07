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
    // NOTE: In borders collapse mode, we assume that the table box borders are 'transfered' to the cells
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

    res.top = resolve(tree, box, padding->top, containingBlock.width);
    res.end = resolve(tree, box, padding->end, containingBlock.width);
    res.bottom = resolve(tree, box, padding->bottom, containingBlock.width);
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

Opt<Au> computeSpecifiedWidth(Tree& tree, Box& box, Size size, Vec2Au containingBlock) {
    if (size.is<Keywords::MinContent>()) {
        auto intrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MIN_CONTENT, containingBlock);
        return intrinsicSize.x;
    } else if (size.is<Keywords::MaxContent>()) {
        auto intrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MAX_CONTENT, containingBlock);
        return intrinsicSize.x;
    } else if (size.is<FitContent>()) {
        auto minIntrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MIN_CONTENT, containingBlock);
        auto maxIntrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::MAX_CONTENT, containingBlock);
        auto stretchIntrinsicSize = computeIntrinsicSize(tree, box, IntrinsicSize::STRETCH_TO_FIT, containingBlock);

        return clamp(stretchIntrinsicSize.x, minIntrinsicSize.x, maxIntrinsicSize.x);
    } else if (size.is<Keywords::Auto>()) {
        return NONE;
    } else if (auto calc = size.is<CalcValue<PercentOr<Length>>>()) {
        return resolve(tree, box, *calc, containingBlock.x);
    } else {
        logWarn("unknown specified size: {}", size);
        return 0_au;
    }
}

Opt<Au> computeSpecifiedHeight(Tree& tree, Box& box, Size size, Vec2Au containingBlock) {
    if (size.is<Keywords::MinContent>()) {
        // https://drafts.csswg.org/css-sizing-3/#valdef-width-min-content
        // for a box’s block size, unless otherwise specified, this is equivalent to its automatic size.
        return NONE;
    } else if (size.is<Keywords::MaxContent>()) {
        // https://drafts.csswg.org/css-sizing-3/#valdef-width-max-content
        // for a box’s block size, unless otherwise specified, this is equivalent to its automatic size.
        return NONE;
    } else if (size.is<FitContent>()) {
        // Since this depends on min/max content size, this is also unknown.
        return NONE;
    } else if (size.is<Keywords::Auto>()) {
        return NONE;
    } else if (auto calc = size.is<CalcValue<PercentOr<Length>>>()) {
        return resolve(tree, box, *calc, containingBlock.y);
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

        if (isMonolithicDisplay)
            tree.fc.enterMonolithicBox();

        auto out = _contentLayout(tree, box, input, startAt, stopAt);

        if (isMonolithicDisplay)
            tree.fc.leaveMonolithicBox();

        return {
            .size = out.size,
            .completelyLaidOut = out.completelyLaidOut,
            .firstBaselineSet = out.firstBaselineSet,
            .lastBaselineSet = out.lastBaselineSet,
        };
    }
}

Input _adaptToContentBox(Input input, UsedSpacings const& usedSpacings) {
    auto borders = usedSpacings.borders;
    auto padding = usedSpacings.padding;

    input.knownSize.x = input.knownSize.x.map(
        [&](Au size) {
            return size - borders.horizontal() - padding.horizontal();
        }
    );

    input.knownSize.y = input.knownSize.y.map(
        [&](Au size) {
            return size - borders.vertical() - padding.vertical();
        }
    );
    input.position = input.position + borders.topStart() + padding.topStart();
    input.pendingVerticalSizes += borders.bottom + padding.bottom;

    return input;
}

Output layoutBorderBox(Tree& tree, Box& box, Input input, UsedSpacings usedSpacings) {
    input = _adaptToContentBox(input, usedSpacings);
    auto output = layoutContentBox(tree, box, input);
    output.size = output.size + usedSpacings.borders.all() + usedSpacings.padding.all();
    return output;
}

Output layoutAndCommitBorderBox(Tree& tree, Box& box, Input input, Frag& parentFrag, UsedSpacings usedSpacings) {
    input = _adaptToContentBox(input, usedSpacings);
    auto output = layoutAndCommitContentBox(tree, box, input, parentFrag, usedSpacings);
    output.size = output.size + usedSpacings.borders.all() + usedSpacings.padding.all();
    return output;
}

Output layoutAndCommitContentBox(Tree& tree, Box& box, Input input, Frag& parentFrag, UsedSpacings usedSpacings) {
    Frag currFrag(&box);

    auto output = layoutContentBox(tree, box, input.withFragment(&currFrag));

    currFrag.metrics = Metrics{
        .padding = usedSpacings.padding,
        .borders = usedSpacings.borders,
        .outlineOffset = resolve(tree, box, box.style->outline->offset),
        .outlineWidth = resolve(tree, box, box.style->outline->width),
        .position = input.position - usedSpacings.borders.topStart() - usedSpacings.padding.topStart(),
        .borderSize = output.size + usedSpacings.borders.all() + usedSpacings.padding.all(),
        .margin = usedSpacings.margin,
        .radii = computeRadii(tree, box, output.size + usedSpacings.borders.all() + usedSpacings.padding.all()),
    };

    parentFrag.add(std::move(currFrag));

    return output;
}

Output layoutRoot(Tree& tree, Input input) {
    if (not input.knownSize.width)
        input.knownSize.width = computeSpecifiedWidth(
            tree, tree.root, tree.root.style->sizing->width, input.containingBlock
        );

    if (not input.knownSize.height)
        input.knownSize.height = computeSpecifiedHeight(
            tree, tree.root, tree.root.style->sizing->height, input.containingBlock
        );

    auto output = layoutBorderBox(
        tree, tree.root, input,
        {
            .padding = computePaddings(tree, tree.root, input.containingBlock),
            .borders = computeBorders(tree, tree.root),
        }
    );

    return output;
}

Tuple<Output, Frag> layoutAndCommitRoot(Tree& tree, Input input) {
    auto parentFragOfRoot = Layout::Frag();

    if (not input.knownSize.width)
        input.knownSize.width = computeSpecifiedWidth(
            tree, tree.root, tree.root.style->sizing->width, input.containingBlock
        );

    if (not input.knownSize.height)
        input.knownSize.height = computeSpecifiedHeight(
            tree, tree.root, tree.root.style->sizing->height, input.containingBlock
        );

    auto out = layoutAndCommitBorderBox(
        tree, tree.root, input, parentFragOfRoot,
        {
            .padding = computePaddings(tree, tree.root, input.containingBlock),
            .borders = computeBorders(tree, tree.root),
        }
    );

    auto fragOfRoot = std::move(parentFragOfRoot.children()[0]);

    layoutPositioned(tree, fragOfRoot, input.containingBlock);

    return {out, std::move(fragOfRoot)};
}

} // namespace Vaev::Layout
