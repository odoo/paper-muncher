export module Vaev.Engine:dom.element;

import Karm.Core;
import Karm.Scene;
import :dom.attr;
import :dom.node;
import :dom.names;
import :dom.text;
import :dom.tokenList;
import :style.counter;

using namespace Karm;

namespace Vaev::Style {
export struct ComputedValues;
} // namespace Vaev::Style

namespace Vaev::Dom {

export struct Element;
export struct PseudoElement;

// https://drafts.csswg.org/css-pseudo/#CSSPseudoElement-interface
export struct PseudoElement {
    // https://drafts.csswg.org/css-pseudo/#dom-csspseudoelement-type
    static Symbol const BEFORE;
    static Symbol const AFTER;
    static Symbol const MARKER;
    Symbol type;
    
    // https://drafts.csswg.org/css-pseudo/#dom-csspseudoelement-parent
    Gc::Ptr<Element> parent;
    
    Opt<Rc<Style::ComputedValues>> _computedValues = NONE;
    Style::CounterSet counters = {};

    PseudoElement(Symbol type, Rc<Style::ComputedValues> computedValues)
        : type(type), _computedValues(computedValues) {}

    Rc<Style::ComputedValues> computedValues() const {
        return _computedValues.unwrap("unstyled pseudo-element");
    }

    // https://drafts.csswg.org/css-pseudo/#dom-csspseudoelement-element
    // https://drafts.csswg.org/selectors-4/#ultimate-originating-element
    Gc::Ref<Element> element() const {
        return parent.upgrade();
    }

    void repr(Io::Emit& e) const {
        e("(pseudo-element {})", type);
    }

    bool operator==(PseudoElement const& other) const {
        return this == &other;
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
    Opt<Rc<Style::ComputedValues>> _computedValues;
    TokenList classList;
    Opt<Rc<Scene::Node>> imageContent;
    Map<Symbol, Rc<PseudoElement>> _pseudoElements;
    Style::CounterSet counters;

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
            for (auto const& [name, attr] : this->attributes.iterItems()) {
                attr->repr(e);
            }
            e.deindent();
        }
    }

    // MARK: Name --------------------------------------------------------------

    Opt<Symbol> namespaceUri() const {
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
        return this->attributes.contains(name);
    }

    bool hasAttributeUnqualified(Str name) const {
        for (auto const& [qualifiedName, _] : this->attributes.iterItems()) {
            if (qualifiedName.name.str() == name) {
                return true;
            }
        }
        return false;
    }

    Opt<Str> getAttribute(QualifiedName name) const {
        auto attr = this->attributes.lookup(name);
        if (attr == NONE)
            return NONE;
        return (*attr)->value;
    }

    Opt<Str> getAttributeUnqualified(Symbol name) const {
        for (auto const& [qualifiedName, attr] : this->attributes.iterItems())
            if (qualifiedName.name == name)
                return attr->value;
        return NONE;
    }

    // MARK: Style -------------------------------------------------------------

    Rc<Style::ComputedValues> computedValues() const {
        return _computedValues.unwrap("unstyled element");
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

    // MARK: Element -----------------------------------------------------------

    // https://html.spec.whatwg.org/multipage/syntax.html#void-elements
    bool isVoidElement() const {
        return qualifiedName == Html::AREA_TAG or
               qualifiedName == Html::BASE_TAG or
               qualifiedName == Html::BR_TAG or
               qualifiedName == Html::COL_TAG or
               qualifiedName == Html::EMBED_TAG or
               qualifiedName == Html::HR_TAG or
               qualifiedName == Html::IMG_TAG or
               qualifiedName == Html::INPUT_TAG or
               qualifiedName == Html::LINK_TAG or
               qualifiedName == Html::META_TAG or
               qualifiedName == Html::SOURCE_TAG or
               qualifiedName == Html::TRACK_TAG or
               qualifiedName == Html::WBR_TAG;
    }

    // MARK: Pseudo Elements ---------------------------------------------------

    void clearPseudoElement() {
        _pseudoElements.clear();
    }

    bool hasPseudoElement(Symbol type) const {
        return _pseudoElements.contains(type);
    }

    void addPseudoElement(Rc<PseudoElement> pseudoElement) {
        pseudoElement->parent = *this;
        _pseudoElements.put(pseudoElement->type, pseudoElement);
    }

    Opt<Rc<PseudoElement>> getPseudoElement(Symbol type) const {
        return _pseudoElements.lookup(type);
    }
};

export struct OriginatingElement : Union<Gc::Ref<Element>, Rc<PseudoElement>> {
    using Union::Union;

    Rc<Style::ComputedValues> computedValues() {
        return visit([](auto& el) {
            return el->computedValues();
        });
    }

    void repr(Io::Emit& e) const {
        e("{}", static_cast<Union const&>(*this));
    }
};

} // namespace Vaev::Dom
