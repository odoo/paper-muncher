#pragma once

#include <karm-base/list.h>
#include <karm-base/rc.h>
#include <karm-base/vec.h>
#include <karm-io/emit.h>

namespace Web::Dom {

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

enum struct NodeType {
#define ITER(NAME, VALUE) NAME = VALUE,
    FOREACH_NODE_TYPE(ITER)
#undef ITER
};

static inline Str toStr(NodeType type) {
    switch (type) {
#define ITER(NAME, VALUE) \
    case NodeType::NAME:  \
        return #NAME;
        FOREACH_NODE_TYPE(ITER)
#undef ITER
    }
    panic("unreachable");
}

// https://dom.spec.whatwg.org/#interface-node
struct Node : public Meta::Static {
    Node *_parent = nullptr;
    LlItem<Node> _siblings;
    Vec<Strong<Node>> _children;

    virtual ~Node() = default;

    virtual NodeType nodeType() const = 0;

    /* --- Parent --- */

    Node &parentNode() {
        if (not _parent)
            panic("node has no parent");
        return *_parent;
    }

    usize _parentIndex() {
        return indexOf(parentNode()._children, *this).unwrap();
    }

    void _detachParent() {
        if (_parent) {
            _parent->_children.removeAt(_parentIndex());
            _parent = nullptr;
        }
    }

    /* --- Children --- */

    bool hasChildren() {
        return _children.len() > 0;
    }

    Strong<Node> firstChild() {
        if (not _children.len())
            panic("node has no children");
        return first(_children);
    }

    Strong<Node> lastChild() {
        if (not _children.len())
            panic("node has no children");
        return last(_children);
    }

    void appendChild(Strong<Node> child) {
        child->_detachParent();
        _children.pushBack(child);
        child->_parent = this;
    }

    void removeChild(Strong<Node> child) {
        if (child->_parent != this)
            panic("node is not a child");
        child->_detachParent();
    }

    /* --- Siblings --- */

    Strong<Node> previousSibling() {
        usize index = _parentIndex();
        return parentNode()._children[index - 1];
    }

    Strong<Node> nextSibling() {
        usize index = _parentIndex();
        return parentNode()._children[index + 1];
    }

    void _dumpChildren(Io::Emit &e) const {
        if (_children.len() > 0) {
            e.indentNewline();
            for (auto &child : _children) {
                child->dump(e);
            }
            e.deindent();
        }
    }

    virtual void _dump(Io::Emit &) const {}

    void dump(Io::Emit &e) const {
        e("({}", Io::toParamCase(toStr(nodeType())));
        _dump(e);
        _dumpChildren(e);
        e(")\n");
    }

    /* --- Operators --- */

    bool operator==(Node const &other) const {
        return this == &other;
    }

    auto operator<=>(Node const &other) const {
        return this <=> &other;
    }
};

} // namespace Web::Dom
