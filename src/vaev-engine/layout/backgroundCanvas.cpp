export module Vaev.Engine:layout.backgroundCanvas;

import Karm.Image;
import Karm.Gc;
import Karm.Font;
import Karm.Gfx;
import Karm.Math;
import Karm.Logger;

import :style;
import :dom.element;
import :layout.values;

namespace Vaev::Layout {

// NOTE This handle only Gfx::Colors but there's a lot more to handle eg Images, Gradients, etc.
Gfx::Color _colorToGfx(Color color) {
    if (auto isRes = color.is<Gfx::Color>()) {
        return *isRes;
    } else {
        logWarn("color was not a Gfx color");
        return Gfx::WHITE;
    }
}

void _patchBackgrounds(MutSlice<Layout::Box>& children) {
    for (auto& child : children) {
        if (child.origin->qualifiedName == Html::BODY_TAG) {
            child.style->backgrounds.cow().color = Gfx::ALPHA;
        }
    }
}

export Gfx::Color fixupBackgrounds(Style::Computer& c, Gc::Ref<Dom::Document> doc, Layout::Tree& tree) {
    auto el = doc->documentElement();
    if (!el) {
        return Gfx::WHITE;
    }

    if (doc->documentElement()->namespaceUri() != Html::NAMESPACE)
        return Gfx::ALPHA;

    auto style = c.computeFor(Style::SpecifiedValues::initial(), *el);
    if (style->backgrounds->color != Gfx::ALPHA) {
        tree.root.style->backgrounds.cow().color = Gfx::ALPHA;
        return _colorToGfx(style->backgrounds->color);
    }

    for (auto child = el->firstChild(); child; child = child->nextSibling()) {
        if (auto isRes = child->is<Dom::Element>()) {
            if (isRes->qualifiedName != Html::BODY_TAG) {
                continue;
            }

            auto childStyle = c.computeFor(*style, *isRes);
            if (childStyle->backgrounds->color != Gfx::ALPHA) {
                auto children = tree.root.children();
                _patchBackgrounds(children);
                return _colorToGfx(childStyle->backgrounds->color);
            } else {
                return Gfx::WHITE;
            }
        }
    }

    return Gfx::WHITE;
}

} // namespace Vaev::Layout
