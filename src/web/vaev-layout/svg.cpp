module;

#include <karm-scene/base.h>
#include <karm-scene/box.h>
// #include <karm-scene/shape.h>
#include <karm-scene/text.h>
#include <karm-io/aton.h>
#include <vaev-dom/document.h>
#include <vaev-dom/tags.h>

export module Vaev.Layout:svg;

import :base;

namespace Vaev::Layout {

Rc<Scene::Box> buildRect(Gc::Ref<Dom::Element> el) {
    auto x = Io::atof(el->getAttribute(AttrName::make("x", SVG)).unwrapOr("0")).unwrapOr(0);
    auto y = Io::atof(el->getAttribute(AttrName::make("y", SVG)).unwrapOr("0")).unwrapOr(0);
    auto width = Io::atof(el->getAttribute(AttrName::make("width", SVG)).unwrapOr("0")).unwrapOr(0);
    auto height = Io::atof(el->getAttribute(AttrName::make("height", SVG)).unwrapOr("0")).unwrapOr(0);

    auto rect = makeRc<Scene::Box>(
        Math::Rectf{x, y, width, height},
        Gfx::Borders{},
        Gfx::Outline{},
        Vec<Gfx::Fill>{Gfx::BLACK}
    );

    return rect;
}

void buildSVGElement(Scene::Stack& stack, Gc::Ref<Dom::Element> el) {
    if (el->tagName == Svg::LINE) {
    } else if (el->tagName == Svg::PATH) {
    } else if (el->tagName == Svg::RECT) {
        stack.add(buildRect(el));
    } else if (el->tagName == Svg::TEXT) {
    }
}

export Scene::Stack buildSVG(Gc::Ref<Dom::Element> el) {
    Scene::Stack stack;
    for (auto child = el->firstChild(); child; child = child->nextSibling()) {
        if (auto el = child->is<Dom::Element>()) {
            buildSVGElement(stack, *el);
        } else {
            // TODO
        }
    }
    return stack;
}

} // namespace Vaev::Layout
