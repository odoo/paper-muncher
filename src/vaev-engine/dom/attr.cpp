module;

#include <karm-io/emit.h>

export module Vaev.Engine:dom.attr;

import :dom.node;
import :dom.names;

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#interface-attr
export struct Attr : Node {
    QualifiedName qualifiedName;
    String value;

    Attr(QualifiedName const& qualifiedName, String const& value)
        : qualifiedName(qualifiedName), value(value) {
    }

    NodeType nodeType() const override {
        return NodeType::ATTRIBUTE;
    }

    void _repr(Io::Emit& e) const override {
        e(" qualifiedName={} value={#}", qualifiedName, value);
    }
};

} // namespace Vaev::Dom
