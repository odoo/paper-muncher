export module Vaev.Engine:layout.svg;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :dom.names;
import :layout.base;
import :style;
import :values;
import :css.values;
import :style.values;

using namespace Karm;

namespace Vaev::Layout {

Au normalizedDiagonal(Vec2Au relativeTo) {
    return Au{
        Math::sqrt(f64{(relativeTo.x.cast<f64>() * relativeTo.x.cast<f64>()) + (relativeTo.y.cast<f64>() * relativeTo.y.cast<f64>())}) / Math::sqrt(2.0)
    };
}

// MARK: Root ----------------------------------------------------------------------------

struct SvgFormatingContext : FormatingContext {
    // https://svgwg.org/svg2-draft/coords.html#SizingSVGInCSS
    Opt<f64> intrinsicAspectRatio(Opt<SvgViewBox> const& vb, Style::Size const& width, Style::Size const& height) {
        // 1. If the width and height sizing properties on the ‘svg’ element are both absolute values:
        auto absolute = [](Style::Size const& size) -> Opt<Style::Pixel> {
            if (auto pp = size.is<Style::PixelPercentage>())
                return pp->is<Style::Pixel>();
            return NONE;
        };
        if (auto w = absolute(width), h = absolute(height); w and h)
            return Some(w->value() / h->value());

        // 2. If an SVG View is active:
        // TODO

        // 3. If the ‘viewBox’ on the ‘svg’ element is correctly specified:
        if (vb)
            return Some((f64)vb->width / vb->height);

        return NONE;
    }

    Vec2Au _resolvePercentageRelative(Opt<SvgViewBox> const& viewbox, Math::Trans2f transform, RectAu boundingBox) {
        // https://svgwg.org/svg2-draft/coords.html#Units
        // SPEC: For <percentage> values that are defined to be relative to the size of SVG viewport:
        // the value to use must be the percentage, in user units, of the * parameter of the ‘viewBox’ applied to that
        // viewport. If no ‘viewBox’ is specified, then the value to use must be the percentage, in user units, of the
        // * of the SVG viewport.
        return viewbox
                   ? Vec2Au{Au{viewbox->width}, Au{viewbox->height}}
                   : transform.inverse().applyVector(boundingBox.size().cast<f64>()).cast<Au>();
    }

    // https://svgwg.org/svg2-draft/paths.html
    static Math::Path _resolveShape(Style::ComputedValues const& style) {
        if (auto d = style.svg->d.is<String>())
            return Math::Path::fromSvg(*d);
        return Math::Path{};
    }

    static RectAu _resolveRectangle(Style::ComputedValues const& style, Vec2Au const& relativeTo) {
        return {
            Layout::resolve(style.svg->x, relativeTo.x),
            Layout::resolve(style.svg->y, relativeTo.y),
            Layout::resolve(style.sizing->width.is<Style::PixelPercentage>().unwrapOr(Style::Percent{100}), relativeTo.x),
            Layout::resolve(style.sizing->height.is<Style::PixelPercentage>().unwrapOr(Style::Percent{100}), relativeTo.y),
        };
    }

    static EllipseAu _resolveCircle(Style::ComputedValues const& style, Vec2Au const& relativeTo) {
        return {
            Vaev::Layout::resolve(style.svg->cx, relativeTo.x),
            Vaev::Layout::resolve(style.svg->cy, relativeTo.y),
            Vaev::Layout::resolve(style.svg->r, relativeTo.x),
        };
    }

    static void _commitShape(Box& box, FragmentBuilder& fragBuilder, Vec2Au relativeTo) {
        Au resolvedStrokeWidth = Vaev::Layout::resolve(box.style->svgPaint->strokeWidth, normalizedDiagonal(relativeTo));
        auto shape = box.content.expect<Css::SvgShapeElement>();

        switch (shape) {
        case Css::SvgShapeElement::RECT:
            fragBuilder.addChildIfAny(
                Some(makeRc<SvgShapeFragment>(box, _resolveRectangle(*box.style, relativeTo), resolvedStrokeWidth))
            );
            break;

        case Css::SvgShapeElement::CIRCLE:
            fragBuilder.addChildIfAny(
                Some(makeRc<SvgShapeFragment>(box, _resolveCircle(*box.style, relativeTo), resolvedStrokeWidth))
            );
            break;

        case Css::SvgShapeElement::PATH:
            fragBuilder.addChildIfAny(
                Some(makeRc<SvgShapeFragment>(box, _resolveShape(*box.style), resolvedStrokeWidth))
            );
            break;

        default:
            unreachable();
        }
    }

    void _commitElement(Input const& input, Box& box, FragmentBuilder& fragBuilder, Vec2Au resolveTo) {
        if (box.content.is<Css::SvgShapeElement>()) {
            _commitShape(box, fragBuilder, resolveTo);
        } else if (box.isSvgRootBox()) {
            auto resolvedRect = _resolveRectangle(*box.style, resolveTo);
            auto root = _commitRoot(
                input,
                box,
                {resolvedRect.x, resolvedRect.y},
                {resolvedRect.width, resolvedRect.height}
            );
            fragBuilder.addChildIfAny(Some(root));
        } else if (box.isSvgForeignObjectBox()) {
            auto resolvedRect = _resolveRectangle(*box.style, resolveTo);

            Input childInput{
                .fragmentainer = input.fragmentainer,
                .generateFragment = true,
                .usedSpacings = {},
                .knownSize = {
                    Some(resolvedRect.width),
                    Some(resolvedRect.height),
                },
                .position = {resolvedRect.x, resolvedRect.y},
            };

            auto output = layoutBorderBox(box, childInput);
            fragBuilder.addChildIfAny(output.fragment);

        } else if (box.isSvg()) {
            auto nestedGroupFragBuilder = FragmentBuilder{box};

            _commitChildren(input, box, nestedGroupFragBuilder, resolveTo);

            fragBuilder.addChildIfAny(Some(nestedGroupFragBuilder.buildSvgGroup()));
        } else {
            unreachable();
        }
    }

    void _commitChildren(Input const& input, Box& group, FragmentBuilder& fragBuilder, Vec2Au resolveTo) {
        for (auto& c : group.children())
            _commitElement(input, c, fragBuilder, resolveTo);
    }

    // https://svgwg.org/svg2-draft/coords.html#ComputingAViewportsTransform
    Math::Trans2f _computeEquivalentTransformOfSVGViewport(SvgViewBox const& vb, Vec2Au const& position, Vec2Au const& size) {
        // 1. Let vb-x, vb-y, vb-width, vb-height be the min-x, min-y, width and height values of the viewBox attribute
        // respectively.
        // 2. Let e-x, e-y, e-width, e-height be the position and size of the element respectively.

        // 3. Let align be the align value of preserveAspectRatio, or 'xMidYMid' if preserveAspectRatio is not defined.
        // FIXME: preserveAspectRatio still not implemented
        Opt<Css::SvgAlignAxis> align = Some(Css::SvgAlignAxis{Css::SvgAlignAxis::MID, Css::SvgAlignAxis::MID});

        // 4. Let meetOrSlice be the meetOrSlice value of preserveAspectRatio, or 'meet' if preserveAspectRatio is not
        // defined or if meetOrSlice is missing from this value.
        // FIXME: preserveAspectRatio still not implemented
        Css::SvgMeetOrSlice meetOrSlice{Css::SvgMeetOrSlice::MEET};

        // 5. Initialize scale-x to e-width/vb-width.
        f64 scaleX = (f64)size.x / vb.width;

        // 6. Initialize scale-y to e-height/vb-height.
        f64 scaleY = (f64)size.y / vb.height;

        // 7. If align is not 'none' and meetOrSlice is 'meet', set the larger of scale-x and scale-y to the smaller.
        // 8. Otherwise, if align is not 'none' and meetOrSlice is 'slice', set the smaller of scale-x and scale-y to
        // the larger.
        if (align != NONE and meetOrSlice == Css::SvgMeetOrSlice::MEET) {
            scaleY = scaleX = min(scaleX, scaleY);
        } else if (align != NONE and meetOrSlice == Css::SvgMeetOrSlice::SLICE) {
            scaleY = scaleX = max(scaleX, scaleY);
        }

        // 9. Initialize translate-x to e-x - (vb-x * scale-x).
        f64 translateX = (f64)position.x - (vb.minX * scaleX);

        // 10. Initialize translate-y to e-y - (vb-y * scale-y)
        f64 translateY = (f64)position.y - (vb.minY * scaleY);

        if (align) {
            // 11. If align contains 'xMid', add (e-width - vb-width * scale-x) / 2 to translate-x.
            if (align->x == Css::SvgAlignAxis::MID) {
                translateX += ((f64)size.x - vb.width * scaleX) / 2;
            }

            // 12. If align contains 'xMax', add (e-width - vb-width * scale-x) to translate-x.
            if (align->x == Css::SvgAlignAxis::MAX) {
                translateX += ((f64)size.x - vb.width * scaleX);
            }

            // 13. If align contains 'yMid', add (e-height - vb-height * scale-y) / 2 to translate-y.
            if (align->y == Css::SvgAlignAxis::MID) {
                translateY += ((f64)size.y - vb.height * scaleY) / 2;
            }

            // 14. If align contains 'yMax', add (e-height - vb-height * scale-y) to translate-y.
            if (align->y == Css::SvgAlignAxis::MAX) {
                translateY += ((f64)size.y - vb.height * scaleY);
            }
        }

        return Math::Trans2f::translate({(f64)translateX, (f64)translateY}).scaled({(f64)scaleX, (f64)scaleY});
    }

    Rc<Fragment> _commitRoot(Input const& input, Box& box, Vec2Au position, Vec2Au size) {
        auto viewbox = box.style->svg->viewBox;

        Math::Trans2f trans = Math::Trans2f::translate(position.cast<f64>());
        if (viewbox)
            trans = _computeEquivalentTransformOfSVGViewport(*viewbox, position, size);

        auto rootFragBuilder = FragmentBuilder{box};

        auto boundingBox = RectAu{position, size};

        _commitChildren(input, box, rootFragBuilder, _resolvePercentageRelative(viewbox, trans, boundingBox));

        return rootFragBuilder.buildSvgRoot(trans, boundingBox);
    }

    // https://www.w3.org/TR/css-images-3/#default-sizing
    // NOTE: not fully compliant
    Vec2Au _defaultSizing(Math::Vec2<Opt<Au>> knownSize, Opt<f64> aspectRatio, Vec2Au defaultSize) {
        // If the specified size is a definite width and height, the concrete object size is given that width and height.
        if (knownSize.x and knownSize.y)
            return {*knownSize.x, *knownSize.y};

        Vec2Au size = defaultSize;
        if (knownSize.x) {
            size.x = *knownSize.x;
            if (aspectRatio)
                size.y = size.x / *aspectRatio;
        } else if (knownSize.y) {
            size.y = *knownSize.y;
            if (aspectRatio)
                size.x = size.y * *aspectRatio;
        } else if (aspectRatio) {
            size.y = size.x / *aspectRatio;
        }

        return size;
    }

    Output run(Box& box, Input input, [[maybe_unused]] usize startAt, [[maybe_unused]] Opt<usize> stopAt) override {
        input.fragmentainer->enterMonolithicBox();
        Defer _ = [&] {
            input.fragmentainer->leaveMonolithicBox();
        };

        auto aspectRatio = intrinsicAspectRatio(
            box.style->svg->viewBox,
            box.style->sizing->width,
            box.style->sizing->height
        );
        auto size = _defaultSizing(input.knownSize, aspectRatio, input.containingBlock);

        auto fragment = _commitRoot(input, box, input.position, size);

        if (input.fragmentainer->allowBreak() and
            not input.fragmentainer->acceptsFit(
                input.position.y,
                size.y,
                input.pendingVerticalSizes
            )) {
            return {
                .fragment = Some(fragment),
                .size = {},
                .completelyLaidOut = false,
                .breakpoint = Some(Breakpoint::overflow()),
                .firstBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
                .lastBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
            };
        }

        return {
            .fragment = Some(fragment),
            .size = size,
            .completelyLaidOut = true,
            .breakpoint = Some(Breakpoint::bottomOfMonolithicBox(box)),
            .firstBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
            .lastBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
        };
    }
};

export Rc<FormatingContext> constructSvgFormatingContext(Box&) {
    return makeRc<SvgFormatingContext>();
}

} // namespace Vaev::Layout
