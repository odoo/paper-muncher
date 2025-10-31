export module Vaev.Engine:dom.element;

import Karm.Core;
import Karm.Scene;
import :dom.attr;
import :dom.node;
import :dom.names;
import :dom.text;
import :dom.tokenList;

using namespace Karm;

namespace Vaev::Style {
export struct SpecifiedValues;

} // namespace Vaev::Style

namespace Vaev::Dom {

// https://drafts.csswg.org/css-pseudo/#CSSPseudoElement-interface
export struct PseudoElement {
    // https://drafts.csswg.org/css-pseudo/#dom-csspseudoelement-type
    static Symbol const BEFORE;
    static Symbol const AFTER;
    static Symbol const MARKER;

    Symbol type;
    Opt<Rc<Style::SpecifiedValues>> _specifiedValues = NONE;

    Rc<Style::SpecifiedValues> specifiedValues() const {
        return _specifiedValues.unwrap("unstyled pseudo-element");
    }
};

Symbol const PseudoElement::BEFORE = "::before"_sym;
Symbol const PseudoElement::AFTER = "::after"_sym;
Symbol const PseudoElement::MARKER = "::marker"_sym;

// https://dom.spec.whatwg.org/#interface-element
export struct Element : Node {
    static constexpr auto TYPE = NodeType::ELEMENT;

    QualifiedName qualifiedName;
    // NOSPEC: Should be a NamedNodeMap
    Map<QualifiedName, Rc<Attr>> attributes;
    Opt<Rc<Style::SpecifiedValues>> _specifiedValues; // FIXME: We should not have this store here
    TokenList classList;
    Opt<Rc<Scene::Node>> imageContent;
    Map<Symbol, Rc<PseudoElement>> _pseudoElements;

    // MARK: Node --------------------------------------------------------------

    Element(QualifiedName const& qualifiedName)
        : qualifiedName(qualifiedName) {
    }

    NodeType nodeType() const override {
        return TYPE;
    }

    void _repr(Io::Emit& e) const override {
        e(" qualifiedName={}", qualifiedName);
        if (this->attributes.len()) {
            e.indentNewline();
            for (auto const& [name, attr] : this->attributes.iterUnordered()) {
                attr->repr(e);
            }
            e.deindent();
        }
    }

    // MARK: Name --------------------------------------------------------------

    Symbol namespaceUri() const {
        return qualifiedName.ns;
    }

    Symbol localName() const {
        return qualifiedName.name;
    }

    // MARK: Attributes --------------------------------------------------------

    Opt<Str> id() const {
        return getAttributeUnqualified("id"_sym);
    }

    Opt<Str> style() const {
        return getAttributeUnqualified("style"_sym);
    }

    void setAttribute(QualifiedName name, String value) {
        if (name == Html::CLASS_ATTR or name == Svg::CLASS_ATTR) {
            for (auto class_ : iterSplit(value, ' ')) {
                this->classList.add(class_);
            }
            return;
        }
        this->attributes.put(name, makeRc<Attr>(name, value));
    }

    bool hasAttribute(QualifiedName name) const {
        return this->attributes.tryGet(name) != NONE;
    }

    bool hasAttributeUnqualified(Str name) const {
        for (auto const& [qualifiedName, _] : this->attributes.iterUnordered()) {
            if (qualifiedName.name.str() == name) {
                return true;
            }
        }
        return false;
    }

    Opt<Str> getAttribute(QualifiedName name) const {
        auto attr = this->attributes.tryGet(name);
        if (attr == NONE)
            return NONE;
        return (*attr)->value;
    }

    Opt<Str> getAttributeUnqualified(Symbol name) const {
        for (auto const& [qualifiedName, attr] : this->attributes.iterUnordered())
            if (qualifiedName.name == name)
                return attr->value;
        return NONE;
    }

    // MARK: Style -------------------------------------------------------------

    Rc<Style::SpecifiedValues> specifiedValues() const {
        return _specifiedValues.unwrap("unstyled element");
    }

    // MARK: Content -----------------------------------------------------------

    // https://dom.spec.whatwg.org/#concept-descendant-text-content
    String textContent() {
        StringBuilder sb;
        for (auto child : iterDepthFirst()) {
            if (auto text = child->is<Text>())
                sb.append(text->data());
        }
        return sb.take();
    }

    // MARK: Pseudo Elements ---------------------------------------------------

    void clearPseudoElement() {
        _pseudoElements.clear();
    }

    bool hasPseudoElement(Symbol type) const {
        return _pseudoElements.has(type);
    }

    void addPseudoElement(Rc<PseudoElement> pseudoElement) {
        _pseudoElements.put(pseudoElement->type, pseudoElement);
    }

    Opt<Rc<PseudoElement>> getPseudoElement(Symbol type) const {
        return _pseudoElements.tryGet(type);
    }
};

} // namespace Vaev::Dom
