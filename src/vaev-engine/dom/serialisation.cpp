export module Vaev.Engine:dom.serialisation;

import Karm.Core;
import Karm.Gc;

import :dom.element;
import :dom.comment;
import :dom.document;
import :dom.documentType;

using namespace Karm;

namespace Vaev::Dom {

// MARK: Escaping --------------------------------------------------------------
// https://html.spec.whatwg.org/multipage/parsing.html#escapingString

export void escapeString(Io::Emit& e, Io::SScan& s, bool attributeMode = false) {
    while (not s.ended()) {
        auto r = s.next();
        // Replace any occurrence of the "&" character by the string "&amp;".
        if (r == '&')
            e("&amp;");

        // Replace any occurrences of the U+00A0 NO-BREAK SPACE character by the string "&nbsp;".
        else if (r == U'\xA0')
            e("&nbsp;");

        // Replace any occurrences of the "<" character by the string "&lt;".
        else if (r == '<')
            e("&lt;");

        // Replace any occurrences of the ">" character by the string "&gt;".
        else if (r == '>')
            e("&gt;");

        // If the algorithm was invoked in the attribute mode, then replace any occurrences of the """ character by the string "&quot;".
        else if (attributeMode and r == '"')
            e("&quot;");

        else
            e(r);
    }
}

export void escapeString(Io::Emit& e, Str str, bool attributeMode = false) {
    Io::SScan s{str};
    escapeString(e, s, attributeMode);
}

// MARK: Serialize -------------------------------------------------------------
// https://html.spec.whatwg.org/multipage/parsing.html#serialising-html-fragments

// https://html.spec.whatwg.org/multipage/parsing.html#serializes-as-void
bool _serializeAsVoid(Gc::Ref<Node> node) {
    auto el = node->is<Element>();
    if (not el)
        return false;

    // For the purposes of the following algorithm, an element serializes as void
    // if its element type is one of the void elements, or is basefont, bgsound, frame, keygen, or param.
    return el->isVoidElement() or
           el->qualifiedName == Html::BASEFONT_TAG or
           el->qualifiedName == Html::BGSOUND_TAG or
           el->qualifiedName == Html::FRAME_TAG or
           el->qualifiedName == Html::KEYGEN_TAG or
           el->qualifiedName == Html::PARAM_TAG;
}

// https://html.spec.whatwg.org/multipage/parsing.html#html-fragment-serialisation-algorithm
export void serializeHtmlFragment(Gc::Ref<Node> node, Io::Emit& e) {
    // 1. If the node serializes as void, then return the empty string.
    if (_serializeAsVoid(node))
        return;

    // 3. If the node is a template element, then let the node instead be the template element's template contents (a DocumentFragment node).
    // TODO: We don't support DocumentFragment

    // 4. If current node is a shadow host, then:
    //    1. Let shadow be current node's shadow root.
    //    2. If serializableShadowRoots is true and shadow’s serializable is true, or shadowRoots contains shadow, then:
    //       1. Append "<template shadowrootmode="".
    //       2. If shadow’s mode is "open", append "open". Otherwise, append "closed".
    //       3. Append """.
    //       4. If shadow’s delegates focus is set, append " shadowrootdelegatesfocus=""".
    //       5. If shadow’s serializable is set, append " shadowrootserializable=""".
    //       6. If shadow’s clonable is set, append " shadowrootclonable=""".
    //       7. If current node’s custom element registry is not shadow’s custom element registry, append " shadowrootcustomelementregistry=""".
    //       8. Append ">".
    //       9. Append the value of running the HTML fragment serialization algorithm with shadow, serializableShadowRoots, and shadowRoots.
    //       10. Append "</template>".
    // TODO: We don't have shadow dom support

    // 5. For each child node of the node, in tree order:
    //    1. Let current node be the child node being processed.
    for (auto currentNode : node->iterChildren()) {
        //    2. Append the appropriate string:
        //       If current node is an Element:
        if (auto el = currentNode->is<Element>()) {
            // - Determine tagname: if in HTML, MathML, or SVG namespace, tagname is local name; otherwise qualified name.
            // - Append "<" followed by tagname.
            e("<{}", el->qualifiedName);

            // - If current node has an is value not present as an attribute, append ' is="<escaped is value>"'.
            if (auto isValue = el->getAttribute(Html::IS_ATTR)) {
                e(" is=\"");
                escapeString(e, isValue.unwrap(), true);
                e("\"");
            }
            // - For each attribute:
            for (auto& [name, attr] : el->attributes.iterUnordered()) {
                if (name == Html::IS_ATTR)
                    continue;
                //     Append space, attribute’s serialized name, "=", quote, escaped value, quote.
                e(" {}=\"", name);
                escapeString(e, attr->value, true);
                e("\"");
            }
            // - Append ">".
            e(">");

            // - If current node serializes as void, continue.
            if (_serializeAsVoid(currentNode))
                continue;

            // - Append the value of running this algorithm on current node
            serializeHtmlFragment(currentNode, e);

            // then "</tagname>".
            e("</{}>", el->qualifiedName);
        }

        // If current node is a Text node:
        else if (auto text = currentNode->is<Text>()) {
            auto parent = text->parentNode();
            // - If its parent is style, script, xmp, iframe, noembed, noframes, plaintext, or (if scripting enabled) noscript,
            if (auto parentElement = parent->is<Element>();
                parentElement and
                (parentElement->qualifiedName == Html::STYLE_TAG or
                 parentElement->qualifiedName == Html::SCRIPT_TAG or
                 parentElement->qualifiedName == Html::XMP_TAG or
                 parentElement->qualifiedName == Html::IFRAME_TAG or
                 parentElement->qualifiedName == Html::NOEMBED_TAG or
                 parentElement->qualifiedName == Html::NOFRAMES_TAG or
                 parentElement->qualifiedName == Html::PLAINTEXT_TAG or
                 parentElement->qualifiedName == Html::NOSCRIPT_TAG)) {
                // append text literally.
                e(text->data());
            }
            // - Otherwise append escaped text.
            else {
                escapeString(e, text->data());
            }
        }

        // If current node is a Comment:
        else if (auto comment = currentNode->is<Comment>()) {
            // - Append "<!--" + data + "-->".
            e("<!--{}-->", comment->data());
        }

        // If current node is a ProcessingInstruction:
        //  - Append "<?" + target + " " + data + ">".
        // TODO: We don't support ProcessingInstruction

        // If current node is a DocumentType:
        else if (auto doctype = currentNode->is<DocumentType>()) {
            // - Append "<!DOCTYPE " + name + ">".
            e("<!DOCTYPE {}>", doctype->name);
        }
    }
}

export String serializeHtmlFragment(Gc::Ref<Node> node) {
    // 1. If the node serializes as void, then return the empty string.
    if (_serializeAsVoid(node))
        return ""s;

    // 2. Let s be a string, and initialize it to the empty string.
    Io::StringWriter sw;
    Io::Emit e{sw};

    serializeHtmlFragment(node, e);

    // 6. Return s.
    return sw.take();
}

} // namespace Vaev::Dom
