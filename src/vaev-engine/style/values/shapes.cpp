export module Vaev.Engine:style.values.shapes;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :css.values.backgrounds;
import :css.values.shapes;
import :style.values.backgrounds;
import :style.values.base;
import :style.values.numeric;

using namespace Karm;

namespace Vaev::Style {

// https://drafts.csswg.org/css-masking/#typedef-geometry-box
export using ShapeBox = Css::ShapeBox;
export using GeometryBox = Css::GeometryBox;

// https://www.w3.org/TR/SVG2/painting.html#FillRuleProperty
export using FillRule = Gfx::FillRule;

export template <>
struct ValueComputer<Css::FillRule> {
    static FillRule compute(Css::FillRule const& value) {
        return value.visit(
            [](Keywords::Nonzero) {
                return FillRule::NONZERO;
            },
            [](Keywords::Evenodd) {
                return FillRule::EVENODD;
            }
        );
    }

    static Css::FillRule specify(FillRule value) {
        if (value == FillRule::EVENODD)
            return Keywords::EVENODD;
        return Keywords::NONZERO;
    }
};

// https://drafts.csswg.org/css-shapes/#funcdef-basic-shape-path
export struct Path {
    Math::Path path;
    FillRule fillRule = FillRule::NONZERO;

    void repr(Io::Emit& e) const {
        e("(path {} {})", fillRule, path);
    }
};

export template <>
struct ValueComputer<Css::Path> {
    static Path compute(Css::Path const& value) {
        return {value.path, computeValue(value.fillRule)};
    }

    static Css::Path specify(Path const& value) {
        return {value.path, specifyValue<Css::FillRule>(value.fillRule)};
    }
};

export using ShapeRadius = Union<
    PxPercent,
    Keywords::ClosestSide,
    Keywords::FarthestSide>;

// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-circle
export struct Circle {
    ShapeRadius radius = Keywords::CLOSEST_SIDE;
    Position position = Position::center();

    void repr(Io::Emit& e) const {
        e("(circle {} {})", radius, position);
    }
};

export template <>
struct ValueComputer<Css::Circle> {
    static Circle compute(Css::Circle const& value, LengthContext auto const& ctx) {
        return {computeValue(value.radius, ctx), computeValue(value.position, ctx)};
    }

    static Css::Circle specify(Circle const& value) {
        return {specifyValue<Css::ShapeRadius>(value.radius), specifyValue<Css::BackgroundPosition>(value.position)};
    }
};

// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-ellipse
export struct Ellipse {
    ShapeRadius rx = Keywords::CLOSEST_SIDE;
    ShapeRadius ry = Keywords::CLOSEST_SIDE;
    Position position = Position::center();

    void repr(Io::Emit& e) const {
        e("(ellipse {} {} {})", rx, ry, position);
    }
};

export template <>
struct ValueComputer<Css::Ellipse> {
    static Ellipse compute(Css::Ellipse const& value, LengthContext auto const& ctx) {
        return {computeValue(value.rx, ctx), computeValue(value.ry, ctx), computeValue(value.position, ctx)};
    }

    static Css::Ellipse specify(Ellipse const& value) {
        return {specifyValue<Css::ShapeRadius>(value.rx), specifyValue<Css::ShapeRadius>(value.ry), specifyValue<Css::BackgroundPosition>(value.position)};
    }
};

// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-inset
export struct Inset {
    Math::Insets<PxPercent> insets;
    Math::Radii<PxPercent> borderRadius;

    void repr(Io::Emit& e) const {
        e("(inset {} {})", insets, borderRadius);
    }
};

export template <>
struct ValueComputer<Css::Inset> {
    static Inset compute(Css::Inset const& value, LengthContext auto const& ctx) {
        return {computeValue(value.insets, ctx), computeValue(value.borderRadius, ctx)};
    }

    static Css::Inset specify(Inset const& value) {
        return {specifyValue<Math::Insets<Css::LengthPercentage>>(value.insets), specifyValue<Math::Radii<Css::LengthPercentage>>(value.borderRadius)};
    }
};

// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-polygon
export struct Polygon {
    FillRule fillRule = FillRule::NONZERO;
    Vec<Pair<PxPercent>> points;

    void repr(Io::Emit& e) const {
        e("(polygon {} {})", fillRule, points);
    }
};

export template <>
struct ValueComputer<Css::Polygon> {
    static Polygon compute(Css::Polygon const& value, LengthContext auto const& ctx) {
        Polygon result{computeValue(value.fillRule), {}};
        for (auto const& [x, y] : value.points)
            result.points.emplaceBack(computeValue(x, ctx), computeValue(y, ctx));
        return result;
    }

    static Css::Polygon specify(Polygon const& value) {
        Css::Polygon result{specifyValue<Css::FillRule>(value.fillRule), {}};
        for (auto const& [x, y] : value.points)
            result.points.emplaceBack(specifyValue<Css::LengthPercentage>(x), specifyValue<Css::LengthPercentage>(y));
        return result;
    }
};

// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-rect
export struct Rect {
    Math::Insets<PxPercent> insets;
    Math::Radii<PxPercent> borderRadius;

    void repr(Io::Emit& e) const {
        e("(rect {} {})", insets, borderRadius);
    }
};

export template <>
struct ValueComputer<Css::Rect> {
    static Rect compute(Css::Rect const& value, LengthContext auto const& ctx) {
        return {computeValue(value.insets, ctx), computeValue(value.borderRadius, ctx)};
    }

    static Css::Rect specify(Rect const& value) {
        return {specifyValue<Math::Insets<Css::LengthPercentage>>(value.insets), specifyValue<Math::Radii<Css::LengthPercentage>>(value.borderRadius)};
    }
};

// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-xywh
export struct Xywh {
    PxPercent x;
    PxPercent y;
    PxPercent width;
    PxPercent height;
    Math::Radii<PxPercent> borderRadius;

    void repr(Io::Emit& e) const {
        e("(xywh {} {} {} {} {})", x, y, width, height, borderRadius);
    }
};

export template <>
struct ValueComputer<Css::Xywh> {
    static Xywh compute(Css::Xywh const& value, LengthContext auto const& ctx) {
        return {
            computeValue(value.x, ctx),
            computeValue(value.y, ctx),
            computeValue(value.width, ctx),
            computeValue(value.height, ctx),
            computeValue(value.borderRadius, ctx),
        };
    }

    static Css::Xywh specify(Xywh const& value) {
        return {
            specifyValue<Css::LengthPercentage>(value.x),
            specifyValue<Css::LengthPercentage>(value.y),
            specifyValue<Css::LengthPercentage>(value.width),
            specifyValue<Css::LengthPercentage>(value.height),
            specifyValue<Math::Radii<Css::LengthPercentage>>(value.borderRadius),
        };
    }
};

// https://drafts.csswg.org/css-shapes-1/#typedef-basic-shape
export using BasicShapeFunction = Union<
    Circle,
    Ellipse,
    Inset,
    Path,
    Polygon,
    Rect,
    Xywh>;

export struct BasicShape {
    Opt<BasicShapeFunction> shape;
    GeometryBox referenceBox = Keywords::BORDER_BOX;

    void repr(Io::Emit& e) const {
        e("(basic-shape {} {})", shape, referenceBox);
    }
};

export template <>
struct ValueComputer<Css::BasicShape> {
    static BasicShape compute(Css::BasicShape const& value, LengthContext auto const& ctx) {
        return {
            value.shape.map([&](Css::BasicShapeFunction const& shape) {
                return shape.visit(
                    [](Css::Path const& path) -> BasicShapeFunction {
                        return computeValue(path);
                    },
                    [&](auto const& s) -> BasicShapeFunction {
                        return computeValue(s, ctx);
                    }
                );
            }),
            value.referenceBox,
        };
    }

    static Css::BasicShape specify(BasicShape const& value) {
        return {
            value.shape.map([](BasicShapeFunction const& shape) {
                return specifyValue<Css::BasicShapeFunction>(shape);
            }),
            value.referenceBox,
        };
    }
};

// https://drafts.csswg.org/css-masking-1/#propdef-clip-path
export using ClipPath = Opt<BasicShape>;

} // namespace Vaev::Style
