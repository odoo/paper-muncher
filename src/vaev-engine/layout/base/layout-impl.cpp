module;

#include <karm/macros>

module Vaev.Engine;

import Karm.Core;
import Karm.Image;
import Karm.Gfx;
import Karm.Math;
import Karm.Logger;
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

    if (box.isSvg() and not box.isSvgForeignObjectBox()) {
        return constructSvgFormatingContext(box);
    } else if (box.isReplaced()) {
        return constructReplacedFormatingContext(box);
    } else if (box.content.is<Rc<Gfx::Prose>>()) {
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

Output _dispatchFormatingContext(Tree& tree, Box& box, Input input, usize startAt, Opt<usize> stopAt) {
    if (box.formatingContext == NONE) {
        box.formatingContext = _constructFormatingContext(box);
        if (box.formatingContext)
            box.formatingContext.unwrap()->build(tree, box);
    }
    if (auto& [formatingContext] = box.formatingContext)
        return formatingContext->run(tree, box, input, startAt, stopAt);
    return Output{};
}

InsetsAu computeMargins(Tree& tree, Box& box, Vec2Au containingBlock) {
    // Boxes that make up a table do not have margins.
    if (box.style->display.isTableInternal())
        return {};

    InsetsAu res;
    auto margin = box.style->margin;

    res.top = resolve(tree, box, margin->top, containingBlock.height);
    res.end = resolve(tree, box, margin->end, containingBlock.width);
    res.bottom = resolve(tree, box, margin->bottom, containingBlock.height);
    res.start = resolve(tree, box, margin->start, containingBlock.width);

    return res;
}

// https://www.w3.org/TR/css-values-4/#snap-a-length-as-a-border-width
Au _snapLengthAsBorderWidth(Au v) {
    if (v < 1_au)
        return ceil(v);
    return floor(v);
}

InsetsAu computeBorders(Tree& tree, Box& box) {
    // NOTE: In borders collapse mode, we assume that the table box borders are 'transfered' to the cells
    if (box.style->display == Display::TABLE_BOX and box.style->table->collapse == BorderCollapse::COLLAPSE) {
        return InsetsAu{};
    }

    InsetsAu res;
    auto borders = box.style->borders;

    if (borders->top.style != Gfx::BorderStyle::NONE)
        res.top = _snapLengthAsBorderWidth(resolve(tree, box, borders->top.width));

    if (borders->end.style != Gfx::BorderStyle::NONE)
        res.end = _snapLengthAsBorderWidth(resolve(tree, box, borders->end.width));

    if (borders->bottom.style != Gfx::BorderStyle::NONE)
        res.bottom = _snapLengthAsBorderWidth(resolve(tree, box, borders->bottom.width));

    if (borders->start.style != Gfx::BorderStyle::NONE)
        res.start = _snapLengthAsBorderWidth(resolve(tree, box, borders->start.width));

    return res;
}

InsetsAu computePaddings(Tree& tree, Box& box, Vec2Au containingBlock) {
    // In a table only table cell have padding
    if (box.style->display.isTableInternal() and box.style->display != Display::TABLE_CELL)
        return {};

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

Vec2Au computeIntrinsicContentSize(Tree& tree, Box& box, IntrinsicSize intrinsic, Opt<Au> capmin) {
    if (intrinsic == IntrinsicSize::AUTO) {
        panic("bad argument");
    }

    auto output = _dispatchFormatingContext(
        tree,
        box,
        {
            .intrinsic = intrinsic,
            .knownSize = {NONE, NONE},
            .capmin = capmin,
        },
        0,
        NONE
    );

    return output.size;
}

Opt<Au> computeSpecifiedBorderBoxWidth(Tree& tree, Box& box, Size size, Vec2Au containingBlock, Au horizontalBorderBox, Opt<Au> capmin) {
    if (auto calc = size.is<CalcValue<PercentOr<Length>>>()) {
        auto specifiedWidth = resolve(tree, box, *calc, containingBlock.x);
        if (box.style->boxSizing == BoxSizing::CONTENT_BOX) {
            specifiedWidth += horizontalBorderBox;
        }
        return specifiedWidth;
    }

    if (size.is<Keywords::MinContent>()) {
        auto intrinsicSize = computeIntrinsicContentSize(tree, box, IntrinsicSize::MIN_CONTENT, capmin);
        return intrinsicSize.x + horizontalBorderBox;
    } else if (size.is<Keywords::MaxContent>()) {
        auto intrinsicSize = computeIntrinsicContentSize(tree, box, IntrinsicSize::MAX_CONTENT, capmin);
        return intrinsicSize.x + horizontalBorderBox;
    } else if (size.is<FitContent>()) {
        auto minIntrinsicSize = computeIntrinsicContentSize(tree, box, IntrinsicSize::MIN_CONTENT, capmin);
        auto maxIntrinsicSize = computeIntrinsicContentSize(tree, box, IntrinsicSize::MAX_CONTENT, capmin);
        auto stretchIntrinsicSize = computeIntrinsicContentSize(tree, box, IntrinsicSize::STRETCH_TO_FIT, capmin);

        return clamp(stretchIntrinsicSize.x, minIntrinsicSize.x, maxIntrinsicSize.x) + horizontalBorderBox;
    } else if (size.is<Keywords::Auto>()) {
        return NONE;
    } else {
        logWarn("unknown specified size: {}", size);
        return 0_au;
    }
}

Opt<Au> computeSpecifiedBorderBoxHeight(Tree& tree, Box& box, Size size, Vec2Au containingBlock, Au verticalBorderBox) {
    if (auto calc = size.is<CalcValue<PercentOr<Length>>>()) {
        auto specifiedHeight = resolve(tree, box, *calc, containingBlock.y);
        if (box.style->boxSizing == BoxSizing::CONTENT_BOX) {
            specifiedHeight += verticalBorderBox;
        }
        return specifiedHeight;
    }

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
    } else {
        logWarn("unknown specified size: {}", size);
        return 0_au;
    }
}

BoxMetrics computeBoxMetrics(Tree& tree, Box& box, Vec2Au position, Vec2Au size, UsedSpacings const& usedSpacings) {
    return BoxMetrics{
        .padding = usedSpacings.padding,
        .borders = usedSpacings.borders,
        .outlineOffset = resolve(tree, box, box.style->outline->offset),
        .outlineWidth = resolve(tree, box, box.style->outline->width),
        .position = position - usedSpacings.borders.topStart() - usedSpacings.padding.topStart(),
        .borderSize = size + usedSpacings.borders.all() + usedSpacings.padding.all(),
        .margin = usedSpacings.margin,
        .radii = computeRadii(tree, box, size + usedSpacings.borders.all() + usedSpacings.padding.all()),
    };
}

Opt<Rc<Fragment>> createBoxFragmentIfRequested(Tree& tree, Box& box, Input input, Vec2Au size, Vec<Rc<Fragment>> children) {
    if (input.generateFragment) {
        auto boxMetrics = computeBoxMetrics(tree, box, input.position, size, input.usedSpacings);
        return makeRc<BoxFragment>(box,  boxMetrics, std::move(children));
    }

    return NONE;
}

static Res<None, Output> _shouldAbortFragmentingBeforeLayout(Fragmentainer& fc, Input input) {
    if (not fc.isDiscoveryMode())
        return Ok(NONE);

    if (not fc.acceptsFit(
            input.position.y,
            0_au,
            input.pendingVerticalSizes
        ))
        return Output{
            .fragment = NONE,
            .size = Vec2Au{0_au, 0_au},
            .completelyLaidOut = false,
            .breakpoint = Breakpoint::overflow()
        };

    return Ok(NONE);
}

Output layoutContentBox(Tree& tree, Box& box, Input input) {
    auto startAt = tree.fc.allowBreak() ? input.breakpointTraverser.getStart().unwrapOr(0uz) : 0uz;
    auto stopAt = tree.fc.allowBreak() and not tree.fc.isDiscoveryMode() ? input.breakpointTraverser.getEnd() : NONE;
    try$(_shouldAbortFragmentingBeforeLayout(tree.fc, input));
    return _dispatchFormatingContext(
        tree, box, input, startAt, stopAt
    );
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

Output layoutBorderBox(Tree& tree, Box& box, Input input) {
    input = _adaptToContentBox(input, input.usedSpacings);
    auto output = layoutContentBox(tree, box, input);
    output.size = output.size + input.usedSpacings.borders.all() + input.usedSpacings.padding.all();
    return output;
}

Output layoutRoot(Tree& tree, Input input) {
    UsedSpacings usedSpacings{
        .padding = computePaddings(tree, tree.root, input.containingBlock),
        .borders = computeBorders(tree, tree.root),
    };

    input.usedSpacings = usedSpacings;

    if (not input.knownSize.width)
        input.knownSize.width = computeSpecifiedBorderBoxWidth(
            tree, tree.root, tree.root.style->sizing->width, input.containingBlock,
            usedSpacings.borders.horizontal() + usedSpacings.padding.horizontal()
        );

    if (not input.knownSize.height)
        input.knownSize.height = computeSpecifiedBorderBoxHeight(
            tree, tree.root, tree.root.style->sizing->height, input.containingBlock,
            usedSpacings.borders.vertical() + usedSpacings.padding.vertical()
        );

    auto out = layoutBorderBox(tree, tree.root, input);

    // FIXME: Totally breaks the immutable fragment model, needs to go elsewhere.
    //        My day is ruined and my disappointment is immeasurable.
    //        - lufio
    if (input.generateFragment)
        layoutPositioned(tree, *out.fragment, input.containingBlock, input);

    return out;
}

} // namespace Vaev::Layout
