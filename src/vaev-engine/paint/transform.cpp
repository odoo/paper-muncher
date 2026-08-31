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
Vec2Au resolveTransformOrigin(TransformOrigin origin, RectAu referenceBox) {
    Layout::Resolver resolver{};

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
        [&](Calc<PercentOr<Length>> value) {
            return referenceBox.start() + resolver.resolve(value, referenceBox.width);
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
        [&](Calc<PercentOr<Length>> value) {
            return referenceBox.top() + resolver.resolve(value, referenceBox.height);
        }
    );

    return {x, y};
}

// https://www.w3.org/TR/css-transforms-1/#transform-property
Math::Trans2f resolveTransform(Slice<TransformFunction> transforms, RectAu referenceBox, Vec2Au origin) {
    Layout::Resolver resolver{};

    auto result = Math::Trans2f::translate(
        origin.cast<f64>()
    );

    for (auto const& transform : transforms) {
        auto trans = transform.visit(
            [&](MatrixTransform const& t) {
                return Math::Trans2f{
                    resolver.resolve(t.values[0]),
                    resolver.resolve(t.values[1]),
                    resolver.resolve(t.values[2]),
                    resolver.resolve(t.values[3]),
                    resolver.resolve(t.values[4]),
                    resolver.resolve(t.values[5]),
                };
            },
            [&](TranslateTransform const& t) {
                return Math::Trans2f::translate({
                    resolver.resolve(t.x, referenceBox.width).cast<f64>(),
                    resolver.resolve(t.y, referenceBox.height).cast<f64>(),
                });
            },
            [&](ScaleTransform const& t) {
                return Math::Trans2f::scale({
                    resolver.resolve(t.x),
                    resolver.resolve(t.y),
                });
            },
            [&](RotateTransform const& t) {
                return Math::Trans2f::rotate(resolver.resolve(t.value).value());
            },
            [&](SkewTransform const& t) {
                return Math::Trans2f::skew({
                    Math::tan(resolver.resolve(t.x).value()),
                    Math::tan(resolver.resolve(t.y).value()),
                });
            },
            [&](SkewXTransform const& t) {
                return Math::Trans2f::skew({
                    Math::tan(resolver.resolve(t.value).value()),
                    0,
                });
            },
            [&](SkewYTransform const& t) {
                return Math::Trans2f::skew({
                    0,
                    Math::tan(resolver.resolve(t.value).value()),
                });
            }
        );

        result = trans.multiply(result);
    }

    return Math::Trans2f::translate(-origin.cast<f64>()).multiply(result);
}

void applyTransform(Rc<Layout::Fragment>& fragment, Gfx::Canvas& g, Math::Rectf viewBox) {
    auto const& transform = *fragment->style().transform;
    auto referenceBox = resolveTransformReference(fragment, transform.box, viewBox.cast<Au>());
    auto origin = resolveTransformOrigin(transform.origin, referenceBox);
    auto t = resolveTransform(transform.transform.unwrap<Vec<TransformFunction>>(), referenceBox, origin);
    g.transform(t);
}

} // namespace Vaev::Paint
