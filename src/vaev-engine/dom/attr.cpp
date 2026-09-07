export module Vaev.Engine:dom.attr;

import Karm.Core;

import :dom.node;
import :dom.names;

using namespace Karm;

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#interface-attr
export struct Attr {
    QualifiedName qualifiedName;
    String value;

    Attr(QualifiedName const& qualifiedName, String const& value)
        : qualifiedName(qualifiedName), value(value) {
    }

    void repr(Io::Emit& e) const {
        e(" qualifiedName={} value={:#}", qualifiedName, value);
    }
};

} // namespace Vaev::Dom
