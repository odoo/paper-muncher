export module Vaev.Engine:dom.node;

import Vaev.Idl;
import Karm.Ref;
import Karm.Gc;
import :dom.tree;

namespace Vaev::Dom {

export struct Document;

#define FOREACH_NODE_TYPE(TYPE)     \
    TYPE(ELEMENT, 1)                \
    TYPE(ATTRIBUTE, 2)              \
    TYPE(TEXT, 3)                   \
    TYPE(CDATA_SECTION, 4)          \
    TYPE(PROCESSING_INSTRUCTION, 7) \
    TYPE(COMMENT, 8)                \
    TYPE(DOCUMENT, 9)               \
    TYPE(DOCUMENT_TYPE, 10)         \
    TYPE(DOCUMENT_FRAGMENT, 11)

export enum struct NodeType {
#define ITER(NAME, VALUE) NAME = VALUE,
    FOREACH_NODE_TYPE(ITER)
#undef ITER
        _LEN,
};

// https://dom.spec.whatwg.org/#interface-node
export struct Node : Idl::PlatformObject, Tree<Node> {
    using PlatformObject::is;

    virtual ~Node() = default;

    // https://dom.spec.whatwg.org/#dom-node-nodetype
    virtual NodeType nodeType() const = 0;

    bool is(Meta::Id id) const override {
        return id == Meta::idOf<Node>() or PlatformObject::is(id);
    }

    Ref::Url baseURI();

    Gc::Ptr<Document> ownerDocument();
    Gc::Ptr<Document const> ownerDocument() const;

    // https://dom.spec.whatwg.org/#get-text-content
    virtual void getTextContent(StringBuilder&) const {};

    // https://dom.spec.whatwg.org/#dom-node-textcontent
    String textContent() const {
        StringBuilder sb;
        getTextContent(sb);
        return sb.take();
    }

    virtual void _repr(Io::Emit&) const {}

    void repr(Io::Emit& e) const;

    void hash(Meta::Derive<Hasher> auto& h) const {
        Karm::hash(h, reinterpret_cast<usize>(this));
    }

    bool operator==(Node const& other) const {
        return this == &other;
    }

    auto operator<=>(Node const& other) const {
        return this <=> &other;
    }
};

} // namespace Vaev::Dom
