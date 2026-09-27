#include <karm/test>

import Karm.Gc;
import Karm.Ref;
import Vaev.Engine;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Xml::Tests {

test$("parse-empty-document") {
    Gc::Heap gc;
    auto s = Io::SScan(""s);
    Xml::XmlParser p{gc};
    assert$(not p._parseElement(s, Some(Html::NAMESPACE))); // An empty document is invalid
    return Ok();
}

test$("parse-open-close-tag") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};
    auto s = Io::SScan("<html></html>");
    auto root = try$(p._parseElement(s, Some(Html::NAMESPACE)));

    auto el = root->as<Dom::Element>();
    assertNe$(el, nullptr);
    assert$(el->qualifiedName == Html::HTML_TAG);

    return Ok();
}

test$("parse-empty-tag") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};
    auto s = Io::SScan("<html/>");
    try$(p._parseElement(s, Some(Html::NAMESPACE)));
    return Ok();
}

test$("parse-attr") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};
    auto s = Io::SScan("<html lang=\"en\"/>");
    auto root = try$(p._parseElement(s, Some(Html::NAMESPACE)));

    auto el = root->as<Dom::Element>();
    assertNe$(el, nullptr);
    assert$(el->hasAttribute(Html::LANG_ATTR));
    assert$(el->getAttribute(Html::LANG_ATTR) == "en");

    return Ok();
}

test$("parse-text") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};

    auto s = Io::SScan("<html>text</html>");
    auto root = try$(p._parseElement(s, Some(Html::NAMESPACE)));

    auto el = root->as<Dom::Element>();
    assertNe$(el, nullptr);
    assert$(el->hasChildren());

    auto text = el->firstChild()->as<Dom::Text>();
    assertNe$(text, nullptr);
    assert$(text->data() == "text");

    return Ok();
}

test$("parse-text-before-tag") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};

    auto s = Io::SScan("<html>text<div/></html>");
    auto root = try$(p._parseElement(s, Some(Html::NAMESPACE)));

    auto el = root->as<Dom::Element>();
    assertNe$(el, nullptr);
    assert$(el->hasChildren());

    auto text = el->firstChild()->as<Dom::Text>();
    assertNe$(text, nullptr);
    assert$(text->data() == "text");

    auto div = text->nextSibling()->as<Dom::Element>();
    assert$(div->nodeType() == Dom::NodeType::ELEMENT);
    assert$(div->qualifiedName == Html::DIV_TAG);

    return Ok();
}

test$("parse-text-after-tag") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};

    auto s = Io::SScan("<html><div/>text</html>");
    auto root = try$(p._parseElement(s, Some(Html::NAMESPACE)));

    auto el = root->as<Dom::Element>();
    assertNe$(el, nullptr);
    assert$(el->hasChildren());

    auto div = el->firstChild()->as<Dom::Element>();
    assertNe$(div, nullptr);
    assert$(div->qualifiedName == Html::DIV_TAG);

    auto text = div->nextSibling()->as<Dom::Text>();
    assertNe$(text, nullptr);
    assert$(text->data() == "text");

    return Ok();
}

test$("parse-text-between-tags") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};

    auto s = Io::SScan("<html><div/>text<div/></html>");
    auto root = try$(p._parseElement(s, Some(Html::NAMESPACE)));

    auto el = root->as<Dom::Element>();
    assertNe$(el, nullptr);
    assert$(el->hasChildren());

    auto div1 = el->firstChild()->as<Dom::Element>();
    assertNe$(div1, nullptr);
    assert$(div1->nodeType() == Dom::NodeType::ELEMENT);
    assert$(div1->qualifiedName == Html::DIV_TAG);

    auto text = div1->nextSibling()->as<Dom::Text>();
    assertNe$(text, nullptr);
    assert$(text->nodeType() == Dom::NodeType::TEXT);
    assert$(text->data() == "text");

    auto div2 = text->nextSibling()->as<Dom::Element>();
    assertNe$(div2, nullptr);
    assert$(div2->nodeType() == Dom::NodeType::ELEMENT);
    assert$(div2->qualifiedName == Html::DIV_TAG);

    return Ok();
}

test$("parse-text-between-tags-and-before") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};

    auto s = Io::SScan("<html>test2<div>text</div></html>");
    auto root = try$(p._parseElement(s, Some(Html::NAMESPACE)));
    auto el = root->as<Dom::Element>();
    assertNe$(el, nullptr);
    assert$(el->hasChildren());

    auto text1 = el->firstChild()->as<Dom::Text>();
    assertNe$(text1, nullptr);
    assert$(text1->nodeType() == Dom::NodeType::TEXT);
    assertEq$(text1->data(), "test2"s);

    auto div = text1->nextSibling()->as<Dom::Element>();
    assertNe$(div, nullptr);
    assert$(div->nodeType() == Dom::NodeType::ELEMENT);
    assert$(div->qualifiedName == Html::DIV_TAG);

    auto text2 = div->firstChild()->as<Dom::Text>();
    assertNe$(text2, nullptr);
    assert$(text2->nodeType() == Dom::NodeType::TEXT);
    assertEq$(text2->data(), "text"s);

    return Ok();
}

test$("parse-nested-tags") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};

    auto s = Io::SScan("<html><head></head><body></body></html>");
    auto root = try$(p._parseElement(s, Some(Html::NAMESPACE)));

    auto el = root->as<Dom::Element>();
    assertNe$(el, nullptr);
    assert$(el->hasChildren());

    auto head = el->firstChild()->as<Dom::Element>();
    assertNe$(head, nullptr);
    assert$(head->nodeType() == Dom::NodeType::ELEMENT);
    assert$(head->qualifiedName == Html::HEAD_TAG);

    auto body = head->nextSibling()->as<Dom::Element>();
    assertNe$(body, nullptr);
    assert$(body->nodeType() == Dom::NodeType::ELEMENT);
    assert$(body->qualifiedName == Html::BODY_TAG);

    return Ok();
}

test$("parse-comment") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};

    auto s = Io::SScan("<html><!-- comment --></html>");
    auto root = try$(p._parseElement(s, Some(Html::NAMESPACE)));

    auto el = root->as<Dom::Element>();
    assertNe$(el, nullptr);
    assert$(el->hasChildren());

    auto comment = el->firstChild()->as<Dom::Comment>();
    assertNe$(comment, nullptr);
    assert$(comment->nodeType() == Dom::NodeType::COMMENT);
    assert$(comment->data() == " comment "s);

    return Ok();
}

test$("parse-doctype") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};

    auto s = Io::SScan("<!DOCTYPE html><html></html>");

    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_XHTML);
    try$(p.parse(s, Some(Html::NAMESPACE), *dom));
    assert$(dom->hasChildren());

    auto doctype = dom->firstChild()->as<Dom::DocumentType>();
    assertNe$(doctype, nullptr);
    assert$(doctype->name == "html"s);

    return Ok();
}

test$("parse-title") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};

    auto s = Io::SScan("<title>the title</title>");
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_XHTML);
    try$(p.parse(s, Some(Html::NAMESPACE), *dom));
    assert$(dom->title() == "the title"s);
    return Ok();
}

test$("parse-comment-with-gt-symb") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};

    auto s = Io::SScan(
        "<title>im a title!</title>"
        "<!-- a b <meta> c d -->"
    );
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_XHTML);
    try$(p.parse(s, Some(Html::NAMESPACE), *dom));

    assert$(dom->hasChildren());
    auto title = dom->firstChild()->as<Dom::Element>();
    assertNe$(title, nullptr);
    assert$(title->nodeType() == Dom::NodeType::ELEMENT);
    assert$(title->qualifiedName == Html::TITLE_TAG);
    assert$(title->hasNextSibling());

    auto comment = title->nextSibling()->as<Dom::Comment>();
    assertNe$(comment, nullptr);
    assert$(comment->nodeType() == Dom::NodeType::COMMENT);
    assert$(comment->data() == " a b <meta> c d "s);

    return Ok();
}

test$("parse-xml-decl") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};

    auto s = Io::SScan("<?xml version='1.0' encoding='UTF-8'?><html></html>");
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_XHTML);
    try$(p.parse(s, Some(Html::NAMESPACE), *dom));
    assert$(dom->xmlVersion == "1.0");
    assert$(dom->xmlEncoding == "UTF-8");
    assert$(dom->xmlStandalone == "no");
    return Ok();
}

test$("parse-xml-different-namespace") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};

    auto s = Io::SScan(
        "<svg xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 0 0\">"
        "<rect/>"
        "</svg>"
    );
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_XHTML);
    try$(p.parse(s, Some(Html::NAMESPACE), *dom));

    auto svg = dom->firstChild()->as<Dom::Element>();
    assertNe$(svg, nullptr);
    assert$(svg->qualifiedName == Svg::SVG_TAG);
    assert$(svg->countChildren() == 1);
    assert$(svg->hasAttribute(Svg::VIEW_BOX_ATTR));

    auto rect = svg->firstChild()->as<Dom::Element>();
    assertNe$(rect, nullptr);
    assert$(rect->qualifiedName == Svg::RECT_TAG);

    return Ok();
}

test$("parse-xml-prefixed-names") {
    Gc::Heap gc;
    Xml::XmlParser p{gc};

    auto s = Io::SScan(
        "<root xmlns:a=\"http://www.example.org/a\">"
        "<child a:foo=\"bar\"/>"
        "<a:item/>"
        "</root>"
    );
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_XHTML);
    try$(p.parse(s, NONE, *dom));

    auto root = dom->firstChild()->as<Dom::Element>();
    assertNe$(root, nullptr);
    assert$((root->qualifiedName == Dom::QualifiedName{NONE, "root"_sym}));

    auto child = root->firstChild()->as<Dom::Element>();
    assertNe$(child, nullptr);
    assert$((child->qualifiedName == Dom::QualifiedName{NONE, "child"_sym}));
    assert$(child->hasAttribute(Dom::QualifiedName{Some("http://www.example.org/a"_sym), "foo"_sym}));
    assert$(child->getAttribute(Dom::QualifiedName{Some("http://www.example.org/a"_sym), "foo"_sym}) == "bar");

    auto item = child->nextSibling()->as<Dom::Element>();
    assertNe$(item, nullptr);
    assert$((item->qualifiedName == Dom::QualifiedName{Some("http://www.example.org/a"_sym), "item"_sym}));

    return Ok();
}

} // namespace Vaev::Xml::Tests
