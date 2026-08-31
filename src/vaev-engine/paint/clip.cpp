export module Vaev.Engine:paint.clip;

import Karm.Core;
import Karm.Math;
import :layout.values;
import :paint.background;

namespace Vaev::Paint {

Pair<RectAu, RadiiAu> resolveClipGeometryBox(Rc<Layout::Fragment>& fragment, GeometryBox box, RectAu viewBox) {
    RadiiAu metricsRadii = 0_au;
    if (auto boxFragment = fragment.is<Layout::BoxFragment>())
        metricsRadii = boxFragment->metrics.radii;

    return box.visit(
        [&](Keywords::BorderBox const&) -> Pair<RectAu, RadiiAu> {
            return {fragment->borderBox(), metricsRadii};
        },
        [&](Keywords::PaddingBox const&) -> Pair<RectAu, RadiiAu> {
            return {fragment->paddingBox(), {0_au}};
        },
        [&](Keywords::ContentBox const&) -> Pair<RectAu, RadiiAu> {
            return {fragment->contentBox(), {0_au}};
        },
        [&](Keywords::MarginBox const&) -> Pair<RectAu, RadiiAu> {
            return {fragment->marginBox(), {0_au}};
        },
        [&](Keywords::FillBox const&) -> Pair<RectAu, RadiiAu> {
            if (fragment->hasCssLayoutBox())
                return {fragment->contentBox(), {0_au}};
            return {fragment->objectBoundingBox(), {0_au}};
        },
        [&](Keywords::StrokeBox const&) -> Pair<RectAu, RadiiAu> {
            if (fragment->hasCssLayoutBox())
                return {fragment->borderBox(), metricsRadii};
            return {fragment->strokeBoundingBox(), metricsRadii};
        },
        [&](Keywords::ViewBox const&) -> Pair<RectAu, RadiiAu> {
            if (fragment->hasCssLayoutBox())
                return {fragment->borderBox(), metricsRadii};
            return {viewBox, {0_au}};
        }
    );
}

Math::Radiif resolveRadii(Math::Radii<Calc<PercentOr<Length>>> const& baseRadii, RectAu const& referenceBox) {
    Math::Radiif radii;
    Layout::Resolver resolver;
    radii.a = resolver.resolve(baseRadii.a, referenceBox.height).cast<f64>();
    radii.b = resolver.resolve(baseRadii.b, referenceBox.width).cast<f64>();
    radii.c = resolver.resolve(baseRadii.c, referenceBox.width).cast<f64>();
    radii.d = resolver.resolve(baseRadii.d, referenceBox.height).cast<f64>();
    radii.e = resolver.resolve(baseRadii.e, referenceBox.height).cast<f64>();
    radii.f = resolver.resolve(baseRadii.f, referenceBox.width).cast<f64>();
    radii.g = resolver.resolve(baseRadii.g, referenceBox.width).cast<f64>();
    radii.h = resolver.resolve(baseRadii.h, referenceBox.height).cast<f64>();
    return radii;
}

// https://drafts.csswg.org/css-shapes-1/#typedef-basic-shape
Tuple<Math::Path, Gfx::FillRule> resolvedClipShape(BasicShapeFunction const& shape, RectAu referenceBox) {
    Math::Path path;
    Gfx::FillRule fillRule = {};
    Layout::Resolver resolver;

    shape.visit(
        [&](Polygon const& polygon) {
            path.moveTo(
                referenceBox.xy.cast<f64>() +
                Math::Vec2f(
                    resolver.resolve(first(polygon.points).v0, referenceBox.width).cast<f64>(),
                    resolver.resolve(first(polygon.points).v1, referenceBox.height).cast<f64>()
                )
            );
            for (auto& point : next(polygon.points)) {
                path.lineTo(
                    referenceBox.xy.cast<f64>() +
                    Math::Vec2f(
                        resolver.resolve(point.v0, referenceBox.width).cast<f64>(),
                        resolver.resolve(point.v1, referenceBox.height).cast<f64>()
                    )
                );
            }
            fillRule = polygon.fillRule;
        },
        [&](Circle const& circle) {
            auto center = resolveBackgroundPosition(
                circle.position,
                referenceBox
            );

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
                radius = resolver
                             .resolve(
                                 circle.radius.unwrap<Calc<PercentOr<Length>>>(),
                                 Au(Math::sqrt(hSquared + wSquared) / Math::sqrt(2.0))
                             )
                             .cast<f64>();
            }
            path.ellipse(Math::Ellipsef(center + referenceBox.xy.cast<f64>(), radius));
        },
        [&](Inset const& inset) {
            Math::Insetsf resolved;
            resolved.start = resolver.resolve(inset.insets.start, referenceBox.width).cast<f64>();
            resolved.end = resolver.resolve(inset.insets.end, referenceBox.width).cast<f64>();
            resolved.top = resolver.resolve(inset.insets.top, referenceBox.height).cast<f64>();
            resolved.bottom = resolver.resolve(inset.insets.bottom, referenceBox.height).cast<f64>();

            Math::Radiif resolvedRadii = resolveRadii(inset.borderRadius, referenceBox);

            path.rect(referenceBox.cast<f64>().shrink(resolved), resolvedRadii);
        },
        [&](Xywh const& xywh) {
            Math::Rectf resolvedRect;
            resolvedRect.x = resolver.resolve(xywh.rect.x, referenceBox.width).cast<f64>();
            resolvedRect.y = resolver.resolve(xywh.rect.y, referenceBox.height).cast<f64>();
            resolvedRect.width = resolver.resolve(xywh.rect.width, referenceBox.width).cast<f64>();
            resolvedRect.height = resolver.resolve(xywh.rect.height, referenceBox.height).cast<f64>();

            Math::Radiif resolvedRadii = resolveRadii(xywh.borderRadius, referenceBox);

            path.rect(resolvedRect.offset(referenceBox.xy.cast<f64>()), resolvedRadii);
        },
        [&](Rect const& rect) {
            Math::Insetsf resolvedInsets;
            resolvedInsets.top = resolver.resolve(rect.insets.top, referenceBox.height).cast<f64>();
            resolvedInsets.end = resolver.resolve(rect.insets.end, referenceBox.width).cast<f64>();
            resolvedInsets.bottom = resolver.resolve(rect.insets.bottom, referenceBox.height).cast<f64>();
            resolvedInsets.start = resolver.resolve(rect.insets.start, referenceBox.width).cast<f64>();

            Math::Radiif resolvedRadii = resolveRadii(rect.borderRadius, referenceBox);

            auto resultBox = referenceBox.cast<f64>();
            resultBox.width = max(resolvedInsets.end - resolvedInsets.start, 0);
            resultBox.height = max(resolvedInsets.bottom - resolvedInsets.top, 0);
            resultBox.x += resolvedInsets.start;
            resultBox.y += resolvedInsets.top;

            path.rect(resultBox, resolvedRadii);
        },
        [&](Ellipse const& ellipse) {
            auto center = resolveBackgroundPosition(
                ellipse.position,
                referenceBox
            );

            f64 rx;
            if (ellipse.rx.is<Keywords::ClosestSide>()) {
                rx = min(Math::abs(referenceBox.width.cast<f64>() - center.x), center.x);
            } else if (ellipse.rx.is<Keywords::FarthestSide>()) {
                rx = max(Math::abs(referenceBox.width.cast<f64>() - center.x), center.x);
            } else {
                rx = resolver.resolve(
                                 ellipse.rx.unwrap<Calc<PercentOr<Length>>>(),
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
                ry =
                    resolver.resolve(
                                ellipse.ry.unwrap<Calc<PercentOr<Length>>>(),
                                referenceBox.height
                    )
                        .cast<f64>();
            }
            path.ellipse(
                Math::Ellipsef(center + referenceBox.xy.cast<f64>(), Math::Vec2f(rx, ry))
            );
        },
        [&](Path const& p) {
            path.path(p.path);
            path.offset(referenceBox.xy.cast<f64>());
            fillRule = p.fillRule;
        }
    );

    return {path, fillRule};
}

void applyClip(Rc<Layout::Fragment>& fragment, Gfx::Canvas& g, Math::Rectf viewBox) {
    auto& clip = fragment->style().clip->unwrap();
    auto [referenceBox, radii] = resolveClipGeometryBox(fragment, clip.referenceBox, viewBox.cast<Au>());

    if (auto& [c] = clip.shape) {
        auto [path, fillRule] = resolvedClipShape(c, referenceBox);
        g.beginPath();
        g.path(path);
        g.clip(fillRule);
    } else {
        g.beginPath();
        g.rect(referenceBox.round().cast<f64>(), radii.cast<f64>());
        g.clip();
    }
}

} // namespace Vaev::Paint
