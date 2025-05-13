module;

#include <karm-io/aton.h>
#include <karm-scene/box.h>
#include <karm-scene/shape.h>
#include <karm-scene/stack.h>
#include <karm-scene/text.h>
#include <vaev-dom/document.h>
#include <vaev-dom/tags.h>
#include <vaev-style/computer.h>
#include <vaev-style/decls.h>

export module Vaev.Layout:svg;

namespace Vaev::Layout {

Opt<PercentOr<Length>> fromCalc(CalcValue<PercentOr<Length>> const& size) {
    return size.visit(
        Visitor{[](CalcValue<PercentOr<Length>>::Value const& v) {
                    return Opt<PercentOr<Length>>{v.unwrap<PercentOr<Length>>()};
                },
                [](auto const) {
                    return Opt<PercentOr<Length>>{NONE};
                }}
    );
}

// FIXME: this should be targeted by the computer refactoring
PercentOr<Length> fromSize(Size const& size) {
    if (size.is<Keywords::Auto>())
        return PercentOr<Length>{Percent{1}};

    auto percentLength = fromCalc(size.unwrap<CalcValue<PercentOr<Length>>>());
    if (not percentLength)
        panic("");
    return percentLength.unwrap();
}

// COPY-PASTED
Au resolve(PercentOr<Length> const& value, Au relative) {
    if (auto valueLength = value.is<Length>())
        return Au{valueLength->val()};
    return Au{relative.cast<f64>() * (value.unwrap<Percent>().value() / 100.)};
}

// FIXME
Opt<Gfx::Color> resolve(Paint color, Gfx::Color currentColor) {
    if (color.is<None>())
        return NONE;

    auto cssColor = color.unwrap<Color>();

    return Vaev::resolve(cssColor, currentColor);
}

Au normalizedDiagonal(Vec2Au relativeTo) {
    return Au{
        Math::sqrt(f64{(relativeTo.x * relativeTo.x) + (relativeTo.y * relativeTo.y)}) / Math::sqrt(2.0)
    };
}

namespace SVG {
struct Rectangle {
    PercentOr<Length> x;
    PercentOr<Length> y;

    PercentOr<Length> width;
    PercentOr<Length> height;

    static Rectangle build(Rc<Style::ComputedStyle> style) {
        return {
            style->svg->x,
            style->svg->y,
            fromSize(style->sizing->width),
            fromSize(style->sizing->height)
        };
    }

    Rc<Scene::Node> toSceneNode(Vec2Au relativeTo, Opt<Gfx::Fill> fill, Opt<Gfx::Fill> strokeColor, Number strokeWidth) const {
        auto resolvedWidth = resolve(width, relativeTo.x).cast<f64>();
        auto resolvedHeight = resolve(height, relativeTo.y).cast<f64>();
        auto resolvedX = resolve(x, relativeTo.x).cast<f64>();
        auto resolvedY = resolve(y, relativeTo.y).cast<f64>();

        Gfx::Borders borders;
        if (strokeColor and strokeWidth > 0) {
            borders = Gfx::Borders{
                Math::Radiif{},
                Math::Insetsf{strokeWidth},
                Array<Gfx::Fill, 4>::fill(*strokeColor),
                Array<Gfx::BorderStyle, 4>::fill(Gfx::BorderStyle::SOLID),
            };

            resolvedWidth += strokeWidth;
            resolvedHeight += strokeWidth;
            resolvedX -= strokeWidth / 2;
            resolvedY -= strokeWidth / 2;
        }

        auto rect = makeRc<Scene::Box>(
            Math::Rect<f64>{resolvedX, resolvedY, resolvedWidth, resolvedHeight},
            borders,
            Gfx::Outline{},
            fill ? Vec<Gfx::Fill>{fill.unwrap()} : Vec<Gfx::Fill>{}
        );

        return rect;
    }

    void repr(Io::Emit& e) const {
        e("(Rectangle {} {} {} {})", x, y, width, height);
    }
};

struct Circle {
    PercentOr<Length> cx;
    PercentOr<Length> cy;
    PercentOr<Length> r;

    static Circle build(Rc<Style::ComputedStyle> style) {
        return {
            style->svg->cx,
            style->svg->cy,
            style->svg->r,
        };
    }

    Rc<Scene::Node> toSceneNode(Vec2Au relativeTo, Opt<Gfx::Fill> fill, Opt<Gfx::Stroke> stroke) const {
        Math::Path path;
        path.ellipse(Math::Ellipse{
            resolve(cx, relativeTo.x).cast<f64>(), resolve(cy, relativeTo.y).cast<f64>(), resolve(r, relativeTo.x).cast<f64>(), // FIXME
        });
        return makeRc<Scene::Shape>(path, stroke, fill);
    }

    void repr(Io::Emit& e) const {
        e("(Circle {} {} {})", cx, cy, r);
    }
};

// https://svgwg.org/svg2-draft/paths.html
struct Path {
    Math::Path path;

    static Path build(Rc<Style::ComputedStyle> style) {
        if (auto d = style->svg->d.is<String>())
            return {
                Math::Path::fromSvg(*d)
            };
        return {Math::Path{}};
    }

    Rc<Scene::Node> toSceneNode(Opt<Gfx::Fill> fill, Opt<Gfx::Stroke> stroke) const {
        return makeRc<Scene::Shape>(path, stroke, fill);
    }

    void repr(Io::Emit& e) const {
        e("(Path)");
    }
};

// https://svgwg.org/svg2-draft/shapes.html#TermShapeElement
using _Shape = Union<
    Rectangle,
    Circle,
    Path>;

struct Shape {
    // NOTE: using composition instead of inheritance due to fill and stroke
    _Shape shape;

    Paint fill;
    Paint stroke;
    PercentOr<Length> strokeWidth;
    Number fillOpacity;

    static _Shape _getShapeFromTagName(Rc<Style::ComputedStyle> style, TagName tagName) {
        if (tagName == Svg::PATH) {
            return SVG::Path::build(style);
        } else if (tagName == Svg::CIRCLE) {
            return SVG::Circle::build(style);
        } else if (tagName == Svg::RECT) {
            return SVG::Rectangle::build(style);
        }
        unreachable();
    }

    static Shape build(Rc<Style::ComputedStyle> style, TagName tagName) {
        return {
            _getShapeFromTagName(style, tagName),
            style->svg->fill,
            style->svg->stroke,
            style->svg->strokeWidth,
            style->svg->fillOpacity,
        };
    }

    Rc<Scene::Node> toSceneNode(Vec2Au relativeTo, Gfx::Color currentColor) const {
        Opt<Gfx::Color> resolvedFill = resolve(fill, currentColor);
        Opt<Gfx::Color> resolvedStrokeColor = resolve(stroke, currentColor);
        Opt<Gfx::Stroke> resolvedStroke;
        Number resolvedStrokeWidth = Number{resolve(strokeWidth, normalizedDiagonal(relativeTo))};

        if (resolvedFill)
            resolvedFill = resolvedFill->withOpacity(fillOpacity);

        if (resolvedStrokeColor)
            resolvedStroke = {*resolvedStrokeColor, resolvedStrokeWidth};

        if (auto rect = shape.is<Rectangle>()) {
            return rect->toSceneNode(relativeTo, resolvedFill, resolvedStrokeColor, resolvedStrokeWidth);
        } else if (auto circle = shape.is<Circle>()) {
            return circle->toSceneNode(relativeTo, resolvedFill, resolvedStroke);
        } else if (auto path = shape.is<Path>()) {
            return path->toSceneNode(resolvedFill, resolvedStroke);
        };
        unreachable();
    }

    void repr(Io::Emit& e) const {
        e("(Shape {} {} {})", shape, fill, stroke);
    }
};

// https://svgwg.org/svg2-draft/embedded.html
struct ForeignObject {
    usize foreignObjectBoxIndex;

    void repr(Io::Emit& e) const {
        e("(Foreign Obj)");
    }
};

bool isShape(TagName tagName) {
    return tagName == Svg::PATH or tagName == Svg::CIRCLE or tagName == Svg::RECT or tagName == Svg::PATH;
}

Math::Trans2<Au> computeTransformOfSVGViewportNoViewBox(Vec2Au const& position) {
    return Math::Trans2<Au>::translate(position);
}

// https://svgwg.org/svg2-draft/coords.html#ComputingAViewportsTransform
Math::Trans2<Au> computeEquivalentTransformOfSVGViewport(ViewBox const& vb, Vec2Au const& position, Vec2Au const& size) {
    // 1. Let vb-x, vb-y, vb-width, vb-height be the min-x, min-y, width and height values of the viewBox attribute
    // respectively.
    // 2. Let e-x, e-y, e-width, e-height be the position and size of the element respectively.

    // 3. Let align be the align value of preserveAspectRatio, or 'xMidYMid' if preserveAspectRatio is not defined.
    // FIXME: preserveAspectRatio still not implemented
    Opt<AlignAxisSVG> align{AlignAxisSVG{AlignAxisSVG::MID, AlignAxisSVG::MID}};

    // 4. Let meetOrSlice be the meetOrSlice value of preserveAspectRatio, or 'meet' if preserveAspectRatio is not
    // defined or if meetOrSlice is missing from this value.
    // FIXME: preserveAspectRatio still not implemented
    MeetOrSlice meetOrSlice{MeetOrSlice::MEET};

    // 5. Initialize scale-x to e-width/vb-width.
    Au scaleX = size.x / Au{vb.width};

    // 6. Initialize scale-y to e-height/vb-height.
    Au scaleY = size.y / Au{vb.height};

    // 7. If align is not 'none' and meetOrSlice is 'meet', set the larger of scale-x and scale-y to the smaller.
    // 8. Otherwise, if align is not 'none' and meetOrSlice is 'slice', set the smaller of scale-x and scale-y to
    // the larger.
    if (align != NONE and meetOrSlice == MeetOrSlice::MEET) {
        scaleY = scaleX = min(scaleX, scaleY);
    } else if (align != NONE and meetOrSlice == MeetOrSlice::SLICE) {
        scaleY = scaleX = max(scaleX, scaleY);
    }

    // 9. Initialize translate-x to e-x - (vb-x * scale-x).
    Au translateX = position.x;

    // 10. Initialize translate-y to e-y - (vb-y * scale-y)
    Au translateY = position.y;

    if (align) {
        // 11. If align contains 'xMid', add (e-width - vb-width * scale-x) / 2 to translate-x.
        if (align->x == AlignAxisSVG::MID) {
            translateX += (size.x - Au{vb.width} * scaleX) / Au{2};
        }

        // 12. If align contains 'xMax', add (e-width - vb-width * scale-x) to translate-x.
        if (align->x == AlignAxisSVG::MAX) {
            translateX += (size.x - Au{vb.width} * scaleX);
        }

        // 13. If align contains 'yMid', add (e-height - vb-height * scale-y) / 2 to translate-y.
        if (align->y == AlignAxisSVG::MID) {
            translateY += (size.y - Au{vb.height} * scaleY) / Au{2};
        }

        // 14. If align contains 'yMax', add (e-height - vb-height * scale-y) to translate-y.
        if (align->y == AlignAxisSVG::MAX) {
            translateY += (size.y - Au{vb.height} * scaleY);
        }
    }

    return Math::Trans2<Au>::scale({scaleX, scaleY}).translated({translateX, translateY});
}

// https://svgwg.org/svg2-draft/coords.html#SizingSVGInCSS
Opt<Number> intrinsicAspectRatio(Opt<ViewBox> const& vb, Size const& width, Size const& height) {
    // FIXME
    auto absoluteValue = [](Size size) -> Opt<Length> {
        if (not size.is<CalcValue<PercentOr<Length>>>())
            return NONE;

        auto calc = size.unwrap<CalcValue<PercentOr<Length>>>();

        auto percOrLength = fromCalc(calc);
        if (not percOrLength)
            return NONE;

        if (percOrLength->is<Percent>())
            return NONE;

        return percOrLength->unwrap<Length>();
    };

    auto absWidth = absoluteValue(width);
    auto absHeight = absoluteValue(height);

    // 1. If the width and height sizing properties on the ‘svg’ element are both absolute values:
    if (absWidth and absHeight)
        return absWidth->val() / absHeight->val();

    // 2. If an SVG View is active:
    // TODO

    // 3. If the ‘viewBox’ on the ‘svg’ element is correctly specified:
    if (vb) {
        return (f64)vb->width / vb->height;
    }

    return NONE;
}

} // namespace SVG

} // namespace Vaev::Layout
