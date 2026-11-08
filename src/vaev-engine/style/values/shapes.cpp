module;

#include <karm/macros>

export module Vaev.Engine:style.values.shapes;

import Karm.Core;

import :css.values.shapes;
import :style.values.backgrounds;
import :style.values.generic;
import :style.values.numeric;
import :values.unitValue;

using namespace Karm;

namespace Vaev::Style {

export using ShapeRadius = Css::ShapeRadiusT<PixelPercentage>;

export using Circle = Css::CircleT<PixelPercentage, Position>;

export Circle toComputed(Css::Circle const& value, LengthContext auto const& ctx) {
    return {toComputed(value.radius, ctx), toComputed(value.position, ctx)};
}

export Css::Circle toSpecified(Circle const& value) {
    return {toSpecified(value.radius), toSpecified(value.position)};
}

export using Ellipse = Css::EllipseT<PixelPercentage, Position>;

export Ellipse toComputed(Css::Ellipse const& value, LengthContext auto const& ctx) {
    return {toComputed(value.rx, ctx), toComputed(value.ry, ctx), toComputed(value.position, ctx)};
}

export Css::Ellipse toSpecified(Ellipse const& value) {
    return {toSpecified(value.rx), toSpecified(value.ry), toSpecified(value.position)};
}

export using Inset = Css::InsetT<PixelPercentage>;

export Inset toComputed(Css::Inset const& value, LengthContext auto const& ctx) {
    return {toComputed(value.insets, ctx), toComputed(value.borderRadius, ctx)};
}

export Css::Inset toSpecified(Inset const& value) {
    return {toSpecified(value.insets), toSpecified(value.borderRadius)};
}

export using Polygon = Css::PolygonT<PixelPercentage>;

export Polygon toComputed(Css::Polygon const& value, LengthContext auto const& ctx) {
    Polygon result{value.fillRule, {}};
    for (auto const& [x, y] : value.points)
        result.points.emplaceBack(toComputed(x, ctx), toComputed(y, ctx));
    return result;
}

export Css::Polygon toSpecified(Polygon const& value) {
    Css::Polygon result{value.fillRule, {}};
    for (auto const& [x, y] : value.points)
        result.points.emplaceBack(toSpecified(x), toSpecified(y));
    return result;
}

export using Rect = Css::RectT<PixelPercentage>;

export Rect toComputed(Css::Rect const& value, LengthContext auto const& ctx) {
    return {toComputed(value.insets, ctx), toComputed(value.borderRadius, ctx)};
}

export Css::Rect toSpecified(Rect const& value) {
    return {toSpecified(value.insets), toSpecified(value.borderRadius)};
}

export using Xywh = Css::XywhT<PixelPercentage>;

export Xywh toComputed(Css::Xywh const& value, LengthContext auto const& ctx) {
    return {
        toComputed(value.x, ctx),
        toComputed(value.y, ctx),
        toComputed(value.width, ctx),
        toComputed(value.height, ctx),
        toComputed(value.borderRadius, ctx),
    };
}

export Css::Xywh toSpecified(Xywh const& value) {
    return {
        toSpecified(value.x),
        toSpecified(value.y),
        toSpecified(value.width),
        toSpecified(value.height),
        toSpecified(value.borderRadius),
    };
}

export using BasicShapeFunction = Css::BasicShapeFunctionT<PixelPercentage, Position>;

export using BasicShape = Css::BasicShapeT<PixelPercentage, Position>;

export BasicShape toComputed(Css::BasicShape const& value, LengthContext auto const& ctx) {
    return {
        value.shape.map([&](Css::BasicShapeFunction const& shape) {
            return shape.visit(
                [](Css::Path const& path) -> BasicShapeFunction {
                    return path;
                },
                [&](auto const& s) -> BasicShapeFunction {
                    return toComputed(s, ctx);
                }
            );
        }),
        value.referenceBox,
    };
}

export Css::BasicShape toSpecified(BasicShape const& value) {
    return {
        value.shape.map([](BasicShapeFunction const& shape) {
            return shape.visit(
                [](Css::Path const& path) -> Css::BasicShapeFunction {
                    return path;
                },
                [](auto const& s) -> Css::BasicShapeFunction {
                    return toSpecified(s);
                }
            );
        }),
        value.referenceBox,
    };
}

} // namespace Vaev::Style
