#pragma once

#include "attr.h"
#include "node.h"
#include "tags.h"
#include "text.h"
#include "token-list.h"

namespace Vaev::Style {
struct ComputedValues;
struct SpecifiedValues;
} // namespace Vaev::Style

namespace Vaev::Dom {

struct PseudoElement {
    Opt<Rc<Style::ComputedValues>> _computedValues = NONE;
    Opt<Rc<Style::SpecifiedValues>> _specifiedValues = NONE;

    Rc<Style::ComputedValues> computedValues() const {
        return _computedValues.unwrap("unstyled pseudo-element");
    }

    Rc<Style::SpecifiedValues> specifiedValues() const {
        return _specifiedValues.unwrap("unstyled pseudo-element");
    }
};

// https://dom.spec.whatwg.org/#interface-element
struct Element : Node {
    static constexpr auto TYPE = NodeType::ELEMENT;

    Opt<Str> id() const {
        if (tagName.ns == HTML)
            return this->getAttribute(Html::ID_ATTR);
        else if (tagName.ns == SVG)
            return this->getAttribute(Svg::ID_ATTR);
        else
            return NONE;
    }

    Opt<Str> style() const {
        if (tagName.ns == HTML)
            return this->getAttribute(Html::STYLE_ATTR);
        else if (tagName.ns == SVG)
            return this->getAttribute(Svg::STYLE_ATTR);
        else
            return NONE;
    }

    TagName tagName;
    // NOSPEC: Should be a NamedNodeMap
    Map<AttrName, Rc<Attr>> attributes;
    Opt<Rc<Style::ComputedValues>> _computedValues;
    Opt<Rc<Style::SpecifiedValues>> _specifiedValues; // FIXME: We should not have this store here
    TokenList classList;

    Element(TagName tagName)
        : tagName(tagName) {
    }

    NodeType nodeType() const override {
        return TYPE;
    }

    Rc<Style::ComputedValues> computedValues() const {
        return _computedValues.unwrap("unstyled element");
    }

    Rc<Style::SpecifiedValues> specifiedValues() const {
        return _specifiedValues.unwrap("unstyled element");
    }

    String textContent() const {
        String builder;
        if (not hasChildren())
            return ""s;

        if (firstChild() != lastChild())
            panic("textContent is not implemented for elements with multiple children");

        if (auto text = firstChild()->is<Text>())
            return text->data();

        panic("textContent is not implemented for elements with children other than text nodes");
    }

    void _repr(Io::Emit& e) const override {
        e(" tagName={#}", this->tagName);
        if (this->attributes.len()) {
            e.indentNewline();
            for (auto const& [name, attr] : this->attributes.iter()) {
                attr->repr(e);
            }
            e.deindent();
        }
    }

    void setAttribute(AttrName name, String value) {
        if (name == Html::CLASS_ATTR or name == Svg::CLASS_ATTR) {
            for (auto class_ : iterSplit(value, ' ')) {
                this->classList.add(class_);
            }
            return;
        }
        auto attr = makeRc<Attr>(name, value);
        this->attributes.put(name, attr);
    }

    bool hasAttribute(AttrName name) const {
        return this->attributes.tryGet(name) != NONE;
    }

    Opt<Str> getAttribute(AttrName name) const {
        auto attr = this->attributes.tryGet(name);
        if (attr == NONE)
            return NONE;
        return (*attr)->value;
    }
};

} // namespace Vaev::Dom
