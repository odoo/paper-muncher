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
        return Some(constructSvgFormatingContext(box));
    } else if (box.isReplaced()) {
        return Some(constructReplacedFormatingContext(box));
    } else if (box.content.is<Rc<Gfx::Prose>>()) {
        return Some(constructInlineFormatingContext(box));
    } else if (
        display == Display::FLOW or
        display == Display::FLOW_ROOT or
        display == Display::TABLE_CELL or
        display == Display::TABLE_CAPTION or
        display == Display::TABLE
    ) {
        return Some(constructBlockFormatingContext(box));
    } else if (display == Display::FLEX) {
        return Some(constructFlexFormatingContext(box));
    } else if (display == Display::GRID) {
        return Some(constructGridFormatingContext(box));
    } else if (display == Display::TABLE_BOX) {
        return Some(constructTableFormatingContext(box));
    } else if (display == Display::INTERNAL) {
        return NONE;
    } else {
        return Some(constructBlockFormatingContext(box));
    }
}

Output _dispatchFormatingContext(Tree& tree, Box& box, Input input, usize startAt, Opt<usize> stopAt) {
    if (box.formatingContext == NONE) {
        box.formatingContext = _constructFormatingContext(box);
        if (box.formatingContext)
            box.formatingContext.expect()->build(tree, box);
    }
    if (auto& [formatingContext] = box.formatingContext)
        return formatingContext->run(tree, box, input, startAt, stopAt);
    return Output{};
}

InsetsAu computeMargins(Tree& tree, Box& box, Math::Vec2<Opt<Au>> containingBlock) {
    // Boxes that make up a table do not have margins.
    if (box.style->display.isTableInternal())
        return {};

    InsetsAu res;
    auto margin = box.style->margin;

    res.top = resolve(tree, box, margin->top, containingBlock.height.unwrapOr(0_au));
    res.end = resolve(tree, box, margin->end, containingBlock.width.unwrapOr(0_au));
    res.bottom = resolve(tree, box, margin->bottom, containingBlock.height.unwrapOr(0_au));
    res.start = resolve(tree, box, margin->start, containingBlock.width.unwrapOr(0_au));

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

InsetsAu computePaddings(Tree& tree, Box& box, Math::Vec2<Opt<Au>> containingBlock) {
    // In a table only table cell have padding
    if (box.style->display.isTableInternal() and box.style->display != Display::TABLE_CELL)
        return {};

    InsetsAu res;
    auto padding = box.style->padding;

    res.top = resolve(tree, box, padding->top, containingBlock.width.unwrapOr(0_au));
    res.end = resolve(tree, box, padding->end, containingBlock.width.unwrapOr(0_au));
    res.bottom = resolve(tree, box, padding->bottom, containingBlock.width.unwrapOr(0_au));
    res.start = resolve(tree, box, padding->start, containingBlock.width.unwrapOr(0_au));

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

// Returns the border-box size of the box's content in the requested axis.
static Au _measureContent(Tree& tree, Box& box, Axis axis, Math::Vec2<Opt<Au>> knownSize, Math::Vec2<Opt<Au>> containingBlock, AvailableSpace availableSpace, Opt<UsedSpacings> overrideSpacings) {
    auto usedSpacings = overrideSpacings.unwrapOrElse([&] {
        return UsedSpacings{
            .padding = computePaddings(tree, box, containingBlock),
            .borders = computeBorders(tree, box),
        };
    });

    auto horizontal = usedSpacings.borders.horizontal() + usedSpacings.padding.horizontal();
    auto vertical = usedSpacings.borders.vertical() + usedSpacings.padding.vertical();

    // NOTE: Measurements are done in content-box terms, like the formatting
    //       contexts expect. This is not done through _adaptToContentBox()
    //       since a measurement has no position nor fragmentation context.
    // FIXME: Definite available space should be adapted too, and inner sizes
    //        should floor at zero.
    //        https://www.w3.org/TR/css-sizing-3/#box-sizing
    auto input = Input{
        .mode = LayoutMode::MEASURE,
        .usedSpacings = usedSpacings,
        .knownSize = {
            knownSize.x.map([&](Au size) {
                return size - horizontal;
            }),
            knownSize.y.map([&](Au size) {
                return size - vertical;
            }),
        },
        .availableSpace = availableSpace,
        .containingBlock = containingBlock,
    };

    // FIXME: The formatting context computes both axes even though only one
    //        is requested.
    auto output = _dispatchFormatingContext(tree, box, input, 0, NONE);

    if (axis == Axis::INLINE)
        return output.size.x + horizontal;
    return output.size.y + vertical;
}

Au measure(Tree& tree, Box& box, Axis requestedAxis, Math::Vec2<Opt<Au>> knownSize, Math::Vec2<Opt<Au>> containingBlock, AvailableSpace availableSpace, [[maybe_unused]] SizingMode sizing, Opt<UsedSpacings> usedSpacings) {
    // FIXME: CONTRIBUTION is treated as SIZE. It should apply the box's own
    //        sizing properties (width, min-width, max-width, box-sizing) and
    //        add padding, border and margins, following
    //        https://www.w3.org/TR/css-sizing-3/#cyclic-percentage-contribution
    return _measureContent(tree, box, requestedAxis, knownSize, containingBlock, availableSpace, usedSpacings);
}

Opt<Au> computeSpecifiedBorderBoxWidth(Tree& tree, Box& box, Size size, Math::Vec2<Opt<Au>> containingBlock, Au horizontalBorderBox) {
    if (auto calc = size.is<Calc<PercentOr<Length>>>()) {
        auto specifiedWidth = resolve(tree, box, *calc, containingBlock.x.unwrapOr(0_au));
        if (box.style->boxSizing == BoxSizing::CONTENT_BOX) {
            specifiedWidth += horizontalBorderBox;
        }
        return Some(specifiedWidth);
    }

    // NOTE: The intrinsic sizes are measured on the content box, and the
    //       caller's edges are added back, matching the known size convention.
    auto measureContent = [&](AvailableSpace availableSpace) {
        return measure(tree, box, Axis::INLINE, {NONE, NONE}, {NONE, NONE}, availableSpace, SizingMode::SIZE, Some(UsedSpacings{})) + horizontalBorderBox;
    };

    if (size.is<Keywords::MinContent>()) {
        return Some(measureContent({MIN_CONTENT, 0_au}));
    } else if (size.is<Keywords::MaxContent>()) {
        return Some(measureContent({MAX_CONTENT, 0_au}));
    } else if (size.is<Keywords::FitContent>()) {
        // FIXME: The stretch-fit size is laid out against a zero available
        //        space, so this is always the min-content size. It should be
        //        clamp(min-content, stretch-fit, max-content) where stretch-fit
        //        is the available space minus the box's margins.
        //        https://www.w3.org/TR/css-sizing-3/#fit-content-size
        auto minSize = measureContent({MIN_CONTENT, 0_au});
        auto maxSize = measureContent({MAX_CONTENT, 0_au});
        auto stretchSize = measureContent({0_au, 0_au});
        return Some(clamp(stretchSize, minSize, maxSize));
    } else if (size.is<Keywords::Auto>()) {
        return NONE;
    } else {
        logWarn("unknown specified size: {}", size);
        return Some(0_au);
    }
}

Opt<Au> computeSpecifiedBorderBoxHeight(Tree& tree, Box& box, Size size, Math::Vec2<Opt<Au>> containingBlock, Au verticalBorderBox) {
    if (auto calc = size.is<Calc<PercentOr<Length>>>()) {
        auto specifiedHeight = resolve(tree, box, *calc, containingBlock.y.unwrapOr(0_au));
        if (box.style->boxSizing == BoxSizing::CONTENT_BOX) {
            specifiedHeight += verticalBorderBox;
        }
        return Some(specifiedHeight);
    }

    if (size.is<Keywords::MinContent>()) {
        // https://drafts.csswg.org/css-sizing-3/#valdef-width-min-content
        // for a box’s block size, unless otherwise specified, this is equivalent to its automatic size.
        return NONE;
    } else if (size.is<Keywords::MaxContent>()) {
        // https://drafts.csswg.org/css-sizing-3/#valdef-width-max-content
        // for a box’s block size, unless otherwise specified, this is equivalent to its automatic size.
        return NONE;
    } else if (size.is<Keywords::FitContent>()) {
        // Since this depends on min/max content size, this is also unknown.
        return NONE;
    } else if (size.is<Keywords::Auto>()) {
        return NONE;
    } else {
        logWarn("unknown specified size: {}", size);
        return Some(0_au);
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
    if (input.mode == LayoutMode::COMMIT) {
        auto boxMetrics = computeBoxMetrics(tree, box, input.position, size, input.usedSpacings);
        return Some(makeRc<BoxFragment>(box, boxMetrics, std::move(children)));
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
            .breakpoint = Some(Breakpoint::overflow())
        };

    return Ok(NONE);
}

Output layoutContentBox(Tree& tree, Box& box, Input input) {
    auto startAt = tree.fc.allowBreak() ? input.breakpointTraverser.getStart().unwrapOr(0uz) : 0uz;
    auto stopAt = tree.fc.allowBreak() and not tree.fc.isDiscoveryMode() ? input.breakpointTraverser.getEnd() : NONE;
    try$(_shouldAbortFragmentingBeforeLayout(tree.fc, input));
    auto out = _dispatchFormatingContext(
        tree, box, input, startAt, stopAt
    );

    Vec<Rc<PlaceholderFragment>> outOfFlowStash;

    if (input.mode == LayoutMode::COMMIT) {
        auto pending = std::move(out.outOfFlowStash);

        for (usize i = 0; i < pending.len(); ++i) {
            auto oofChild = pending[i];

            bool handleAsAbsoluteCb = oofChild->style().position == Keywords::ABSOLUTE and isAbsolutePositioningContainingBlock(*box.style);
            bool handleAsFixedCb = oofChild->style().position == Keywords::FIXED and isFixedPositioningContainingBlock(*box.style);

            if (handleAsAbsoluteCb or handleAsFixedCb) {
                auto containingBlock = RectAu{
                    {input.position.x, input.position.y},
                    out.size,
                };
                containingBlock = containingBlock.grow(input.usedSpacings.padding);

                auto childOutput = layoutAbsolutePositioned(tree, oofChild->originatingBox(), containingBlock, oofChild->staticPosRect, input.pageNumber);
                auto childFragment = childOutput.fragment.expect();

                pending.pushBack(childOutput.outOfFlowStash);

                childFragment->flags().set(Fragment::OUT_OF_FLOW);

                oofChild->fragment = Some(childFragment);

                out.fragment.expect()->_children.pushBack(childFragment);
            } else {
                outOfFlowStash.pushBack(oofChild);
            }
        }
    }

    out.outOfFlowStash = std::move(outOfFlowStash);

    return out;
}

// FIXME: Inner sizes should floor at zero, and definite available space
//        should be adapted too.
//        https://www.w3.org/TR/css-sizing-3/#box-sizing
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

    if (input.mode == LayoutMode::COMMIT) {
        auto pending = std::move(out.outOfFlowStash);

        for (usize i = 0; i < pending.len(); ++i) {
            auto oofChild = pending[i];

            auto childOutput = layoutAbsolutePositioned(tree, oofChild->originatingBox(), definiteOrZero(input.containingBlock), oofChild->staticPosRect, input.pageNumber);
            auto childFragment = childOutput.fragment.expect();

            pending.pushBack(childOutput.outOfFlowStash);

            childFragment->flags().set(Fragment::OUT_OF_FLOW);

            oofChild->fragment = Some(childFragment);

            out.fragment.expect()->_children.pushBack(childFragment);
        }
    }

    return out;
}

} // namespace Vaev::Layout
