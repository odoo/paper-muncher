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

Math::Radiif resolveRadii(Math::Radii<Calc<Au, Percent>> const& baseRadii, RectAu const& referenceBox) {
    Math::Radiif radii;
    radii.a = resolve(baseRadii.a, referenceBox.height).cast<f64>();
    radii.b = resolve(baseRadii.b, referenceBox.width).cast<f64>();
    radii.c = resolve(baseRadii.c, referenceBox.width).cast<f64>();
    radii.d = resolve(baseRadii.d, referenceBox.height).cast<f64>();
    radii.e = resolve(baseRadii.e, referenceBox.height).cast<f64>();
    radii.f = resolve(baseRadii.f, referenceBox.width).cast<f64>();
    radii.g = resolve(baseRadii.g, referenceBox.width).cast<f64>();
    radii.h = resolve(baseRadii.h, referenceBox.height).cast<f64>();
    return radii;
}

// https://drafts.csswg.org/css-shapes-1/#typedef-basic-shape
Tuple<Math::Path, Gfx::FillRule> resolvedClipShape(Computed<BasicShapeFunction> const& shape, RectAu referenceBox) {
    Math::Path path;
    Gfx::FillRule fillRule = {};

    shape.visit(
        [&](Computed<Polygon> const& polygon) {
            path.moveTo(
                referenceBox.xy.cast<f64>() +
                Math::Vec2f(
                    resolve(first(polygon.points).v0, referenceBox.width).cast<f64>(),
                    resolve(first(polygon.points).v1, referenceBox.height).cast<f64>()
                )
            );
            for (auto& point : next(polygon.points)) {
                path.lineTo(
                    referenceBox.xy.cast<f64>() +
                    Math::Vec2f(
                        resolve(point.v0, referenceBox.width).cast<f64>(),
                        resolve(point.v1, referenceBox.height).cast<f64>()
                    )
                );
            }
            fillRule = polygon.fillRule;
        },
        [&](Computed<Circle> const& circle) {
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
                radius = resolve(
                             circle.radius.expect<Calc<Au, Percent>>(),
                             Au(Math::sqrt(hSquared + wSquared) / Math::sqrt(2.0))
                )
                             .cast<f64>();
            }
            path.ellipse(Math::Ellipsef(center + referenceBox.xy.cast<f64>(), radius));
        },
        [&](Computed<Inset> const& inset) {
            Math::Insetsf resolved;
            resolved.start = resolve(inset.insets.start, referenceBox.width).cast<f64>();
            resolved.end = resolve(inset.insets.end, referenceBox.width).cast<f64>();
            resolved.top = resolve(inset.insets.top, referenceBox.height).cast<f64>();
            resolved.bottom = resolve(inset.insets.bottom, referenceBox.height).cast<f64>();

            Math::Radiif resolvedRadii = resolveRadii(inset.borderRadius, referenceBox);

            path.rect(referenceBox.cast<f64>().shrink(resolved), resolvedRadii);
        },
        [&](Computed<Xywh> const& xywh) {
            Math::Rectf resolvedRect;
            resolvedRect.x = resolve(xywh.rect.x, referenceBox.width).cast<f64>();
            resolvedRect.y = resolve(xywh.rect.y, referenceBox.height).cast<f64>();
            resolvedRect.width = resolve(xywh.rect.width, referenceBox.width).cast<f64>();
            resolvedRect.height = resolve(xywh.rect.height, referenceBox.height).cast<f64>();

            Math::Radiif resolvedRadii = resolveRadii(xywh.borderRadius, referenceBox);

            path.rect(resolvedRect.offset(referenceBox.xy.cast<f64>()), resolvedRadii);
        },
        [&](Computed<Rect> const& rect) {
            Math::Insetsf resolvedInsets;
            resolvedInsets.top = resolve(rect.insets.top, referenceBox.height).cast<f64>();
            resolvedInsets.end = resolve(rect.insets.end, referenceBox.width).cast<f64>();
            resolvedInsets.bottom = resolve(rect.insets.bottom, referenceBox.height).cast<f64>();
            resolvedInsets.start = resolve(rect.insets.start, referenceBox.width).cast<f64>();

            Math::Radiif resolvedRadii = resolveRadii(rect.borderRadius, referenceBox);

            auto resultBox = referenceBox.cast<f64>();
            resultBox.width = max(resolvedInsets.end - resolvedInsets.start, 0);
            resultBox.height = max(resolvedInsets.bottom - resolvedInsets.top, 0);
            resultBox.x += resolvedInsets.start;
            resultBox.y += resolvedInsets.top;

            path.rect(resultBox, resolvedRadii);
        },
        [&](Computed<Ellipse> const& ellipse) {
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
                rx = resolve(
                         ellipse.rx.expect<Calc<Au, Percent>>(),
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
                    resolve(
                        ellipse.ry.expect<Calc<Au, Percent>>(),
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
    auto& clip = fragment->style().clip->expect();
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
