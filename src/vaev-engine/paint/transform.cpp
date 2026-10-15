export module Vaev.Engine:paint.transform;

import Karm.Core;
import Karm.Math;
import :layout.values;

namespace Vaev::Paint {

// https://www.w3.org/TR/css-transforms-1/#transform-box
RectAu resolveTransformReference(Rc<Layout::Fragment>& fragment, TransformBox box, RectAu viewBox) {
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
Vec2Au resolveTransformOrigin(Computed<TransformOrigin> const& origin, RectAu referenceBox) {
    auto x = origin.xOffset.visit(
        [&](Keywords::Left) {
            return referenceBox.start();
        },
        [&](Keywords::Right) {
            return referenceBox.end();
        },
        [&](Keywords::Center) {
            return referenceBox.center().x;
        },
        [&](Calc<Au, Percent> const& value) {
            return referenceBox.start() + resolve(value, referenceBox.width);
        }
    );

    auto y = origin.yOffset.visit(
        [&](Keywords::Top) {
            return referenceBox.top();
        },
        [&](Keywords::Bottom) {
            return referenceBox.bottom();
        },
        [&](Keywords::Center) {
            return referenceBox.center().y;
        },
        [&](Calc<Au, Percent> const& value) {
            return referenceBox.top() + resolve(value, referenceBox.height);
        }
    );

    return {x, y};
}

// https://www.w3.org/TR/css-transforms-1/#transform-property
Math::Trans2f resolveTransform(Slice<Computed<TransformFunction>> transforms, RectAu referenceBox, Vec2Au origin) {
    auto result = Math::Trans2f::translate(
        origin.cast<f64>()
    );

    for (auto const& transform : transforms) {
        auto trans = transform.visit(
            [&](MatrixTransform const& t) {
                return Math::Trans2f{
                    resolve(t.values[0]),
                    resolve(t.values[1]),
                    resolve(t.values[2]),
                    resolve(t.values[3]),
                    resolve(t.values[4]),
                    resolve(t.values[5]),
                };
            },
            [&](Computed<TranslateTransform> const& t) {
                return Math::Trans2f::translate({
                    resolve(t.x, referenceBox.width).cast<f64>(),
                    resolve(t.y, referenceBox.height).cast<f64>(),
                });
            },
            [&](ScaleTransform const& t) {
                return Math::Trans2f::scale({
                    resolve(t.x),
                    resolve(t.y),
                });
            },
            [&](RotateTransform const& t) {
                return Math::Trans2f::rotate(resolve(t.value).value());
            },
            [&](SkewTransform const& t) {
                return Math::Trans2f::skew({
                    Math::tan(resolve(t.x).value()),
                    Math::tan(resolve(t.y).value()),
                });
            },
            [&](SkewXTransform const& t) {
                return Math::Trans2f::skew({
                    Math::tan(resolve(t.value).value()),
                    0,
                });
            },
            [&](SkewYTransform const& t) {
                return Math::Trans2f::skew({
                    0,
                    Math::tan(resolve(t.value).value()),
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
    return resolveTransform(transform.transform.expect<Computed<TransformList>>(), referenceBox, origin);
}

void applyTransform(Rc<Layout::Fragment>& fragment, Gfx::Canvas& g, Math::Rectf viewBox) {
    g.transform(resolveTransform(fragment, viewBox));
}

} // namespace Vaev::Paint
