module;

#include <karm-io/aton.h>
#include <karm-scene/base.h>
#include <karm-scene/box.h>
#include <karm-scene/shape.h>
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
                              }});
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

    logDebug("descobriu {} {} {}", cssColor, currentColor, Vaev::resolve(cssColor, currentColor));

    return Vaev::resolve(cssColor, currentColor);
}

namespace SVG {
struct Rectangle {
    PercentOr<Length> x;
    PercentOr<Length> y;

    PercentOr<Length> width;
    PercentOr<Length> height;

    static Rectangle build(Rc<Style::ComputedStyle> style) {
        return {
            style->svg->y,
            style->svg->x,
            fromSize(style->sizing->width),
            fromSize(style->sizing->height)
        };
    }

    Rc<Scene::Node> toSceneNode(Vec2Au relativeTo, Opt<Gfx::Fill> fill, Opt<Gfx::Fill>) const {
        auto rect = makeRc<Scene::Box>(
            Math::Rect<f64>{
                resolve(x, relativeTo.x).cast<f64>(),
                resolve(y, relativeTo.y).cast<f64>(),
                resolve(width, relativeTo.x).cast<f64>(),
                resolve(height, relativeTo.y).cast<f64>(),
            },
            Gfx::Borders{},
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

    Rc<Scene::Node> toSceneNode(Vec2Au relativeTo, Opt<Gfx::Fill> fill, Opt<Gfx::Fill> stroke) const {
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
        return {
            Math::Path::fromSvg(style->svg->d.unwrap<String>())
        };
    }

    Rc<Scene::Node> toSceneNode(Opt<Gfx::Fill> fill, Opt<Gfx::Fill> stroke) const {
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
            style->svg->stroke
        };
    }

    Rc<Scene::Node> toSceneNode(Vec2Au relativeTo, Gfx::Color currentColor) const {
        Opt<Gfx::Color> resolvedFill = resolve(fill, currentColor);
        Opt<Gfx::Color> resolvedStroke = resolve(stroke, currentColor);

        if (auto rect = shape.is<Rectangle>()) {
            return rect->toSceneNode(relativeTo, resolvedFill, resolvedStroke);
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

} // namespace SVG

} // namespace Vaev::Layout
