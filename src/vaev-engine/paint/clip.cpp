module Vaev.Engine;

import Karm.Core;
import Karm.Scene;
import Karm.Math;

import :layout.base;
import :layout.values;

using namespace Karm;

namespace Vaev::Paint {

Rc<Scene::Node> _applyClip(Layout::Frag const& frag, Rc<Scene::Node> content) {
    Math::Path result;
    auto& clip = frag.style().clip->unwrap();

    // TODO: handle SVG cases (https://drafts.fxtf.org/css-masking/#typedef-geometry-box)
    auto [referenceBox, radii] = clip.referenceBox.visit(Visitor{
        [&](Keywords::BorderBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag.metrics.borderBox(), frag.metrics.radii};
        },
        [&](Keywords::PaddingBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag.metrics.paddingBox(), {0_au}};
        },
        [&](Keywords::ContentBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag.metrics.contentBox(), {0_au}};
        },
        [&](Keywords::MarginBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag.metrics.marginBox(), {0_au}};
        },
        [&](Keywords::FillBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag.metrics.contentBox(), {0_au}};
        },
        [&](Keywords::StrokeBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag.metrics.borderBox(), frag.metrics.radii};
        },
        [&](Keywords::ViewBox const&) -> Pair<RectAu, RadiiAu> {
            return {frag.metrics.borderBox(), {0_au}};
        },
    });

    if (not clip.shape) {
        result.rect(referenceBox.round().cast<f64>(), radii.cast<f64>());
        return makeRc<Scene::Clip>(content, result);
    }

    auto resolver = Layout::Resolver();
    return clip.shape.unwrap().visit(Visitor{
        [&](Polygon const& polygon) {
            result.moveTo(
                referenceBox.xy.cast<f64>() +
                Math::Vec2f(
                    resolver.resolve(first(polygon.points).v0, referenceBox.width).cast<f64>(),
                    resolver.resolve(first(polygon.points).v1, referenceBox.height).cast<f64>()
                )
            );
            for (auto& point : next(polygon.points)) {
                result.lineTo(
                    referenceBox.xy.cast<f64>() +
                    Math::Vec2f(
                        resolver.resolve(point.v0, referenceBox.width).cast<f64>(),
                        resolver.resolve(point.v1, referenceBox.height).cast<f64>()
                    )
                );
            }

            return makeRc<Scene::Clip>(content, result, polygon.fillRule);
        },
        [&](Circle const& circle) {
            auto center = _resolveBackgroundPosition(resolver, circle.position, referenceBox);
            f64 radius;
            if (circle.radius.is<Keywords::ClosestSide>()) {
                radius = min(
                    Math::abs(referenceBox.width.cast<f64>() - center.x),
                    center.x,
                    center.y,
                    Math::abs(referenceBox.height.cast<f64>() - center.y)
                );
            } else if (circle.radius.is<Keywords::FarthestSide>()) {
                radius = max(
                    Math::abs(referenceBox.width.cast<f64>() - center.x),
                    center.x,
                    center.y,
                    Math::abs(referenceBox.height.cast<f64>() - center.y)
                );
            } else {
                auto hSquared = Math::pow2(referenceBox.height.cast<f64>());
                auto wSquared = Math::pow2(referenceBox.width.cast<f64>());
                radius = resolver.resolve(
                                     circle.radius.unwrap<CalcValue<PercentOr<Length>>>(),
                                     Au(Math::sqrt(hSquared + wSquared) / Math::sqrt(2.0))
                )
                             .cast<f64>();
            }
            result.ellipse(Math::Ellipsef(center + referenceBox.xy.cast<f64>(), radius));

            return makeRc<Scene::Clip>(content, result);
        },
        [&](Inset const& inset) {
            Math::Insetsf resolved;
            resolved.start = resolver.resolve(inset.insets.start, referenceBox.width).cast<f64>();
            resolved.end = resolver.resolve(inset.insets.end, referenceBox.width).cast<f64>();
            resolved.top = resolver.resolve(inset.insets.top, referenceBox.height).cast<f64>();
            resolved.bottom = resolver.resolve(inset.insets.bottom, referenceBox.height).cast<f64>();

            Math::Radiif resolvedRadii = _resolveRadii(resolver, inset.borderRadius, referenceBox);

            result.rect(referenceBox.cast<f64>().shrink(resolved), resolvedRadii);

            return makeRc<Scene::Clip>(content, result);
        },
        [&](Xywh const& xywh) {
            Math::Rectf resolvedRect;
            resolvedRect.x = resolver.resolve(xywh.rect.x, referenceBox.width).cast<f64>();
            resolvedRect.y = resolver.resolve(xywh.rect.y, referenceBox.height).cast<f64>();
            resolvedRect.width = resolver.resolve(xywh.rect.width, referenceBox.width).cast<f64>();
            resolvedRect.height = resolver.resolve(xywh.rect.height, referenceBox.height).cast<f64>();

            Math::Radiif resolvedRadii = _resolveRadii(resolver, xywh.borderRadius, referenceBox);

            result.rect(resolvedRect.offset(referenceBox.xy.cast<f64>()), resolvedRadii);

            return makeRc<Scene::Clip>(content, result);
        },
        [&](Rect const& rect) {
            Math::Insetsf resolvedInsets;
            resolvedInsets.top = resolver.resolve(rect.insets.top, referenceBox.height).cast<f64>();
            resolvedInsets.end = resolver.resolve(rect.insets.end, referenceBox.width).cast<f64>();
            resolvedInsets.bottom = resolver.resolve(rect.insets.bottom, referenceBox.height).cast<f64>();
            resolvedInsets.start = resolver.resolve(rect.insets.start, referenceBox.width).cast<f64>();

            Math::Radiif resolvedRadii = _resolveRadii(resolver, rect.borderRadius, referenceBox);

            auto resultBox = referenceBox.cast<f64>();
            resultBox.width = max(resolvedInsets.end - resolvedInsets.start, 0);
            resultBox.height = max(resolvedInsets.bottom - resolvedInsets.top, 0);
            resultBox.x += resolvedInsets.start;
            resultBox.y += resolvedInsets.top;

            result.rect(resultBox, resolvedRadii);

            return makeRc<Scene::Clip>(content, result);
        },
        [&](Ellipse const& ellipse) {
            auto center = _resolveBackgroundPosition(resolver, ellipse.position, referenceBox);

            f64 rx;
            if (ellipse.rx.is<Keywords::ClosestSide>()) {
                rx = min(Math::abs(referenceBox.width.cast<f64>() - center.x), center.x);
            } else if (ellipse.rx.is<Keywords::FarthestSide>()) {
                rx = max(Math::abs(referenceBox.width.cast<f64>() - center.x), center.x);
            } else {
                rx = resolver.resolve(
                                 ellipse.rx.unwrap<CalcValue<PercentOr<Length>>>(),
                                 referenceBox.width
                )
                         .cast<f64>();
            }

            f64 ry;
            if (ellipse.ry.is<Keywords::ClosestSide>()) {
                ry = min(Math::abs(referenceBox.height.cast<f64>() - center.y), center.y);
            } else if (ellipse.ry.is<Keywords::FarthestSide>()) {
                ry = max(Math::abs(referenceBox.height.cast<f64>() - center.y), center.y);
            } else {
                ry = resolver.resolve(
                                 ellipse.ry.unwrap<CalcValue<PercentOr<Length>>>(),
                                 referenceBox.height
                )
                         .cast<f64>();
            }
            result.ellipse(Math::Ellipsef(center + referenceBox.xy.cast<f64>(), Math::Vec2f(rx, ry)));

            return makeRc<Scene::Clip>(content, result);
        },
        [&](Path const& path) {
            result.path(path.path);
            result.offset(referenceBox.xy.cast<f64>());
            return makeRc<Scene::Clip>(content, result, path.fillRule);
        },
    });
}

} // namespace Vaev::Paint
