#include "layout.h"

#include "block.h"
#include "box.h"
#include "flex.h"
#include "grid.h"
#include "table.h"
#include "values.h"

namespace Vaev::Layout {

Output _contentLayout(Tree &tree, Box &box, Input input) {
    auto display = box.style->display;

    if (auto run = box.content.is<Karm::Text::Run>()) {
        box.layout.fontSize = resolve(tree, box, box.style->font->size);
        Karm::Text::Font font = {
            box.fontFace,
            box.layout.fontSize.cast<f64>(),
        };
        run->shape(font);
        return Output::fromSize(run->size().cast<Px>());
    } else if (
        display == Display::FLOW or
        display == Display::FLOW_ROOT or
        display == Display::TABLE_CELL or
        display == Display::TABLE_CAPTION
    ) {
        return blockLayout(tree, box, input);
    } else if (display == Display::FLEX) {
        return flexLayout(tree, box, input);
    } else if (display == Display::GRID) {
        return gridLayout(tree, box, input);
    } else if (display == Display::TABLE) {
        return tableLayout(tree, box, input);
    } else if (display == Display::INTERNAL) {
        return Output{};
    } else {
        return blockLayout(tree, box, input);
    }
}

InsetsPx computeMargins(Tree &tree, Box &box, Input input) {
    InsetsPx res;
    auto margin = box.style->margin;

    res.top = resolve(tree, box, margin->top, input.containingBlock.height);
    res.end = resolve(tree, box, margin->end, input.containingBlock.width);
    res.bottom = resolve(tree, box, margin->bottom, input.containingBlock.height);
    res.start = resolve(tree, box, margin->start, input.containingBlock.width);

    return res;
}

InsetsPx computeBorders(Tree &tree, Box &box) {
    InsetsPx res;
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

static InsetsPx _computePaddings(Tree &tree, Box &box, Input input) {
    InsetsPx res;
    auto padding = box.style->padding;

    res.top = resolve(tree, box, padding->top, input.containingBlock.height);
    res.end = resolve(tree, box, padding->end, input.containingBlock.width);
    res.bottom = resolve(tree, box, padding->bottom, input.containingBlock.height);
    res.start = resolve(tree, box, padding->start, input.containingBlock.width);

    return res;
}

static Math::Radii<Px> _computeRadii(Tree &tree, Box &box, Vec2Px size) {
    auto radii = box.style->borders->radii;
    Math::Radii<Px> res;

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

static Cons<Opt<Px>, IntrinsicSize> _computeSpecifiedSize(Tree &tree, Box &box, Input input, Size size, IntrinsicSize intrinsic) {
    if (size == Size::MIN_CONTENT or intrinsic == IntrinsicSize::MIN_CONTENT) {
        return {NONE, IntrinsicSize::MIN_CONTENT};
    } else if (size == Size::MAX_CONTENT or intrinsic == IntrinsicSize::MAX_CONTENT) {
        return {NONE, IntrinsicSize::MAX_CONTENT};
    } else if (size == Size::AUTO) {
        return {NONE, IntrinsicSize::AUTO};
    } else if (size == Size::FIT_CONTENT) {
        return {NONE, IntrinsicSize::STRETCH_TO_FIT};
    } else if (size == Size::LENGTH) {
        return {resolve(tree, box, size.value, input.containingBlock.width), IntrinsicSize::AUTO};
    } else {
        logWarn("unknown specified size: {}", size);
        return {Px{0}, IntrinsicSize::AUTO};
    }
}

Output layout(Tree &tree, Box &box, Input input) {
    // FIXME: confirm how the preffered width/height parameters interacts with intrinsic size argument from input
    auto borders = computeBorders(tree, box);
    auto padding = _computePaddings(tree, box, input);
    auto sizing = box.style->sizing;

    auto [specifiedWidth, widthIntrinsicSize] = _computeSpecifiedSize(tree, box, input, sizing->width, input.intrinsic.x);
    if (input.knownSize.width == NONE) {
        // FIXME: making prefered width as mandatory width; im not sure this is ok
        input.knownSize.width = specifiedWidth;
    }
    input.knownSize.width = input.knownSize.width.map([&](auto s) {
        if (sizing->maxWidth == Size::LENGTH) {
            auto maxWidth = resolve(tree, box, sizing->maxWidth.value, input.containingBlock.width);
            s = min(s, maxWidth);
        }
        if (sizing->minWidth == Size::LENGTH) {
            auto minWidth = resolve(tree, box, sizing->minWidth.value, input.containingBlock.width);
            s = max(s, minWidth);
        }

        if (box.style->boxSizing == BoxSizing::CONTENT_BOX and specifiedWidth != NONE)
            return s;
        return max(Px{0}, s - padding.horizontal() - borders.horizontal());
    });
    input.intrinsic.x = widthIntrinsicSize;

    auto [specifiedHeight, heightIntrinsicSize] = _computeSpecifiedSize(tree, box, input, sizing->height, input.intrinsic.y);
    if (input.knownSize.height == NONE) {
        input.knownSize.height = specifiedHeight;
    }

    input.knownSize.height = input.knownSize.height.map([&](auto s) {
        if (sizing->maxHeight == Size::LENGTH) {
            auto maxHeight = resolve(tree, box, sizing->maxHeight.value, input.containingBlock.height);
            s = min(s, maxHeight);
        }
        if (sizing->minHeight == Size::LENGTH) {
            auto minHeight = resolve(tree, box, sizing->minHeight.value, input.containingBlock.height);
            s = max(s, minHeight);
        }

        if (box.style->boxSizing == BoxSizing::CONTENT_BOX and specifiedWidth != NONE)
            return s;
        return max(Px{0}, s - padding.vertical() - borders.vertical());
    });
    input.intrinsic.y = heightIntrinsicSize;

    input.position = input.position + borders.topStart() + padding.topStart();

    auto [size] = _contentLayout(tree, box, input);

    size.width = input.knownSize.width.unwrapOr(size.width);
    size.height = input.knownSize.height.unwrapOr(size.height);

    size = size + padding.all() + borders.all();

    if (input.commit == Commit::YES) {
        box.layout.position = input.position - borders.topStart() - padding.topStart();
        box.layout.borderSize = size;
        box.layout.padding = padding;
        box.layout.borders = borders;
        box.layout.radii = _computeRadii(tree, box, size);
    }

    return Output::fromSize(size);
}

} // namespace Vaev::Layout
