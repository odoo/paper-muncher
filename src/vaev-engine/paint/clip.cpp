export module Vaev.Engine:paint.clip;

import Karm.Core;
import Karm.Math;
import :layout.values;
import :paint.background;

namespace Vaev::Paint {

Pair<Layout::RectAu, Layout::RadiiAu> resolveClipGeometryBox(Rc<Layout::Fragment>& fragment, Style::GeometryBox box, Layout::RectAu viewBox) {
    Layout::RadiiAu metricsRadii = 0_au;
    if (auto boxFragment = fragment.is<Layout::BoxFragment>())
        metricsRadii = boxFragment->metrics.radii;

    return box.visit(
        [&](Style::Keywords::BorderBox const&) -> Pair<Layout::RectAu, Layout::RadiiAu> {
            return {fragment->borderBox(), metricsRadii};
        },
        [&](Style::Keywords::PaddingBox const&) -> Pair<Layout::RectAu, Layout::RadiiAu> {
            return {fragment->paddingBox(), {0_au}};
        },
        [&](Style::Keywords::ContentBox const&) -> Pair<Layout::RectAu, Layout::RadiiAu> {
            return {fragment->contentBox(), {0_au}};
        },
        [&](Style::Keywords::MarginBox const&) -> Pair<Layout::RectAu, Layout::RadiiAu> {
            return {fragment->marginBox(), {0_au}};
        },
        [&](Style::Keywords::FillBox const&) -> Pair<Layout::RectAu, Layout::RadiiAu> {
            if (fragment->hasCssLayoutBox())
                return {fragment->contentBox(), {0_au}};
            return {fragment->objectBoundingBox(), {0_au}};
        },
        [&](Style::Keywords::StrokeBox const&) -> Pair<Layout::RectAu, Layout::RadiiAu> {
            if (fragment->hasCssLayoutBox())
                return {fragment->borderBox(), metricsRadii};
            return {fragment->strokeBoundingBox(), metricsRadii};
        },
        [&](Style::Keywords::ViewBox const&) -> Pair<Layout::RectAu, Layout::RadiiAu> {
            if (fragment->hasCssLayoutBox())
                return {fragment->borderBox(), metricsRadii};
            return {viewBox, {0_au}};
        }
    );
}

Math::Radiif resolveRadii(Math::Radii<Style::PxPercent> const& baseRadii, Layout::RectAu const& referenceBox) {
    Math::Radiif radii;
    radii.a = Layout::resolve(baseRadii.a, referenceBox.height).cast<f64>();
    radii.b = Layout::resolve(baseRadii.b, referenceBox.width).cast<f64>();
    radii.c = Layout::resolve(baseRadii.c, referenceBox.width).cast<f64>();
    radii.d = Layout::resolve(baseRadii.d, referenceBox.height).cast<f64>();
    radii.e = Layout::resolve(baseRadii.e, referenceBox.height).cast<f64>();
    radii.f = Layout::resolve(baseRadii.f, referenceBox.width).cast<f64>();
    radii.g = Layout::resolve(baseRadii.g, referenceBox.width).cast<f64>();
    radii.h = Layout::resolve(baseRadii.h, referenceBox.height).cast<f64>();
    return radii;
}

// https://drafts.csswg.org/css-shapes-1/#typedef-basic-shape
Tuple<Math::Path, Gfx::FillRule> resolvedClipShape(Style::BasicShapeFunction const& shape, Layout::RectAu referenceBox) {
    Math::Path path;
    Gfx::FillRule fillRule = {};

    shape.visit(
        [&](Style::Polygon const& polygon) {
            path.moveTo(
                referenceBox.xy.cast<f64>() +
                Math::Vec2f(
                    Layout::resolve(first(polygon.points).v0, referenceBox.width).cast<f64>(),
                    Layout::resolve(first(polygon.points).v1, referenceBox.height).cast<f64>()
                )
            );
            for (auto& point : next(polygon.points)) {
                path.lineTo(
                    referenceBox.xy.cast<f64>() +
                    Math::Vec2f(
                        Layout::resolve(point.v0, referenceBox.width).cast<f64>(),
                        Layout::resolve(point.v1, referenceBox.height).cast<f64>()
                    )
                );
            }
            fillRule = polygon.fillRule;
        },
        [&](Style::Circle const& circle) {
            auto center = resolveBackgroundPosition(
                circle.position,
                referenceBox
            );

            f64 radius;
            if (circle.radius.is<Style::Keywords::ClosestSide>()) {
                radius = min(
                    Math::abs(referenceBox.width.cast<f64>() - center.x),
                    center.x,
                    center.y,
                    Math::abs(referenceBox.height.cast<f64>() - center.y)
                );
            } else if (circle.radius.is<Style::Keywords::FarthestSide>()) {
                radius = max(
                    Math::abs(referenceBox.width.cast<f64>() - center.x),
                    center.x,
                    center.y,
                    Math::abs(referenceBox.height.cast<f64>() - center.y)
                );
            } else {
                auto hSquared = Math::pow2(referenceBox.height.cast<f64>());
                auto wSquared = Math::pow2(referenceBox.width.cast<f64>());
                radius = Layout::resolve(
                                 circle.radius.expect<Style::PxPercent>(),
                                 Layout::Au(Math::sqrt(hSquared + wSquared) / Math::sqrt(2.0))
                             )
                             .cast<f64>();
            }
            path.ellipse(Math::Ellipsef(center + referenceBox.xy.cast<f64>(), radius));
        },
        [&](Style::Inset const& inset) {
            Math::Insetsf resolved;
            resolved.start = Layout::resolve(inset.insets.start, referenceBox.width).cast<f64>();
            resolved.end = Layout::resolve(inset.insets.end, referenceBox.width).cast<f64>();
            resolved.top = Layout::resolve(inset.insets.top, referenceBox.height).cast<f64>();
            resolved.bottom = Layout::resolve(inset.insets.bottom, referenceBox.height).cast<f64>();

            Math::Radiif resolvedRadii = resolveRadii(inset.borderRadius, referenceBox);

            path.rect(referenceBox.cast<f64>().shrink(resolved), resolvedRadii);
        },
        [&](Style::Xywh const& xywh) {
            Math::Rectf resolvedRect;
            resolvedRect.x = Layout::resolve(xywh.x, referenceBox.width).cast<f64>();
            resolvedRect.y = Layout::resolve(xywh.y, referenceBox.height).cast<f64>();
            resolvedRect.width = Layout::resolve(xywh.width, referenceBox.width).cast<f64>();
            resolvedRect.height = Layout::resolve(xywh.height, referenceBox.height).cast<f64>();

            Math::Radiif resolvedRadii = resolveRadii(xywh.borderRadius, referenceBox);

            path.rect(resolvedRect.offset(referenceBox.xy.cast<f64>()), resolvedRadii);
        },
        [&](Style::Rect const& rect) {
            Math::Insetsf resolvedInsets;
            resolvedInsets.top = Layout::resolve(rect.insets.top, referenceBox.height).cast<f64>();
            resolvedInsets.end = Layout::resolve(rect.insets.end, referenceBox.width).cast<f64>();
            resolvedInsets.bottom = Layout::resolve(rect.insets.bottom, referenceBox.height).cast<f64>();
            resolvedInsets.start = Layout::resolve(rect.insets.start, referenceBox.width).cast<f64>();

            Math::Radiif resolvedRadii = resolveRadii(rect.borderRadius, referenceBox);

            auto resultBox = referenceBox.cast<f64>();
            resultBox.width = max(resolvedInsets.end - resolvedInsets.start, 0);
            resultBox.height = max(resolvedInsets.bottom - resolvedInsets.top, 0);
            resultBox.x += resolvedInsets.start;
            resultBox.y += resolvedInsets.top;

            path.rect(resultBox, resolvedRadii);
        },
        [&](Style::Ellipse const& ellipse) {
            auto center = resolveBackgroundPosition(
                ellipse.position,
                referenceBox
            );

            f64 rx;
            if (ellipse.rx.is<Style::Keywords::ClosestSide>()) {
                rx = min(Math::abs(referenceBox.width.cast<f64>() - center.x), center.x);
            } else if (ellipse.rx.is<Style::Keywords::FarthestSide>()) {
                rx = max(Math::abs(referenceBox.width.cast<f64>() - center.x), center.x);
            } else {
                rx = Layout::resolve(
                                 ellipse.rx.expect<Style::PxPercent>(),
                                 referenceBox.width
                )
                         .cast<f64>();
            }

            f64 ry;
            if (ellipse.ry.is<Style::Keywords::ClosestSide>()) {
                ry = min(Math::abs(referenceBox.height.cast<f64>() - center.y), center.y);
            } else if (ellipse.ry.is<Style::Keywords::FarthestSide>()) {
                ry = max(Math::abs(referenceBox.height.cast<f64>() - center.y), center.y);
            } else {
                ry =
                    Layout::resolve(
                                ellipse.ry.expect<Style::PxPercent>(),
                                referenceBox.height
                    )
                        .cast<f64>();
            }
            path.ellipse(
                Math::Ellipsef(center + referenceBox.xy.cast<f64>(), Math::Vec2f(rx, ry))
            );
        },
        [&](Style::Path const& p) {
            path.path(p.path);
            path.offset(referenceBox.xy.cast<f64>());
            fillRule = p.fillRule;
        }
    );

    return {path, fillRule};
}

void applyClip(Rc<Layout::Fragment>& fragment, Gfx::Canvas& g, Math::Rectf viewBox) {
    auto& clip = fragment->style().clip->expect();
    auto [referenceBox, radii] = resolveClipGeometryBox(fragment, clip.referenceBox, viewBox.cast<Layout::Au>());

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
