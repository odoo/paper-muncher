export module Vaev.Engine:style.select.qualifiedName;

import Karm.Core;

import :dom.element;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Style {

export struct NullNamespace {
    bool operator==(NullNamespace const&) const = default;
};

export constexpr NullNamespace NULL_NAMESPACE = {};

export struct Universal {
    bool operator==(Universal const&) const = default;
};

export constexpr Universal UNIVERSAL = {};

// https://infra.spec.whatwg.org/#ascii-lowercase
static Symbol _asciiLowercase(Symbol sym) {
    StringBuilder sb;
    for (auto rune : iterRunes(sym.str()))
        sb.append(toAsciiLower(rune));
    return Symbol::from(sb.take());
}

export struct QualifiedNameSelector {
    using NamespacePattern = Union<
        NullNamespace, // represents the name B that belongs to no namespace.
        Universal,     // represents the name C in any namespace, including no namespace.
        Symbol>;

    using NamePattern = Union<Universal, Symbol>;

    NamespacePattern ns; // Null namespace, universal namespace, or an exact namespace.
    NamePattern name;    // Universal name or an exact name.
    NamePattern asciiLowercaseName;

    QualifiedNameSelector()
        : ns(UNIVERSAL), name(UNIVERSAL), asciiLowercaseName(UNIVERSAL) {}

    QualifiedNameSelector(NamespacePattern ns, NamePattern name)
        : ns(ns), name(name), asciiLowercaseName(name) {
        if (auto exact = name.is<Symbol>())
            asciiLowercaseName = _asciiLowercase(*exact);
    }

    QualifiedNameSelector(Symbol ns, Symbol name)
        : QualifiedNameSelector(NamespacePattern{ns}, NamePattern{name}) {}

    static QualifiedNameSelector wildcard() {
        return {UNIVERSAL, UNIVERSAL};
    }

    Opt<Symbol> exactName(bool asciiLowercase = false) const {
        if (auto exact = (asciiLowercase ? asciiLowercaseName : name).is<Symbol>())
            return Some(*exact);
        return NONE;
    }

    bool match(Dom::QualifiedName const& q, bool asciiLowercase = false) const {
        if (
            not ns.visit(
                [&](NullNamespace const&) {
                    return q.ns == NONE;
                },
                [&](Universal const&) {
                    return true;
                },
                [&](Symbol const& exact) {
                    return q.ns == exact;
                }
            )
        )
            return false;

        auto const& namePattern = asciiLowercase ? asciiLowercaseName : name;
        if (
            not namePattern.visit(
                [&](Universal const&) {
                    return true;
                },
                [&](Symbol const& exact) {
                    return q.name == exact;
                }
            )
        )
            return false;

        return true;
    }

    Dom::QualifiedName fullyQualified() const {
        auto exactName = this->exactName();

        if (ns.is<Universal>() or not exactName)
            panic("cannot fully qualify a wildcard selector");

        if (ns.is<NullNamespace>())
            return {NONE, exactName.expect()};

        return {Some(*ns.is<Symbol>()), exactName.expect()};
    }

    bool operator==(QualifiedNameSelector const&) const = default;

    void repr(Io::Emit& e) const {
        auto exactName = this->exactName();

        if (ns.is<Universal>() and exactName)
            e("*|{}", exactName.expect());
        else if (auto exactNs = ns.is<Symbol>(); exactNs and exactName)
            e("{}|{}", *exactNs, exactName.expect());
        else if (exactName)
            e("{}", exactName.expect());
        else if (ns.is<Universal>())
            e("*");
        else if (auto exactNs = ns.is<Symbol>())
            e("{}|*", *exactNs);
        else
            e("|*");
    }
};

} // namespace Vaev::Style
