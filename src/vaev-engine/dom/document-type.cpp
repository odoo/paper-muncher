module;

#include <karm-io/emit.h>

export module Vaev.Engine:dom.document_type;

import :dom.node;

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#interface-documenttype
export struct DocumentType : Node {
    static constexpr auto TYPE = NodeType::DOCUMENT_TYPE;

    Symbol name = Symbol::EMPTY;
    String publicId;
    String systemId;

    DocumentType() = default;

    DocumentType(Symbol name, String publicId, String systemId)
        : name(name), publicId(publicId), systemId(systemId) {
    }

    NodeType nodeType() const override {
        return TYPE;
    }

    void _repr(Io::Emit& e) const override {
        e(" name={#} publicId={#} systemId={#}", this->name, this->publicId, this->systemId);
    }
};

} // namespace Vaev::Dom
