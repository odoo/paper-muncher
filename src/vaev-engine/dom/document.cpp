export module Vaev.Engine:dom.document;

import Karm.Gc;
import Karm.Ref;
import Karm.Font;
import Karm.Sys;

import :dom.node;
import :dom.element;
import :props.base;
import :props.registry;

namespace Vaev::Style {

export struct StyleSheetList;

} // namespace Vaev::Style

namespace Vaev::Dom {

export struct Window;

export enum struct QuirkMode {
    NO,
    LIMITED,
    YES
};

// https://dom.spec.whatwg.org/#interface-document
export struct Document : Node {
    static constexpr auto TYPE = NodeType::DOCUMENT;

    Ref::Url _url;
    Ref::Uti _uti;
    QuirkMode quirkMode = QuirkMode::NO;

    String xmlVersion;
    String xmlEncoding;
    String xmlStandalone = "no"s; // https://www.w3.org/TR/xml/#NT-SDDecl

    // https://drafts.csswg.org/cssom/#dom-documentorshadowroot-stylesheets
    Gc::Ptr<Style::StyleSheetList> styleSheets;

    // https://drafts.css-houdini.org/css-properties-values-api/#dom-window-registeredpropertyset-slot
    Style::RegisteredPropertySet registeredPropertySet = Style::defaultRegistry();

    Opt<Rc<Font::Database>> fontDatabase;

    Document(Ref::Url url, Ref::Uti uti)
        : _url(url), _uti(uti) {
    }

    NodeType nodeType() const override {
        return TYPE;
    }

    String title() const {
        for (auto node : iterDepthFirst()) {
            if (auto element = node->is<Element>())
                if (element->qualifiedName == Html::TITLE_TAG) {
                    return element->textContent();
                }
        }
        return ""s;
    }

    Ref::Url const& url() const {
        return _url;
    }

    // https://dom.spec.whatwg.org/#ref-for-dom-document-contenttype%E2%91%A0
    Ref::Uti contentType() const {
        return _uti;
    }

    // https://dom.spec.whatwg.org/#document-element
    Gc::Ptr<Element> documentElement() const {
        for (auto child = firstChild(); child; child = child->nextSibling())
            if (auto el = child->is<Element>())
                return el;
        return nullptr;
    }

    // https://html.spec.whatwg.org/multipage/dom.html#dom-document-body-dev
    Gc::Ptr<Element> body() const {
        auto document = documentElement();

        if (not document)
            return nullptr;

        if (document->qualifiedName != Html::HTML_TAG)
            return nullptr;

        for (auto child : document->iterChildren()) {
            if (auto el = child->is<Element>();
                el and el->qualifiedName == Html::BODY_TAG)
                return el;
        }

        return nullptr;
    }

    Rc<Style::ComputedValues> initialComputedValues() const {
        auto initialStyle = registeredPropertySet.initialComputedValues();
        initialStyle->setCustomProp("-vaev-url", {Css::Token::string(Io::format("\"{}\"", url()))});
        initialStyle->setCustomProp("-vaev-title", {Css::Token::string(Io::format("\"{}\"", title()))});
        initialStyle->setCustomProp("-vaev-datetime", {Css::Token::string(Io::format("\"{}\"", Sys::now()))});
        return initialStyle;
    }
};

} // namespace Vaev::Dom
