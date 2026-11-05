export module Vaev.Engine:paint.transform;

import Karm.Core;
import Karm.Math;
import :layout.values;

namespace Vaev::Paint {

// https://www.w3.org/TR/css-transforms-1/#transform-box
RectAu resolveTransformReference(Rc<Layout::Fragment>& fragment, Css::TransformBox box, RectAu viewBox) {
    // For SVG elements without associated CSS layout box, the used value
    // for content-box is fill-box and for border-box is stroke-box;
    // conversely for elements with one, fill-box is content-box and both
    // stroke-box and view-box are border-box.
    return box.visit(
        [&](Keywords::ContentBox const&) {
            if (not fragment->hasCssLayoutBox())
                return fragment->objectBoundingBox();
            return fragment->contentBox();
        },
        [&](Keywords::BorderBox const&) {
            if (not fragment->hasCssLayoutBox())
                return fragment->strokeBoundingBox();
            return fragment->borderBox();
        },
        [&](Keywords::FillBox const&) {
            if (fragment->hasCssLayoutBox())
                return fragment->contentBox();
            return fragment->objectBoundingBox();
        },
        [&](Keywords::StrokeBox const&) {
            if (fragment->hasCssLayoutBox())
                return fragment->borderBox();
            return fragment->strokeBoundingBox();
        },
        [&](Keywords::ViewBox const&) {
            if (fragment->hasCssLayoutBox())
                return fragment->borderBox();
            return viewBox;
        }
    );
}

// https://www.w3.org/TR/css-transforms-1/#transform-origin-property
Vec2Au resolveTransformOrigin(Style::Position const& origin, RectAu referenceBox) {
    return {
        referenceBox.start() + Layout::resolve(origin.x, referenceBox.width),
        referenceBox.top() + Layout::resolve(origin.y, referenceBox.height),
    };
}

// https://www.w3.org/TR/css-transforms-1/#transform-property
Math::Trans2f resolveTransform(Slice<Style::TransformFunction> transforms, RectAu referenceBox, Vec2Au origin) {
    auto result = Math::Trans2f::translate(
        origin.cast<f64>()
    );

    for (auto const& transform : transforms) {
        auto trans = transform.visit(
            [&](Style::MatrixTransform const& t) {
                return Math::Trans2f{
                    t.values[0],
                    t.values[1],
                    t.values[2],
                    t.values[3],
                    t.values[4],
                    t.values[5],
                };
            },
            [&](Style::TranslateTransform const& t) {
                return Math::Trans2f::translate({
                    Layout::resolve(t.x, referenceBox.width).cast<f64>(),
                    Layout::resolve(t.y, referenceBox.height).cast<f64>(),
                });
            },
            [&](Style::ScaleTransform const& t) {
                return Math::Trans2f::scale({t.x, t.y});
            },
            [&](Style::RotateTransform const& t) {
                return Math::Trans2f::rotate(t.value.value());
            },
            [&](Style::SkewTransform const& t) {
                return Math::Trans2f::skew({
                    Math::tan(t.x.value()),
                    Math::tan(t.y.value()),
                });
            },
            [&](Style::SkewXTransform const& t) {
                return Math::Trans2f::skew({
                    Math::tan(t.value.value()),
                    0,
                });
            },
            [&](Style::SkewYTransform const& t) {
                return Math::Trans2f::skew({
                    0,
                    Math::tan(t.value.value()),
                });
            }
        );

        result = trans.multiply(result);
    }

    return Math::Trans2f::translate(-origin.cast<f64>()).multiply(result);
}

Math::Trans2f resolveTransform(Rc<Layout::Fragment>& fragment, Math::Rectf viewBox) {
    auto const& transform = *fragment->style().transform;
    auto referenceBox = resolveTransformReference(fragment, transform.box, viewBox.cast<Au>());
    auto origin = resolveTransformOrigin(transform.origin, referenceBox);
    return resolveTransform(transform.transform.expect<Style::TransformList>(), referenceBox, origin);
}

void applyTransform(Rc<Layout::Fragment>& fragment, Gfx::Canvas& g, Math::Rectf viewBox) {
    g.transform(resolveTransform(fragment, viewBox));
}

} // namespace Vaev::Paint
