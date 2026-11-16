module;

#include <karm/macros>

export module Vaev.Engine:xml.parser;

import Karm.Core;
import Karm.Gc;
import Karm.Logger;
import Karm.Xml;

import :dom.document;
import :dom.comment;
import :dom.documentType;

using namespace Karm;

namespace Vaev::Xml {

static Dom::QualifiedName bridge(Karm::Xml::QualifiedName const& name) {
    return {name.ns, name.name};
}

export struct XmlParser {
    Gc::Heap& _heap;

    Res<> parse(Io::SScan& s, Opt<Symbol> const& ns, Dom::Document& doc) {
        Opt<Gc::Ref<Dom::Element>> root = NONE;
        Vec<Gc::Ref<Dom::Element>> stack;

        for (auto event : Karm::Xml::Parser::parse(s, ns)) {
            try$(event.visit(
                [&](Karm::Xml::StartElement& e) -> Res<> {
                    auto el = _heap.alloc<Dom::Element>(bridge(e.name));
                    for (auto& attr : e.attrs)
                        el->setAttribute(bridge(attr.name), std::move(attr.value));

                    if (stack.len())
                        last(stack)->appendChild(el);
                    else
                        root = Some(el);

                    stack.pushBack(el);
                    return Ok();
                },
                [&](Karm::Xml::EndElement&) -> Res<> {
                    (void)stack.popBack();
                    if (not stack.len())
                        doc.appendChild(root.expect());
                    return Ok();
                },
                [&](Karm::Xml::Text& e) -> Res<> {
                    last(stack)->appendChild(_heap.alloc<Dom::Text>(std::move(e.data)));
                    return Ok();
                },
                [&](Karm::Xml::Comment& e) -> Res<> {
                    auto comment = _heap.alloc<Dom::Comment>(std::move(e.data));
                    if (stack.len())
                        last(stack)->appendChild(comment);
                    else
                        doc.appendChild(comment);
                    return Ok();
                },
                [&](Karm::Xml::Doctype& e) -> Res<> {
                    doc.appendChild(_heap.alloc<Dom::DocumentType>(e.name, e.publicId, e.systemId));
                    return Ok();
                },
                [&](Karm::Xml::XmlDecl& e) -> Res<> {
                    logWarnIf(e.version != "1.0"s, "Version {} not supported, treating this document as a 1.0 document", e.version);
                    doc.xmlVersion = e.version;
                    if (auto const& [encoding] = e.encoding)
                        doc.xmlEncoding = encoding;
                    if (auto const& [standalone] = e.standalone)
                        doc.xmlStandalone = standalone;
                    return Ok();
                },
                [](Error& e) -> Res<> {
                    return e;
                }
            ));
        }

        return Ok();
    }
};

} // namespace Vaev::Xml
