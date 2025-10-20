module Vaev.Engine;

import Karm.Scene;
import Karm.Gfx;
import Karm.Math;
import Karm.Core;

import :layout.base;
import :layout.values;

using namespace Karm;

namespace Vaev::Paint {

// https://www.w3.org/TR/css-transforms-1/#transform-box
static RectAu _resolveTransformReferenceCSS(Layout::Metrics const& metrics, TransformBox box) {
    // For elements with associated CSS layout box, the used value for fill-box
    // is content-box and for stroke-box and view-box is border-box.
    return box.visit(
        Visitor{
            [&](Keywords::ContentBox const&) {
                return metrics.contentBox();
            },
            [&](Keywords::BorderBox const&) {
                return metrics.borderBox();
            },
            [&](Keywords::FillBox const&) {
                return metrics.contentBox();
            },
            [&](Keywords::StrokeBox const&) {
                return metrics.borderBox();
            },
            [&](Keywords::ViewBox const&) {
                return metrics.borderBox();
            },
        }
    );
}

static Vec2Au _resolveTransformOrigin(RectAu referenceBox, TransformOrigin origin) {
    Layout::Resolver resolver{};

    auto x = origin.xOffset.visit(
        Visitor{
            [&](Keywords::Left) {
                return referenceBox.start();
            },
            [&](Keywords::Right) {
                return referenceBox.end();
            },
            [&](Keywords::Center) {
                return referenceBox.center().x;
            },
            [&](CalcValue<PercentOr<Length>> value) {
                return referenceBox.start() + resolver.resolve(value, referenceBox.width);
            }
        }
    );

    auto y = origin.yOffset.visit(
        Visitor{
            [&](Keywords::Top) {
                return referenceBox.top();
            },
            [&](Keywords::Bottom) {
                return referenceBox.bottom();
            },
            [&](Keywords::Center) {
                return referenceBox.center().y;
            },
            [&](CalcValue<PercentOr<Length>> value) {
                return referenceBox.top() + resolver.resolve(value, referenceBox.height);
            }
        }
    );

    return {x, y};
}

static Math::Trans2f _resolveTransform(RectAu referenceBox, Vec2Au origin, Slice<TransformFunction> transforms) {
    auto result = Math::Trans2f::translate(
        origin.cast<f64>()
    );
    Layout::Resolver resolver{};

    for (auto const& transform : transforms) {
        auto trans = transform.visit(
            Visitor{
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
            }
        );

        result = trans.multiply(result);
    }

    return Math::Trans2f::translate(-origin.cast<f64>()).multiply(result);
}

Rc<Scene::Node> _applyTransform(Style::TransformProps const& transform, RectAu referenceBox, Rc<Scene::Node> content) {
    auto origin = _resolveTransformOrigin(referenceBox, transform.origin);
    auto const& transformFunctions = transform.transform.unwrap<Vec<TransformFunction>>();
    auto trans = _resolveTransform(referenceBox, origin, transformFunctions);
    return makeRc<Scene::Transform>(content, trans);
}

Rc<Scene::Node> _applyTransform(Layout::Frag const& frag, Rc<Scene::Node> content) {
    auto const& transform = *frag.style().transform;
    auto referenceBox = _resolveTransformReferenceCSS(frag.metrics, transform.box);
    return _applyTransform(transform, referenceBox, content);
}

} // namespace Vaev::Paint
