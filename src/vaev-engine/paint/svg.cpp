module Vaev.Engine;

import Karm.Scene;
import Karm.Gfx;
import Karm.Math;
import Karm.Core;

import :layout.base;
import :layout.values;

namespace Vaev::Paint {

Rc<Scene::Stack> _paintSvgAggregate(Layout::Svg::GroupFrag& group, Gfx::Color currentColor, RectAu viewBox);
Rc<Scene::Node> _paintSvg(Layout::SvgRootFrag& svgRoot, Gfx::Color currentColor);

// https://www.w3.org/TR/css-transforms-1/#transform-box
static RectAu _resolveTransformReferenceSVG(Layout::Svg::Frag& svgFrag, RectAu viewBox, TransformBox box) {
    // For SVG elements without associated CSS layout box, the used value
    // for content-box is fill-box and for border-box is stroke-box.
    return box.visit(
        Visitor{
            [&](Keywords::ContentBox const&) {
                return svgFrag.objectBoundingBox();
            },
            [&](Keywords::BorderBox const&) {
                return svgFrag.strokeBoundingBox();
            },
            [&](Keywords::FillBox const&) {
                return svgFrag.objectBoundingBox();
            },
            [&](Keywords::StrokeBox const&) {
                return svgFrag.strokeBoundingBox();
            },
            [&](Keywords::ViewBox const&) {
                return viewBox;
            },
        }
    );
}

Rc<Scene::Node> _applyTransformIfNeeded(Layout::Svg::Frag& svgFrag, RectAu viewBox, Rc<Scene::Node> content) {
    auto const& transform = *svgFrag.style().transform;
    if (not transform.has())
        return content;
    auto referenceBox = _resolveTransformReferenceSVG(svgFrag, viewBox, transform.box);
    return _applyTransform(transform, referenceBox, content);
}

Rc<Scene::Node> _paintSvgElement(Layout::Svg::GroupFrag::Element& element, Gfx::Color currentColor, RectAu viewBox) {
    if (auto shape = element.is<Layout::Svg::ShapeFrag>()) {
        return _applyTransformIfNeeded(
            *shape, viewBox,
            shape->toSceneNode(currentColor)
        );
    } else if (auto nestedGroup = element.is<Layout::Svg::GroupFrag>()) {
        return _applyTransformIfNeeded(
            *nestedGroup, viewBox,
            _paintSvgAggregate(*nestedGroup, currentColor, viewBox)
        );
    } else if (auto nestedRoot = element.is<Layout::SvgRootFrag>()) {
        return _applyTransformIfNeeded(
            *nestedRoot, viewBox,
            makeRc<Scene::Clip>(
                _paintSvg(*nestedRoot, currentColor),
                nestedRoot->boundingBox.toRect().cast<f64>()
            )
        );
    } else if (auto foreignObject = element.is<::Box<Layout::Frag>>()) {
        auto& frag = **foreignObject;
        Scene::Stack stackForeignObj;
        _establishStackingContext(frag, stackForeignObj, {});
        return makeRc<Scene::Clip>(
            makeRc<Scene::Stack>(std::move(stackForeignObj)),
            frag.metrics.borderBox().cast<f64>()
        );
    }
    unreachable();
}

Rc<Scene::Stack> _paintSvgAggregate(Layout::Svg::GroupFrag& group, Gfx::Color currentColor, RectAu viewBox) {
    // NOTE: A SVG group does not create a stacking context, but its easier to manipulate a group if itself is its own node
    Scene::Stack stack;
    for (auto& element : group.elements)
        stack.add(_paintSvgElement(element, currentColor, viewBox));
    return makeRc<Scene::Stack>(std::move(stack));
}

Rc<Scene::Node> _paintSvg(Layout::SvgRootFrag& svgRoot, Gfx::Color currentColor) {
    // FIXME: Ugly cast because we need to upcast, should be fixed once we unify SvgRootFrag with Frag
    Layout::SvgRoot const& rootBox = *static_cast<Layout::SvgRoot const*>(svgRoot.box.buf());

    // https://drafts.csswg.org/css-transforms/#transform-box
    // SPEC: The reference box is positioned at the origin of the coordinate system established
    // by the viewBox attribute.
    // NOTE: Origin here was interpreted as (0, 0) instead of (minX, minY).
    // NOTE: It is not clear whether '(SPEC) the nearest SVG viewport' includes the root's viewBox itself.
    // We assume it doesn't.
    RectAu viewBox =
        rootBox.viewBox
            ? Math::Rect{Math::Vec2{rootBox.viewBox->width, rootBox.viewBox->height}}.cast<Au>()
            : svgRoot.boundingBox.toRect();

    auto content = _paintSvgAggregate(svgRoot, currentColor, viewBox);
    return makeRc<Scene::Transform>(content, svgRoot.transf);
}

} // namespace Vaev::Paint
