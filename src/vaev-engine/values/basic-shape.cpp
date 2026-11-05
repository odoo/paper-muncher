module;

#include <karm/macros>

export module Vaev.Engine:values.basicShape;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :css;
import :values.base;
import :values.calc2;
import :values.length;
import :values.percent;
import :values.background;
import :values.insets;
import :values.borders;
import :values.keywords;
import :values.unitValue;

using namespace Karm;

namespace Vaev {

// https://drafts.csswg.org/css-masking/#typedef-geometry-box
export using ShapeBox = Union<
    Keywords::BorderBox,
    Keywords::PaddingBox,
    Keywords::ContentBox,
    Keywords::MarginBox>;

export using GeometryBox = Union<
    Keywords::BorderBox,
    Keywords::PaddingBox,
    Keywords::ContentBox,
    Keywords::MarginBox,
    Keywords::FillBox,
    Keywords::StrokeBox,
    Keywords::ViewBox>;

export using FillRule = Union<
    Keywords::Nonzero,
    Keywords::Evenodd>;

export Gfx::FillRule fillRuleToGfx(FillRule rule) {
    return rule.visit(
        [](Keywords::Nonzero&) {
            return Gfx::FillRule::NONZERO;
        },
        [](Keywords::Evenodd&) {
            return Gfx::FillRule::EVENODD;
        }
    );
}

export template <typename L>
using ShapeRadiusT = Union<
    L,
    Keywords::ClosestSide,
    Keywords::FarthestSide>;

export using ShapeRadius = ShapeRadiusT<LengthPercentage>;
namespace Style {

export using ShapeRadius = ShapeRadiusT<PixelPercentage>;

} // namespace Style

export Style::ShapeRadius resolve(ShapeRadius const& value, LengthContext auto const& ctx) {
    return value.visit([&](auto const& v) -> Style::ShapeRadius { return resolve(v, ctx); });
}

export ShapeRadius toSpecified(Style::ShapeRadius const& value) {
    return value.visit([](auto const& v) -> ShapeRadius { return toSpecified(v); });
}

// MARK: Circle ----------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-circle

export template <typename L, typename P>
struct CircleT {
    ShapeRadiusT<L> radius = Keywords::CLOSEST_SIDE;
    P position = P::center();

    void repr(Io::Emit& e) const {
        e("(circle {} {})", radius, position);
    }
};

export using Circle = CircleT<LengthPercentage, BackgroundPosition>;
namespace Style {

export using Circle = CircleT<PixelPercentage, Style::Position>;

} // namespace Style

export Style::Circle resolve(Circle const& value, LengthContext auto const& ctx) {
    return {resolve(value.radius, ctx), resolve(value.position, ctx)};
}

export Circle toSpecified(Style::Circle const& value) {
    return {toSpecified(value.radius), toSpecified(value.position)};
}

export template <>
struct ValueParser<Circle> {
    static Res<Circle> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("circle(")) {
            Circle result;
            Cursor<Css::Sst> scan = c->content;

            if (auto radius = parseValue<ShapeRadius>(scan)) {
                result.radius = radius.expect();
            }

            eatWhitespace(scan);
            if (scan.skip(Css::Token::ident("at"))) {
                eatWhitespace(scan);
                if (auto position = parseValue<BackgroundPosition>(scan)) {
                    result.position = position.expect();
                }
                eatWhitespace(scan);
            }

            if (not scan.ended())
                return Error::invalidData("unexpected data in circle");

            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid circle");
    }
};

// MARK: Ellipse ---------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-ellipse

export template <typename L, typename P>
struct EllipseT {
    ShapeRadiusT<L> rx = Keywords::CLOSEST_SIDE;
    ShapeRadiusT<L> ry = Keywords::CLOSEST_SIDE;
    P position = P::center();

    void repr(Io::Emit& e) const {
        e("(ellipse {} {} {})", rx, ry, position);
    }
};

export using Ellipse = EllipseT<LengthPercentage, BackgroundPosition>;
namespace Style {

export using Ellipse = EllipseT<PixelPercentage, Style::Position>;

} // namespace Style

export Style::Ellipse resolve(Ellipse const& value, LengthContext auto const& ctx) {
    return {resolve(value.rx, ctx), resolve(value.ry, ctx), resolve(value.position, ctx)};
}

export Ellipse toSpecified(Style::Ellipse const& value) {
    return {toSpecified(value.rx), toSpecified(value.ry), toSpecified(value.position)};
}

export template <>
struct ValueParser<Ellipse> {
    static Res<Ellipse> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("ellipse(")) {
            Ellipse result;
            Cursor<Css::Sst> scan = c->content;

            if (auto radius = parseValue<ShapeRadius>(scan)) {
                result.rx = radius.expect();
                eatWhitespace(scan);
                result.ry = try$(parseValue<ShapeRadius>(scan));
            }

            eatWhitespace(scan);
            if (scan.skip(Css::Token::ident("at"))) {
                eatWhitespace(scan);
                if (auto position = parseValue<BackgroundPosition>(scan)) {
                    result.position = position.expect();
                }
                eatWhitespace(scan);
            }

            if (not scan.ended())
                return Error::invalidData("unexpected data in ellipse");

            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid ellipse");
    }
};

// MARK: Inset -----------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-inset

export template <typename L>
struct InsetT {
    Math::Insets<L> insets;
    Math::Radii<L> borderRadius;

    void repr(Io::Emit& e) const {
        e("(inset {} ", insets);
        reprRadii(e, borderRadius);
        e(")");
    }
};

export using Inset = InsetT<LengthPercentage>;
namespace Style {

export using Inset = InsetT<PixelPercentage>;

} // namespace Style

export Style::Inset resolve(Inset const& value, LengthContext auto const& ctx) {
    return {resolve(value.insets, ctx), resolve(value.borderRadius, ctx)};
}

export Inset toSpecified(Style::Inset const& value) {
    return {toSpecified(value.insets), toSpecified(value.borderRadius)};
}

export template <>
struct ValueParser<Inset> {
    static Res<Inset> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("inset(")) {
            Cursor<Css::Sst> scan = c->content;

            auto insets = parseValue<Math::Insets<LengthPercentage>>(scan);
            if (not insets)
                return Error::invalidData("expected insets");

            Inset result{insets.take(), {LengthPercentage::fromPercentage(0)}};

            eatWhitespace(scan);
            if (scan.skip(Css::Token::ident("round"))) {
                eatWhitespace(scan);
                if (auto radii = parseValue<Math::Radii<LengthPercentage>>(scan)) {
                    result.borderRadius = radii.expect();
                }
                eatWhitespace(scan);
            }

            if (not scan.ended())
                return Error::invalidData("unexpected data in inset");

            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid inset");
    }
};

// MARK: Path ------------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-path

export struct Path {
    Math::Path path;
    Gfx::FillRule fillRule = Gfx::FillRule::NONZERO;

    void repr(Io::Emit& e) const {
        e("(path {} {})", fillRule, path);
    }
};

export template <>
struct ValueParser<Path> {
    static Res<Path> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("path(")) {
            Path result;
            Cursor<Css::Sst> scan = c->content;

            bool begin = true;
            if (auto fill = parseValue<FillRule>(scan)) {
                result.fillRule = fillRuleToGfx(fill.expect());
                begin = false;
            }

            while ((begin or scan.skip(Css::Token::COMMA)) and not scan.ended()) {
                begin = false;

                eatWhitespace(scan);
                if (not result.path.evalSvg(try$(parseValue<String>(scan))))
                    return Error::invalidData("expected svg path in a string");

                if (not last(result.path._contours).close)
                    result.path.close();
            }

            if (begin)
                return Error::invalidData("expected data in path");

            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid path");
    }
};

// MARK: Polygon ---------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-polygon

export template <typename L>
struct PolygonT {
    Gfx::FillRule fillRule = Gfx::FillRule::NONZERO;
    Vec<Pair<L>> points;

    void repr(Io::Emit& e) const {
        e("(polygon {} {})", fillRule, points);
    }
};

export using Polygon = PolygonT<LengthPercentage>;
namespace Style {

export using Polygon = PolygonT<PixelPercentage>;

} // namespace Style

export Style::Polygon resolve(Polygon const& value, LengthContext auto const& ctx) {
    Style::Polygon result{value.fillRule, {}};
    for (auto const& [x, y] : value.points)
        result.points.emplaceBack(resolve(x, ctx), resolve(y, ctx));
    return result;
}

export Polygon toSpecified(Style::Polygon const& value) {
    Polygon result{value.fillRule, {}};
    for (auto const& [x, y] : value.points)
        result.points.emplaceBack(toSpecified(x), toSpecified(y));
    return result;
}

export template <>
struct ValueParser<Polygon> {
    static Res<Polygon> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("polygon(")) {
            Polygon result;
            Cursor<Css::Sst> scan = c->content;

            bool begin = true;
            if (auto fill = parseValue<FillRule>(scan)) {
                result.fillRule = fillRuleToGfx(fill.expect());
                begin = false;
            }

            while ((begin or scan.skip(Css::Token::COMMA)) and not scan.ended()) {
                begin = false;

                eatWhitespace(scan);
                auto x = try$(parseValue<LengthPercentage>(scan));
                eatWhitespace(scan);
                auto y = try$(parseValue<LengthPercentage>(scan));
                eatWhitespace(scan);
                result.points.emplaceBack(x, y);
            }

            if (not result.points)
                return Error::invalidData("expected points in polygon");

            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid polygon");
    }
};

// MARK: Rect ------------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-rect

export template <typename L>
struct RectT {
    Math::Insets<L> insets;
    Math::Radii<L> borderRadius;

    void repr(Io::Emit& e) const {
        e("(rect {} ", insets);
        reprRadii(e, borderRadius);
        e(")");
    }
};

export using Rect = RectT<LengthPercentage>;
namespace Style {

export using Rect = RectT<PixelPercentage>;

} // namespace Style

export Style::Rect resolve(Rect const& value, LengthContext auto const& ctx) {
    return {resolve(value.insets, ctx), resolve(value.borderRadius, ctx)};
}

export Rect toSpecified(Style::Rect const& value) {
    return {toSpecified(value.insets), toSpecified(value.borderRadius)};
}

export template <>
struct ValueParser<Rect> {
    static Res<Rect> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("rect(")) {
            // NOTE: An auto value makes the edge of the box coincide with the corresponding edge of the reference box.
            Rect result{
                {LengthPercentage::fromPercentage(0), LengthPercentage::fromPercentage(100), LengthPercentage::fromPercentage(100), LengthPercentage::fromPercentage(0)},
                {LengthPercentage::fromPercentage(0)},
            };
            Cursor<Css::Sst> scan = c->content;

            eatWhitespace(scan);
            if (not parseValue<Keywords::Auto>(scan))
                result.insets.top = try$(parseValue<LengthPercentage>(scan));

            eatWhitespace(scan);
            if (not parseValue<Keywords::Auto>(scan))
                result.insets.end = try$(parseValue<LengthPercentage>(scan));

            eatWhitespace(scan);
            if (not parseValue<Keywords::Auto>(scan))
                result.insets.bottom = try$(parseValue<LengthPercentage>(scan));

            eatWhitespace(scan);
            if (not parseValue<Keywords::Auto>(scan))
                result.insets.start = try$(parseValue<LengthPercentage>(scan));

            eatWhitespace(scan);
            if (scan.skip(Css::Token::ident("round"))) {
                eatWhitespace(scan);
                if (auto radii = parseValue<Math::Radii<LengthPercentage>>(scan)) {
                    result.borderRadius = radii.expect();
                }
                eatWhitespace(scan);
            }
            if (not scan.ended())
                return Error::invalidData("unexpected data in rect");

            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid rect");
    }
};

// MARK: Xywh ------------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-xywh

export template <typename L>
struct XywhT {
    L x;
    L y;
    L width;
    L height;
    Math::Radii<L> borderRadius;

    void repr(Io::Emit& e) const {
        e("(xywh {} {} {} {} ", x, y, width, height);
        reprRadii(e, borderRadius);
        e(")");
    }
};

export using Xywh = XywhT<LengthPercentage>;
namespace Style {

export using Xywh = XywhT<PixelPercentage>;

} // namespace Style

export Style::Xywh resolve(Xywh const& value, LengthContext auto const& ctx) {
    return {
        resolve(value.x, ctx),
        resolve(value.y, ctx),
        resolve(value.width, ctx),
        resolve(value.height, ctx),
        resolve(value.borderRadius, ctx),
    };
}

export Xywh toSpecified(Style::Xywh const& value) {
    return {
        toSpecified(value.x),
        toSpecified(value.y),
        toSpecified(value.width),
        toSpecified(value.height),
        toSpecified(value.borderRadius),
    };
}

export template <>
struct ValueParser<Xywh> {
    static Res<Xywh> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("xywh(")) {
            Cursor<Css::Sst> scan = c->content;

            eatWhitespace(scan);
            auto x = try$(parseValue<LengthPercentage>(scan));

            eatWhitespace(scan);
            auto y = try$(parseValue<LengthPercentage>(scan));

            eatWhitespace(scan);
            auto width = try$(parseValue<LengthPercentage>(scan));

            eatWhitespace(scan);
            auto height = try$(parseValue<LengthPercentage>(scan));

            Xywh result{x, y, width, height, {LengthPercentage::fromPercentage(0)}};

            eatWhitespace(scan);
            if (scan.skip(Css::Token::ident("round"))) {
                eatWhitespace(scan);
                if (auto radii = parseValue<Math::Radii<LengthPercentage>>(scan)) {
                    result.borderRadius = radii.expect();
                }
                eatWhitespace(scan);
            }

            if (not scan.ended())
                return Error::invalidData("unexpected data in xywh");

            c.next();
            return Ok(result);
        }
        return Error::invalidData("invalid xywh");
    }
};

// MARK: Basic Shape -----------------------------------------------------------
// https://drafts.csswg.org/css-masking/#the-clip-path
//  [ <basic-shape> || <geometry-box> ]

export template <typename L, typename P>
using BasicShapeFunctionT = Union<
    CircleT<L, P>, EllipseT<L, P>, InsetT<L>, Path, PolygonT<L>, RectT<L>, XywhT<L>>;

export using BasicShapeFunction = BasicShapeFunctionT<LengthPercentage, BackgroundPosition>;
namespace Style {

export using BasicShapeFunction = BasicShapeFunctionT<PixelPercentage, Style::Position>;

} // namespace Style

export template <typename L, typename P>
struct BasicShapeT {
    Opt<BasicShapeFunctionT<L, P>> shape;
    GeometryBox referenceBox = Keywords::BORDER_BOX;

    void repr(Io::Emit& e) const {
        e("(basic-shape {} {})", shape, referenceBox);
    }
};

export using BasicShape = BasicShapeT<LengthPercentage, BackgroundPosition>;
namespace Style {

export using BasicShape = BasicShapeT<PixelPercentage, Style::Position>;

} // namespace Style

export Style::BasicShape resolve(BasicShape const& value, LengthContext auto const& ctx) {
    return {
        value.shape.map([&](BasicShapeFunction const& shape) {
            return shape.visit(
                [](Path const& path) -> Style::BasicShapeFunction {
                    return path;
                },
                [&](auto const& s) -> Style::BasicShapeFunction {
                    return resolve(s, ctx);
                }
            );
        }),
        value.referenceBox,
    };
}

export BasicShape toSpecified(Style::BasicShape const& value) {
    return {
        value.shape.map([](Style::BasicShapeFunction const& shape) {
            return shape.visit(
                [](Path const& path) -> BasicShapeFunction {
                    return path;
                },
                [](auto const& s) -> BasicShapeFunction {
                    return toSpecified(s);
                }
            );
        }),
        value.referenceBox,
    };
}

export template <>
struct ValueParser<BasicShape> {
    static Res<BasicShape> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        BasicShape resultShape;
        auto box = parseValue<GeometryBox>(c);
        auto shape = parseValue<BasicShapeFunction>(c);

        if (shape and not box)
            box = parseValue<GeometryBox>(c);

        if (not shape and not box)
            return Error::invalidData("expected basic shape or geometry box");

        resultShape.referenceBox = box.unwrapOr(Keywords::BORDER_BOX);

        if (shape)
            resultShape.shape = Some(shape.expect());

        return Ok(resultShape);
    }
};

} // namespace Vaev
