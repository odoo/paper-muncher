export module Vaev.Engine:style.values.shapes;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :css.values.backgrounds;
import :css.values.shapes;
import :style.values.backgrounds;
import :style.values.generic;
import :style.values.numeric;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-masking/#typedef-geometry-box
export using ShapeBox = Css::ShapeBox;
export using GeometryBox = Css::GeometryBox;
export using FillRule = Css::FillRule;

// https://drafts.csswg.org/css-shapes/#funcdef-basic-shape-path
export using Path = Css::Path;

export using ShapeRadius = Union<
    PxPercent,
    Keywords::ClosestSide,
    Keywords::FarthestSide>;

export struct Circle {
    ShapeRadius radius = Keywords::CLOSEST_SIDE;
    Position position = Position::center();

    void repr(Io::Emit& e) const {
        e("(circle {} {})", radius, position);
    }
};

export Circle toComputed(Css::Circle const& value, LengthContext auto const& ctx) {
    return {toComputed(value.radius, ctx), toComputed(value.position, ctx)};
}

export Css::Circle toSpecified(Circle const& value) {
    return {toSpecified(value.radius), toSpecified(value.position)};
}

export struct Ellipse {
    ShapeRadius rx = Keywords::CLOSEST_SIDE;
    ShapeRadius ry = Keywords::CLOSEST_SIDE;
    Position position = Position::center();

    void repr(Io::Emit& e) const {
        e("(ellipse {} {} {})", rx, ry, position);
    }
};

export Ellipse toComputed(Css::Ellipse const& value, LengthContext auto const& ctx) {
    return {toComputed(value.rx, ctx), toComputed(value.ry, ctx), toComputed(value.position, ctx)};
}

export Css::Ellipse toSpecified(Ellipse const& value) {
    return {toSpecified(value.rx), toSpecified(value.ry), toSpecified(value.position)};
}

export struct Inset {
    Math::Insets<PxPercent> insets;
    Math::Radii<PxPercent> borderRadius;

    void repr(Io::Emit& e) const {
        e("(inset {} ", insets);
        Css::reprRadii(e, borderRadius);
        e(")");
    }
};

export Inset toComputed(Css::Inset const& value, LengthContext auto const& ctx) {
    return {toComputed(value.insets, ctx), toComputed(value.borderRadius, ctx)};
}

export Css::Inset toSpecified(Inset const& value) {
    return {toSpecified(value.insets), toSpecified(value.borderRadius)};
}

export struct Polygon {
    Gfx::FillRule fillRule = Gfx::FillRule::NONZERO;
    Vec<Pair<PxPercent>> points;

    void repr(Io::Emit& e) const {
        e("(polygon {} {})", fillRule, points);
    }
};

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

export struct Rect {
    Math::Insets<PxPercent> insets;
    Math::Radii<PxPercent> borderRadius;

    void repr(Io::Emit& e) const {
        e("(rect {} ", insets);
        Css::reprRadii(e, borderRadius);
        e(")");
    }
};

export Rect toComputed(Css::Rect const& value, LengthContext auto const& ctx) {
    return {toComputed(value.insets, ctx), toComputed(value.borderRadius, ctx)};
}

export Css::Rect toSpecified(Rect const& value) {
    return {toSpecified(value.insets), toSpecified(value.borderRadius)};
}

export struct Xywh {
    PxPercent x;
    PxPercent y;
    PxPercent width;
    PxPercent height;
    Math::Radii<PxPercent> borderRadius;

    void repr(Io::Emit& e) const {
        e("(xywh {} {} {} {} ", x, y, width, height);
        Css::reprRadii(e, borderRadius);
        e(")");
    }
};

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

export using BasicShapeFunction = Union<
    Circle, Ellipse, Inset, Path, Polygon, Rect, Xywh>;

export struct BasicShape {
    Opt<BasicShapeFunction> shape;
    GeometryBox referenceBox = Keywords::BORDER_BOX;

    void repr(Io::Emit& e) const {
        e("(basic-shape {} {})", shape, referenceBox);
    }
};

export BasicShape toComputed(Css::BasicShape const& value, LengthContext auto const& ctx) {
    return {
        value.shape.map([&](Css::BasicShapeFunction const& shape) {
            return shape.visit(
                [](Path const& path) -> BasicShapeFunction {
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
                [](Path const& path) -> Css::BasicShapeFunction {
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
