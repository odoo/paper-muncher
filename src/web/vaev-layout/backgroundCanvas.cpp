module;

#include <karm-image/loader.h>
#include <karm-text/loader.h>
#include <karm-text/prose.h>
#include <vaev-dom/document.h>
#include <vaev-style/computer.h>

export module Vaev.Layout:backgroundCanvas;

import :values;

namespace Vaev::Layout {

Gfx::Color ColorToGfx(Color color) {
    if (auto isRes = color.is<Gfx::Color>()) {
        return *isRes;
    } else {
        logWarn("color was not a Gfx color");
        return Gfx::WHITE;
    }
}

export Gfx::Color BGSearch(Style::Computer& c, Gc::Ref<Dom::Document> doc) {
    auto el = doc->documentElement();
    if (!el) {
        return Gfx::WHITE;
    }

    auto style = c.computeFor(Style::Computed::initial(), *el);
    if (style->backgrounds->color != Gfx::ALPHA) {
        style->backgrounds.cow().color = Gfx::PINK;
        return ColorToGfx(style->backgrounds->color);
    }

    for (auto child = el->firstChild(); child; child = child->nextSibling()) {
        if (auto isRes = child->is<Dom::Element>()) {
            if (isRes->tagName != Html::BODY) {
                continue;
            }

            auto childStyle = c.computeFor(*style, *isRes);
            if (childStyle->backgrounds->color != Gfx::ALPHA) {
                childStyle->backgrounds.cow().color = Gfx::PINK;
                return ColorToGfx(childStyle->backgrounds->color);
            } else {
                return Gfx::WHITE;
            }
        }
    }

    return Gfx::WHITE;
}

} // namespace Vaev::Layout
