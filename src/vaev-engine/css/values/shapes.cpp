module;

#include <karm/macros>

export module Vaev.Engine:css.values.shapes;

import Karm.Core;
import Karm.Gfx;
import Karm.Math;

import :css.lexer;
import :css.parser;
import :css.values.backgrounds;
import :css.values.base;
import :css.values.numeric;
import :css.values.keywords;
import :css.values.primitives;

using namespace Karm;

namespace Vaev::Css {

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

Gfx::FillRule fillRuleToGfx(FillRule rule) {
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
    LengthPercentage,
    Keywords::ClosestSide,
    Keywords::FarthestSide>;

// MARK: Circle ----------------------------------------------------------------
// https://www.w3.org/TR/css-shapes-1/#funcdef-basic-shape-circle

export struct Circle {
    ShapeRadius radius = Keywords::CLOSEST_SIDE;
    BackgroundPosition position = BackgroundPosition::center();

    void repr(Io::Emit& e) const {
        e("(circle {} {})", radius, position);
    }
};


export template <>
struct ValueParser<Circle> {
    static Res<Circle> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Token::function("circle(")) {
            Circle result;
            Cursor<Sst> scan = c->content;

            if (auto radius = parseValue<ShapeRadius>(scan)) {
                result.radius = radius.expect();
            }

            eatWhitespace(scan);
            if (scan.skip(Token::ident("at"))) {
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

export struct Ellipse {
    ShapeRadius rx = Keywords::CLOSEST_SIDE;
    ShapeRadius ry = Keywords::CLOSEST_SIDE;
    BackgroundPosition position = BackgroundPosition::center();

    void repr(Io::Emit& e) const {
        e("(ellipse {} {} {})", rx, ry, position);
    }
};


export template <>
struct ValueParser<Ellipse> {
    static Res<Ellipse> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Token::function("ellipse(")) {
            Ellipse result;
            Cursor<Sst> scan = c->content;

            if (auto radius = parseValue<ShapeRadius>(scan)) {
                result.rx = radius.expect();
                eatWhitespace(scan);
                result.ry = try$(parseValue<ShapeRadius>(scan));
            }

            eatWhitespace(scan);
            if (scan.skip(Token::ident("at"))) {
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

export struct Inset {
    Math::Insets<LengthPercentage> insets;
    Math::Radii<LengthPercentage> borderRadius;

    void repr(Io::Emit& e) const {
        e("(inset {} {})", insets, borderRadius);
    }
};


export template <>
struct ValueParser<Inset> {
    static Res<Inset> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Token::function("inset(")) {
            Cursor<Sst> scan = c->content;

            auto insets = parseValue<Math::Insets<LengthPercentage>>(scan);
            if (not insets)
                return Error::invalidData("expected insets");

            Inset result{insets.take(), {percent(0)}};

            eatWhitespace(scan);
            if (scan.skip(Token::ident("round"))) {
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
    static Res<Path> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Token::function("path(")) {
            Path result;
            Cursor<Sst> scan = c->content;

            bool begin = true;
            if (auto fill = parseValue<FillRule>(scan)) {
                result.fillRule = fillRuleToGfx(fill.expect());
                begin = false;
            }

            while ((begin or scan.skip(Token::COMMA)) and not scan.ended()) {
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

export struct Polygon {
    Gfx::FillRule fillRule = Gfx::FillRule::NONZERO;
    Vec<Pair<LengthPercentage>> points;

    void repr(Io::Emit& e) const {
        e("(polygon {} {})", fillRule, points);
    }
};


export template <>
struct ValueParser<Polygon> {
    static Res<Polygon> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Token::function("polygon(")) {
            Polygon result;
            Cursor<Sst> scan = c->content;

            bool begin = true;
            if (auto fill = parseValue<FillRule>(scan)) {
                result.fillRule = fillRuleToGfx(fill.expect());
                begin = false;
            }

            while ((begin or scan.skip(Token::COMMA)) and not scan.ended()) {
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

export struct Rect {
    Math::Insets<LengthPercentage> insets;
    Math::Radii<LengthPercentage> borderRadius;

    void repr(Io::Emit& e) const {
        e("(rect {} {})", insets, borderRadius);
    }
};


export template <>
struct ValueParser<Rect> {
    static Res<Rect> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Token::function("rect(")) {
            // NOTE: An auto value makes the edge of the box coincide with the corresponding edge of the reference box.
            Rect result{
                {percent(0), percent(100), percent(100), percent(0)},
                {percent(0)},
            };
            Cursor<Sst> scan = c->content;

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
            if (scan.skip(Token::ident("round"))) {
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

export struct Xywh {
    LengthPercentage x;
    LengthPercentage y;
    LengthPercentage width;
    LengthPercentage height;
    Math::Radii<LengthPercentage> borderRadius;

    void repr(Io::Emit& e) const {
        e("(xywh {} {} {} {} {})", x, y, width, height, borderRadius);
    }
};


export template <>
struct ValueParser<Xywh> {
    static Res<Xywh> parse(Cursor<Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c->prefix == Token::function("xywh(")) {
            Cursor<Sst> scan = c->content;

            eatWhitespace(scan);
            auto x = try$(parseValue<LengthPercentage>(scan));

            eatWhitespace(scan);
            auto y = try$(parseValue<LengthPercentage>(scan));

            eatWhitespace(scan);
            auto width = try$(parseValue<LengthPercentage>(scan));

            eatWhitespace(scan);
            auto height = try$(parseValue<LengthPercentage>(scan));

            Xywh result{x, y, width, height, {percent(0)}};

            eatWhitespace(scan);
            if (scan.skip(Token::ident("round"))) {
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

export using BasicShapeFunction = Union<
    Circle, Ellipse, Inset, Path, Polygon, Rect, Xywh>;

export struct BasicShape {
    Opt<BasicShapeFunction> shape;
    GeometryBox referenceBox = Keywords::BORDER_BOX;

    void repr(Io::Emit& e) const {
        e("(basic-shape {} {})", shape, referenceBox);
    }
};


export template <>
struct ValueParser<BasicShape> {
    static Res<BasicShape> parse(Cursor<Sst>& c) {
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

} // namespace Vaev::Css
