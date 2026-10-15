module;

#include <karm/macros>

export module Vaev.Engine:values.basicShape;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :css;
import :values.base;
import :values.calc;
import :values.length;
import :values.percent;
import :values.background;
import :values.insets;
import :values.borders;
import :values.keywords;
import :values.resolved;

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

export using ShapeRadius = Union<
    Calc<Length, Percent>,
    Keywords::ClosestSide,
    Keywords::FarthestSide>;

// MARK: Circle ----------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-circle

export template <template <typename> typename S>
struct _Circle {
    S<ShapeRadius> radius = Keywords::CLOSEST_SIDE;
    S<BackgroundPosition> position = {Keywords::CENTER, Percent(0), Keywords::CENTER, Percent(0)};

    void repr(Io::Emit& e) const {
        e("(circle {} {})", radius, position);
    }
};

export using Circle = _Circle<Specified>;

export template <>
struct _Computed<Circle> {
    using Type = _Circle<Computed>;
};

export Computed<Circle> compute(Circle const& value, auto const& ctx) {
    return {compute(value.radius, ctx), compute(value.position, ctx)};
}

export template <>
struct _Specify<Circle> {
    static Circle from(Computed<Circle> const& value) {
        return {specified<ShapeRadius>(value.radius), specified<BackgroundPosition>(value.position)};
    }
};

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

export template <template <typename> typename S>
struct _Ellipse {
    S<ShapeRadius> rx = Keywords::CLOSEST_SIDE;
    S<ShapeRadius> ry = Keywords::CLOSEST_SIDE;
    S<BackgroundPosition> position = {Keywords::CENTER, Percent(0), Keywords::CENTER, Percent(0)};

    void repr(Io::Emit& e) const {
        e("(ellipse {} {} {})", rx, ry, position);
    }
};

export using Ellipse = _Ellipse<Specified>;

export template <>
struct _Computed<Ellipse> {
    using Type = _Ellipse<Computed>;
};

export Computed<Ellipse> compute(Ellipse const& value, auto const& ctx) {
    return {compute(value.rx, ctx), compute(value.ry, ctx), compute(value.position, ctx)};
}

export template <>
struct _Specify<Ellipse> {
    static Ellipse from(Computed<Ellipse> const& value) {
        return {
            specified<ShapeRadius>(value.rx),
            specified<ShapeRadius>(value.ry),
            specified<BackgroundPosition>(value.position),
        };
    }
};

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

export template <template <typename> typename S>
struct _Inset {
    S<Math::Insets<Calc<Length, Percent>>> insets = {Percent(0)};
    S<Math::Radii<Calc<Length, Percent>>> borderRadius = {Percent(0)};

    void repr(Io::Emit& e) const {
        e("(inset {} {})", insets, borderRadius);
    }
};

export using Inset = _Inset<Specified>;

export template <>
struct _Computed<Inset> {
    using Type = _Inset<Computed>;
};

export Computed<Inset> compute(Inset const& value, auto const& ctx) {
    return {compute(value.insets, ctx), compute(value.borderRadius, ctx)};
}

export template <>
struct _Specify<Inset> {
    static Inset from(Computed<Inset> const& value) {
        return {
            specified<Math::Insets<Calc<Length, Percent>>>(value.insets),
            specified<Math::Radii<Calc<Length, Percent>>>(value.borderRadius),
        };
    }
};

export template <>
struct ValueParser<Inset> {
    static Res<Inset> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("inset(")) {
            Inset result;
            Cursor<Css::Sst> scan = c->content;

            if (auto insets = parseValue<Math::Insets<Calc<Length, Percent>>>(scan)) {
                result.insets = insets.expect();
            } else {
                return Error::invalidData("expected insets");
            }

            eatWhitespace(scan);
            if (scan.skip(Css::Token::ident("round"))) {
                eatWhitespace(scan);
                if (auto radii = parseValue<Math::Radii<Calc<Length, Percent>>>(scan)) {
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

export template <template <typename> typename S>
struct _Polygon {
    Gfx::FillRule fillRule = Gfx::FillRule::NONZERO;
    Vec<Pair<S<Calc<Length, Percent>>>> points;

    void repr(Io::Emit& e) const {
        e("(polygon {} {})", fillRule, points);
    }
};

export using Polygon = _Polygon<Specified>;

export template <>
struct _Computed<Polygon> {
    using Type = _Polygon<Computed>;
};

export Computed<Polygon> compute(Polygon const& value, auto const& ctx) {
    Computed<Polygon> result;
    result.fillRule = value.fillRule;
    for (auto const& point : value.points)
        result.points.emplaceBack(compute(point.v0, ctx), compute(point.v1, ctx));
    return result;
}

export template <>
struct _Specify<Polygon> {
    static Polygon from(Computed<Polygon> const& value) {
        Polygon result;
        result.fillRule = value.fillRule;
        for (auto const& point : value.points)
            result.points.emplaceBack(
                specified<Calc<Length, Percent>>(point.v0),
                specified<Calc<Length, Percent>>(point.v1)
            );
        return result;
    }
};

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
                auto x = try$(parseValue<Calc<Length, Percent>>(scan));
                eatWhitespace(scan);
                auto y = try$(parseValue<Calc<Length, Percent>>(scan));
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

export template <template <typename> typename S>
struct _Rect {
    S<Math::Insets<Calc<Length, Percent>>> insets = {Percent(0), Percent(100), Percent(100), Percent(0)};
    S<Math::Radii<Calc<Length, Percent>>> borderRadius = {Percent(0)};

    void repr(Io::Emit& e) const {
        e("(rect {} {})", insets, borderRadius);
    }
};

export using Rect = _Rect<Specified>;

export template <>
struct _Computed<Rect> {
    using Type = _Rect<Computed>;
};

export Computed<Rect> compute(Rect const& value, auto const& ctx) {
    return {compute(value.insets, ctx), compute(value.borderRadius, ctx)};
}

export template <>
struct _Specify<Rect> {
    static Rect from(Computed<Rect> const& value) {
        return {
            specified<Math::Insets<Calc<Length, Percent>>>(value.insets),
            specified<Math::Radii<Calc<Length, Percent>>>(value.borderRadius),
        };
    }
};

export template <>
struct ValueParser<Rect> {
    static Res<Rect> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("rect(")) {
            Rect result;
            Cursor<Css::Sst> scan = c->content;

            eatWhitespace(scan);
            if (not parseValue<Keywords::Auto>(scan))
                result.insets.top = try$(parseValue<Calc<Length, Percent>>(scan));

            eatWhitespace(scan);
            if (not parseValue<Keywords::Auto>(scan))
                result.insets.end = try$(parseValue<Calc<Length, Percent>>(scan));

            eatWhitespace(scan);
            if (not parseValue<Keywords::Auto>(scan))
                result.insets.bottom = try$(parseValue<Calc<Length, Percent>>(scan));

            eatWhitespace(scan);
            if (not parseValue<Keywords::Auto>(scan))
                result.insets.start = try$(parseValue<Calc<Length, Percent>>(scan));

            eatWhitespace(scan);
            if (scan.skip(Css::Token::ident("round"))) {
                eatWhitespace(scan);
                if (auto radii = parseValue<Math::Radii<Calc<Length, Percent>>>(scan)) {
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

export template <template <typename> typename S>
struct _Xywh {
    Math::Rect<S<Calc<Length, Percent>>> rect = {Percent(0), Percent(0), Percent(0), Percent(0)};
    S<Math::Radii<Calc<Length, Percent>>> borderRadius = {Percent(0)};

    void repr(Io::Emit& e) const {
        e("(xywh {} {})", rect, borderRadius);
    }
};

export using Xywh = _Xywh<Specified>;

export template <>
struct _Computed<Xywh> {
    using Type = _Xywh<Computed>;
};

export Computed<Xywh> compute(Xywh const& value, auto const& ctx) {
    return {
        value.rect.map([&](Calc<Length, Percent> const& v) {
            return compute(v, ctx);
        }),
        compute(value.borderRadius, ctx),
    };
}

export template <>
struct _Specify<Xywh> {
    static Xywh from(Computed<Xywh> const& value) {
        return {
            value.rect.map([](Computed<Calc<Length, Percent>> const& v) {
                return specified<Calc<Length, Percent>>(v);
            }),
            specified<Math::Radii<Calc<Length, Percent>>>(value.borderRadius),
        };
    }
};

export template <>
struct ValueParser<Xywh> {
    static Res<Xywh> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Css::Token::function("xywh(")) {
            Xywh result;
            Cursor<Css::Sst> scan = c->content;

            eatWhitespace(scan);
            result.rect.x = try$(parseValue<Calc<Length, Percent>>(scan));

            eatWhitespace(scan);
            result.rect.y = try$(parseValue<Calc<Length, Percent>>(scan));

            eatWhitespace(scan);
            result.rect.width = try$(parseValue<Calc<Length, Percent>>(scan));

            eatWhitespace(scan);
            result.rect.height = try$(parseValue<Calc<Length, Percent>>(scan));

            eatWhitespace(scan);
            if (scan.skip(Css::Token::ident("round"))) {
                eatWhitespace(scan);
                if (auto radii = parseValue<Math::Radii<Calc<Length, Percent>>>(scan)) {
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

export using BasicShapeFunction = Union<
    Circle, Ellipse, Inset, Path, Polygon, Rect, Xywh>;

export template <template <typename> typename S>
struct _BasicShape {
    Opt<S<BasicShapeFunction>> shape;
    GeometryBox referenceBox = Keywords::BORDER_BOX;

    void repr(Io::Emit& e) const {
        e("(basic-shape {} {})", shape, referenceBox);
    }
};

export using BasicShape = _BasicShape<Specified>;

export template <>
struct _Computed<BasicShape> {
    using Type = _BasicShape<Computed>;
};

export Computed<BasicShape> compute(BasicShape const& value, auto const& ctx) {
    Computed<BasicShape> result;
    if (value.shape)
        result.shape = Some(compute(*value.shape, ctx));
    result.referenceBox = value.referenceBox;
    return result;
}

export template <>
struct _Specify<BasicShape> {
    static BasicShape from(Computed<BasicShape> const& value) {
        BasicShape result;
        if (value.shape)
            result.shape = Some(specified<BasicShapeFunction>(*value.shape));
        result.referenceBox = value.referenceBox;
        return result;
    }
};

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
