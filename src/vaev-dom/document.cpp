#include "document.h"

#include "element.h"

namespace Vaev::Dom {

String Document::title() {
    for (auto node : iterDepthFirst()) {
        if (auto element = node->is<Element>())
            if (element->tagName == Html::TITLE) {
                return element->textContent();
            }
    }
    return ""s;
}

Gc::Ptr<Dom::Element> Document::documentElement() const {
    for (auto child = firstChild(); child; child = child->nextSibling())
        if (auto el = child->is<Element>())
            return el;
    return nullptr;
}

} // namespace Vaev::Dom
