export module Vaev.Engine:dom.attr;

import Karm.Core;

import :dom.names;

using namespace Karm;

namespace Vaev::Dom {

// https://dom.spec.whatwg.org/#interface-attr
//
// Not a Node: nothing in this codebase ever traverses an attribute as part of
// the tree or dispatches on it polymorphically (it's stored inline in the
// owning element's attribute list, not linked in as a child) — inheriting
// Node's vtable pointer and Tree<Node> parent/sibling/child links bought
// nothing but per-attribute overhead.
export struct Attr {
    // Short values — the overwhelming common case: ids, classes, colspan,
    // name attributes, ordinary hrefs — are interned, so identical values
    // repeated across many elements (very common in generated markup) share
    // one allocation instead of paying for a new heap string per element.
    // Long ones (data: URIs, inline SVG path data) fall back to an owned
    // String: Symbol's intern table is a slab that's never freed for the
    // life of the process, so interning something megabytes-long would
    // panic (a single entry can't cross the slab's chunk size) or bloat
    // that table forever — and such values are typically unique per
    // element anyway, so interning wouldn't help even if it were safe.
    static constexpr usize INTERN_LIMIT = 256;

    QualifiedName qualifiedName;
    Union<Symbol, String> value;

    static Union<Symbol, String> intern(Str value) {
        if (value.len() <= INTERN_LIMIT)
            return Symbol::from(value);
        return String{value};
    }

    Attr(QualifiedName const& qualifiedName, Str value)
        : qualifiedName(qualifiedName), value(intern(value)) {
    }

    Str str() const {
        return value.visit(
            [](Symbol const& s) -> Str {
                return s.str();
            },
            [](String const& s) -> Str {
                return s;
            }
        );
    }

    void repr(Io::Emit& e) const {
        e(" qualifiedName={} value={:#}", qualifiedName, str());
    }
};

} // namespace Vaev::Dom
