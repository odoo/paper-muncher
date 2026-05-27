export module Vaev.Engine:layout.svg;

import Karm.Core;
import Karm.Scene;
import Karm.Gfx;
import Karm.Math;

import :dom.names;
import :layout.base;
import :style;
import :values;

using namespace Karm;

namespace Vaev::Layout {

// SVG sizes shouldn't be defined using calc values
Opt<PercentOr<Length>> extractValueFromCalc(CalcValue<PercentOr<Length>> const& size) {
    return size.visit(
        [](CalcValue<PercentOr<Length>>::Value const& v) {
            return Opt{v.unwrap<PercentOr<Length>>()};
        },
        [](auto const) {
            return Opt<PercentOr<Length>>{NONE};
        }
    );
}

// FIXME: this should be targeted by the computer refactoring, so we have only resolved percentage or length values
PercentOr<Length> fromSize(Size const& size) {
    if (size.is<Keywords::Auto>())
        return PercentOr<Length>{Percent{100}};

    return extractValueFromCalc(size.unwrap<CalcValue<PercentOr<Length>>>())
        .unwrapOr(PercentOr<Length>{Percent{100}});
}

// FIXME: already present in Resolver obj, but it doesnt make sense to instantiate one here; should also be targeted
// by the computer refactoring
Au resolve(PercentOr<Length> const& value, Au relative) {
    if (auto valueLength = value.is<Length>())
        return Au{valueLength->val()};
    return Au{relative.cast<f64>() * (value.unwrap<Percent>().value() / 100.)};
}

Au normalizedDiagonal(Vec2Au relativeTo) {
    return Au{
        Math::sqrt(f64{(relativeTo.x.cast<f64>() * relativeTo.x.cast<f64>()) + (relativeTo.y.cast<f64>() * relativeTo.y.cast<f64>())}) / Math::sqrt(2.0)
    };
}

// MARK: Root ----------------------------------------------------------------------------

struct SvgFormatingContext : FormatingContext {
    // https://svgwg.org/svg2-draft/coords.html#SizingSVGInCSS
    Opt<Number> intrinsicAspectRatio(Opt<SvgViewBox> const& vb, Size const& width, Size const& height) {
        // FIXME: again this should be targetted by the styling computation refactoring,
        // where Size will be resolved to a mix between Percent and Lengths
        auto absoluteValue = [](Size size) -> Opt<Length> {
            if (not size.is<CalcValue<PercentOr<Length>>>())
                return NONE;

            auto calc = size.unwrap<CalcValue<PercentOr<Length>>>();

            auto percOrLength = extractValueFromCalc(calc);
            if (not percOrLength)
                return NONE;

            if (percOrLength->is<Percent>())
                return NONE;

            return percOrLength->unwrap<Length>();
        };

        auto absWidth = absoluteValue(width);
        auto absHeight = absoluteValue(height);

        // 1. If the width and height sizing properties on the ‘svg’ element are both absolute values:
        if (absWidth and absHeight)
            return absWidth->val() / absHeight->val();

        // 2. If an SVG View is active:
        // TODO

        // 3. If the ‘viewBox’ on the ‘svg’ element is correctly specified:
        if (vb)
            return (f64)vb->width / vb->height;

        return NONE;
    }

    Vec2Au _resolvePercentageRelative(Opt<SvgViewBox> const& viewbox, SvgRootFrag& svgFrag) {
        // https://svgwg.org/svg2-draft/coords.html#Units
        // SPEC: For <percentage> values that are defined to be relative to the size of SVG viewport:
        // the value to use must be the percentage, in user units, of the * parameter of the ‘viewBox’ applied to that
        // viewport. If no ‘viewBox’ is specified, then the value to use must be the percentage, in user units, of the
        // * of the SVG viewport.
        return viewbox
                   ? Vec2Au{Au{viewbox->width}, Au{viewbox->height}}
                   : svgFrag
                         .transf.inverse()
                         .applyVector(
                             Vec2Au{
                                 svgFrag.boundingBox.width,
                                 svgFrag.boundingBox.height,
                             }
                                 .cast<f64>()
                         )
                         .cast<Au>();
    }

    // https://svgwg.org/svg2-draft/paths.html
    static Rc<Math::Path> _resolveShape(Style::ComputedValues const& style) {
        if (auto d = style.svg->d.is<String>())
            return Karm::makeRc<Math::Path>(Math::Path::fromSvg(*d));
        return Karm::makeRc<Math::Path>(Math::Path{});
    }

    static RectAu _resolveRectangle(Style::ComputedValues const& style, Vec2Au const& relativeTo) {
        return {
            Layout::resolve(style.svg->x, relativeTo.x),
            Layout::resolve(style.svg->y, relativeTo.y),
            Layout::resolve(fromSize(style.sizing->width), relativeTo.x),
            Layout::resolve(fromSize(style.sizing->height), relativeTo.y)
        };
    }

    static EllipseAu _resolveCircle(Style::ComputedValues const& style, Vec2Au const& relativeTo) {
        return {
            Vaev::Layout::resolve(style.svg->cx, relativeTo.x),
            Vaev::Layout::resolve(style.svg->cy, relativeTo.y),
            Vaev::Layout::resolve(style.svg->r, relativeTo.x),
        };
    }

    static SvgShapeFrag _commitShape(Box& box, Vec2Au relativeTo) {
        Au resolvedStrokeWidth = Vaev::Layout::resolve(box.style->svg->strokeWidth, normalizedDiagonal(relativeTo));
        auto shape = box.content.unwrap<SvgShapeElement>();

        switch (shape) {
        case SvgShapeElement::RECT: {
            return {
                _resolveRectangle(*box.style, relativeTo),
                box,
                resolvedStrokeWidth,
            };
        }
        case SvgShapeElement::CIRCLE: {
            return {
                _resolveCircle(*box.style, relativeTo),
                box,
                resolvedStrokeWidth,
            };
        }
        case SvgShapeElement::PATH: {
            return {
                _resolveShape(*box.style),
                box,
                resolvedStrokeWidth,
            };
        }

        default:
            unreachable();
        }
    }

    SvgGroupFrag::Element _commitElement(Tree& tree, Box& box, Vec2Au resolveTo) {
        if (box.content.is<SvgShapeElement>()) {
            return _commitShape(box, resolveTo);
        } else if (box.isSvgRootBox()) {
            auto resolvedRect = _resolveRectangle(*box.style, resolveTo);
            return _commitRoot(
                tree,
                box,
                {resolvedRect.x, resolvedRect.y},
                {resolvedRect.width, resolvedRect.height}
            );
        } else if (box.isSvgForeignObjectBox()) {
            auto resolvedRect = _resolveRectangle(*box.style, resolveTo);

            auto frag = Frag();
            Input childInput{
                .knownSize = {resolvedRect.width, resolvedRect.height},
                .position = {resolvedRect.x, resolvedRect.y},
            };
            auto output = layoutAndCommitBorderBox(tree, box, childInput, frag, UsedSpacings{});
            return makeBox<Frag>(std::move(frag.children()[0]));
        } else if (box.isSvg()) {
            SvgGroupFrag nestedGroupFrag{box};
            _commitChildren(tree, box, nestedGroupFrag, resolveTo);
            return nestedGroupFrag;
        } else {
            unreachable();
        }
    }

    void _commitChildren(Tree& tree, Box& group, SvgGroupFrag& groupFrag, Vec2Au resolveTo) {
        for (auto& c : group.children())
            groupFrag.add(_commitElement(tree, c, resolveTo));
    }

    // https://svgwg.org/svg2-draft/coords.html#ComputingAViewportsTransform
    Math::Trans2f _computeEquivalentTransformOfSVGViewport(SvgViewBox const& vb, Vec2Au const& position, Vec2Au const& size) {
        // 1. Let vb-x, vb-y, vb-width, vb-height be the min-x, min-y, width and height values of the viewBox attribute
        // respectively.
        // 2. Let e-x, e-y, e-width, e-height be the position and size of the element respectively.

        // 3. Let align be the align value of preserveAspectRatio, or 'xMidYMid' if preserveAspectRatio is not defined.
        // FIXME: preserveAspectRatio still not implemented
        Opt<SvgAlignAxis> align{SvgAlignAxis{SvgAlignAxis::MID, SvgAlignAxis::MID}};

        // 4. Let meetOrSlice be the meetOrSlice value of preserveAspectRatio, or 'meet' if preserveAspectRatio is not
        // defined or if meetOrSlice is missing from this value.
        // FIXME: preserveAspectRatio still not implemented
        SvgMeetOrSlice meetOrSlice{SvgMeetOrSlice::MEET};

        // 5. Initialize scale-x to e-width/vb-width.
        f64 scaleX = (f64)size.x / vb.width;

        // 6. Initialize scale-y to e-height/vb-height.
        f64 scaleY = (f64)size.y / vb.height;

        // 7. If align is not 'none' and meetOrSlice is 'meet', set the larger of scale-x and scale-y to the smaller.
        // 8. Otherwise, if align is not 'none' and meetOrSlice is 'slice', set the smaller of scale-x and scale-y to
        // the larger.
        if (align != NONE and meetOrSlice == SvgMeetOrSlice::MEET) {
            scaleY = scaleX = min(scaleX, scaleY);
        } else if (align != NONE and meetOrSlice == SvgMeetOrSlice::SLICE) {
            scaleY = scaleX = max(scaleX, scaleY);
        }

        // 9. Initialize translate-x to e-x - (vb-x * scale-x).
        f64 translateX = (f64)position.x - (vb.minX * scaleX);

        // 10. Initialize translate-y to e-y - (vb-y * scale-y)
        f64 translateY = (f64)position.y - (vb.minY * scaleY);

        if (align) {
            // 11. If align contains 'xMid', add (e-width - vb-width * scale-x) / 2 to translate-x.
            if (align->x == SvgAlignAxis::MID) {
                translateX += ((f64)size.x - vb.width * scaleX) / 2;
            }

            // 12. If align contains 'xMax', add (e-width - vb-width * scale-x) to translate-x.
            if (align->x == SvgAlignAxis::MAX) {
                translateX += ((f64)size.x - vb.width * scaleX);
            }

            // 13. If align contains 'yMid', add (e-height - vb-height * scale-y) / 2 to translate-y.
            if (align->y == SvgAlignAxis::MID) {
                translateY += ((f64)size.y - vb.height * scaleY) / 2;
            }

            // 14. If align contains 'yMax', add (e-height - vb-height * scale-y) to translate-y.
            if (align->y == SvgAlignAxis::MAX) {
                translateY += ((f64)size.y - vb.height * scaleY);
            }
        }

        return Math::Trans2f::translate({(f64)translateX, (f64)translateY}).scaled({(f64)scaleX, (f64)scaleY});
    }

    SvgRootFrag _commitRoot(Tree& tree, Box& box, Vec2Au position, Vec2Au size) {
        auto viewbox = box.style->svg->viewBox;

        Math::Trans2f trans = Math::Trans2f::translate(position.cast<f64>());
        if (viewbox)
            trans = _computeEquivalentTransformOfSVGViewport(*viewbox, position, size);

        SvgRootFrag svgFrag = {
            box,
            trans,
            {position, size},
        };

        _commitChildren(tree, box, svgFrag, _resolvePercentageRelative(viewbox, svgFrag));
        return svgFrag;
    }

    // https://www.w3.org/TR/css-images-3/#default-sizing
    // NOTE: not fully compliant
    Vec2Au _defaultSizing(Math::Vec2<Opt<Au>> knownSize, Opt<f64> aspectRatio, Vec2Au defaultSize) {
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

    Output run(Tree& tree, Box& box, Input input, [[maybe_unused]] usize startAt, [[maybe_unused]] Opt<usize> stopAt) override {
        auto aspectRatio = intrinsicAspectRatio(
            box.style->svg->viewBox,
            box.style->sizing->width,
            box.style->sizing->height
        );
        auto size = _defaultSizing(input.knownSize, aspectRatio, input.containingBlock);

        if (input.fragment) {
            auto svgFrag = _commitRoot(tree, box, input.position, size);
            svgFrag.computeBoundingBoxes();
            input.fragment->content = svgFrag;
        }

        if (tree.fc.allowBreak() and
            not tree.fc.acceptsFit(
                input.position.y,
                size.y,
                input.pendingVerticalSizes
            )) {
            return {
                .size = {},
                .completelyLaidOut = false,
                .breakpoint = Breakpoint::overflow(),
                .firstBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
                .lastBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
            };
        }

        return {
            .size = size,
            .completelyLaidOut = true,
            .breakpoint = Breakpoint::bottomOfMonolithicBox(box),
            .firstBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
            .lastBaselineSet = BaselinePositionsSet::fromSinglePosition(size.y),
        };
    }
};

export Rc<FormatingContext> constructSvgFormatingContext(Box&) {
    return makeRc<SvgFormatingContext>();
}

} // namespace Vaev::Layout
