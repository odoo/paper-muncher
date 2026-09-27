#include <karm/test>

import Vaev.Engine;
import Karm.Gc;
import Karm.Ref;
import Karm.Diag;

using namespace Karm;
using namespace Karm::Literals;

namespace Vaev::Dom::Tests {

test$("parse-empty-document") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write(""s, diags);
    return Ok();
}

test$("parse-open-close-tag-with-structure") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write("<html></html>"s, diags);

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->qualifiedName == Html::HTML_TAG);
    assert$(html->hasChildren());

    auto head = html->firstChild()->as<Element>();
    assertNe$(head, nullptr);
    assert$(head->qualifiedName == Html::HEAD_TAG);

    auto body = head->nextSibling()->as<Element>();
    assertNe$(body, nullptr);
    assert$(body->qualifiedName == Html::BODY_TAG);

    return Ok();
}

test$("parse-empty-tag") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write("<html/>"s, diags);

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->qualifiedName == Html::HTML_TAG);
    return Ok();
}

test$("parse-attr") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write("<html lang=\"en\"/>"s, diags);

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->hasAttribute(Html::LANG_ATTR));
    assert$(html->getAttribute(Html::LANG_ATTR) == "en");

    return Ok();
}

test$("parse-text") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write("<html>text</html>"s, diags);

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->qualifiedName == Html::HTML_TAG);
    assert$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->as<Element>();
    assertNe$(body, nullptr);
    assert$(body->qualifiedName == Html::BODY_TAG);
    assert$(body->hasChildren());

    auto text = body->firstChild()->as<Text>();
    assertNe$(text, nullptr);
    assert$(text->data() == "text");

    return Ok();
}

test$("parse-title") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write("<title>the title</title>", diags);

    assert$(dom->title() == "the title");
    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->countChildren() == 2);

    auto head = html->firstChild()->as<Element>();
    assertNe$(head, nullptr);
    assert$(head->qualifiedName == Html::HEAD_TAG);
    assert$(head->hasChildren());

    auto title = head->firstChild()->as<Element>();
    assertNe$(title, nullptr);
    assert$(title->qualifiedName == Html::TITLE_TAG);
    assert$(title->hasChildren());

    auto text = title->firstChild()->as<Text>();
    assertNe$(text, nullptr);
    assert$(text->data() == "the title");

    return Ok();
}

test$("parse-comment-with-gt-symb") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write(
        "<title>im a title!</title>"
        "<!-- a b <meta> c d -->",
        diags
    );

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);

    auto head = html->firstChild()->as<Element>();
    assertNe$(head, nullptr);
    assert$(head->qualifiedName == Html::HEAD_TAG);
    assert$(head->hasChildren());

    auto title = head->firstChild()->as<Element>();
    assertNe$(title, nullptr);
    assert$(title->qualifiedName == Html::TITLE_TAG);

    auto comment = title->nextSibling()->as<Comment>();
    assertNe$(comment, nullptr);
    assert$(comment->data() == " a b <meta> c d ");

    return Ok();
}

test$("parse-p-after-comment") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write(
        "<!-- im a comment -->"
        "<p>im a p</p>",
        diags
    );

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto comment = dom->firstChild()->as<Comment>();
    assertNe$(comment, nullptr);

    auto html = comment->nextSibling()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->as<Element>();
    assertNe$(body, nullptr);
    assert$(body->qualifiedName == Html::BODY_TAG);
    assert$(body->hasChildren());

    auto p = body->firstChild()->as<Element>();
    assertNe$(p, nullptr);
    assert$(p->qualifiedName == Html::P_TAG);

    auto text = p->firstChild()->as<Text>();
    assertNe$(text, nullptr);
    assert$(text->data() == "im a p");

    return Ok();
}

test$("parse-not-nested-p-and-els-inbody") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write("<div>b</div><p>a<div>b</div><p>a<p>a", diags);

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->qualifiedName == Html::HTML_TAG);
    assert$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->as<Element>();
    assertNe$(body, nullptr);
    assert$(body->qualifiedName == Html::BODY_TAG);
    assert$(body->countChildren() == 5);

    return Ok();
}

test$("parse-char-referece-as-text") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write("<html><body>im there&sect;&Aacute;&sect;&seca;&seca&Aacute;im also there</body></html>", diags);

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->qualifiedName == Html::HTML_TAG);

    auto body = html->firstChild()->nextSibling()->as<Element>();
    assertNe$(body, nullptr);
    assert$(body->qualifiedName == Html::BODY_TAG);
    assert$(body->hasChildren());

    auto text = body->firstChild()->as<Text>();
    assertNe$(text, nullptr);
    assert$(text->data() == "im there§Á§&seca;&secaÁim also there");

    return Ok();
}

test$("parse-char-referece-as-attribute-value") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write("<meta value=\"im there&sect;&Aacute;&sect;&seca;&seca&Aacute;im also there\">", diags);

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->qualifiedName == Html::HTML_TAG);
    assert$(html->countChildren() == 2);

    auto head = html->firstChild()->as<Element>();
    assertNe$(head, nullptr);
    assert$(head->qualifiedName == Html::HEAD_TAG);
    assert$(head->hasChildren());

    auto meta = head->firstChild()->as<Element>();
    assertNe$(meta, nullptr);
    assert$(meta->qualifiedName == Html::META_TAG);
    assert$(meta->getAttribute(Html::VALUE_ATTR) == "im there§Á§&seca;&secaÁim also there");

    return Ok();
}

test$("parse-char-referece-spec-example") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write(
        "<html><meta value=\"I'm &notit; I tell you\">"
        "<body><div>I'm &notit; I tell you</div><div>I'm &notin; I tell you</div></body></html>",
        diags
    );

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->qualifiedName == Html::HTML_TAG);

    auto body = html->firstChild()->nextSibling()->as<Element>();
    assertNe$(body, nullptr);
    assert$(body->qualifiedName == Html::BODY_TAG);
    assert$(body->hasChildren());

    {
        auto div1 = body->firstChild()->as<Element>();
        assertNe$(div1, nullptr);
        assert$(div1->qualifiedName == Html::DIV_TAG);

        auto text1 = div1->firstChild()->as<Text>();
        assertNe$(text1, nullptr);
        assert$(text1->data() == "I'm ¬it; I tell you");
    }
    {
        auto div2 = body->firstChild()->nextSibling()->as<Element>();
        assertNe$(div2, nullptr);
        assert$(div2->qualifiedName == Html::DIV_TAG);

        auto text2 = div2->firstChild()->as<Text>();
        assertNe$(text2, nullptr);
        assert$(text2->data() == "I'm ∉ I tell you");
    }
    {
        auto head = html->firstChild()->as<Element>();
        assertNe$(head, nullptr);
        assert$(head->qualifiedName == Html::HEAD_TAG);

        auto meta = head->firstChild()->as<Element>();
        assertNe$(meta, nullptr);
        assert$(meta->getAttribute(Html::VALUE_ATTR) == "I'm &notit; I tell you");
    }

    return Ok();
}

test$("parse-input-element") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write("<div><input></div>", diags);

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->qualifiedName == Html::HTML_TAG);
    assert$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->as<Element>();
    assertNe$(body, nullptr);
    assert$(body->qualifiedName == Html::BODY_TAG);

    auto div = body->firstChild()->as<Element>();
    assertNe$(div, nullptr);
    assert$(div->qualifiedName == Html::DIV_TAG);

    auto input = div->firstChild()->as<Element>();
    assertNe$(input, nullptr);
    assert$(input->qualifiedName == Html::INPUT_TAG);

    return Ok();
}

test$("parse-empty-table-element") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write("<table></table>", diags);

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->qualifiedName == Html::HTML_TAG);
    assert$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->as<Element>();
    assertNe$(body, nullptr);
    assert$(body->qualifiedName == Html::BODY_TAG);

    auto table = body->firstChild()->as<Element>();
    assertNe$(table, nullptr);
    assert$(table->qualifiedName == Html::TABLE_TAG);
    assert$(not table->hasChildren());

    return Ok();
}

test$("parse-table-element") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write("<table><thead><tr><th>hi</th></tr></thead></table>", diags);

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->qualifiedName == Html::HTML_TAG);
    assert$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->as<Element>();
    assertNe$(body, nullptr);
    assert$(body->qualifiedName == Html::BODY_TAG);

    auto table = body->firstChild()->as<Element>();
    assertNe$(table, nullptr);
    assert$(table->qualifiedName == Html::TABLE_TAG);
    assert$(table->countChildren() == 1);

    auto thead = table->firstChild()->as<Element>();
    assertNe$(thead, nullptr);
    assert$(thead->qualifiedName == Html::THEAD_TAG);

    auto headerRow = thead->firstChild()->as<Element>();
    assertNe$(headerRow, nullptr);
    assert$(headerRow->qualifiedName == Html::TR_TAG);

    auto headerCell = headerRow->firstChild()->as<Element>();
    assertNe$(headerCell, nullptr);
    assert$(headerCell->qualifiedName == Html::TH_TAG);

    auto text = headerCell->firstChild()->as<Text>();
    assertNe$(text, nullptr);
    assert$(text->data() == "hi");

    return Ok();
}

test$("parse-table-element-create-body-tr-scope") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write("<table><th>hi</th></table>", diags);

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->qualifiedName == Html::HTML_TAG);
    assert$(html->countChildren() == 2);

    auto body = html->firstChild()->nextSibling()->as<Element>();
    assertNe$(body, nullptr);
    assert$(body->qualifiedName == Html::BODY_TAG);

    auto table = body->firstChild()->as<Element>();
    assertNe$(table, nullptr);
    assert$(table->qualifiedName == Html::TABLE_TAG);
    assert$(table->countChildren() == 1);

    auto tbody = table->firstChild()->as<Element>();
    assertNe$(tbody, nullptr);
    assert$(tbody->qualifiedName == Html::TBODY_TAG);

    auto row = tbody->firstChild()->as<Element>();
    assertNe$(row, nullptr);
    assert$(row->qualifiedName == Html::TR_TAG);

    auto header = row->firstChild()->as<Element>();
    assertNe$(header, nullptr);
    assert$(header->qualifiedName == Html::TH_TAG);

    auto text = header->firstChild()->as<Text>();
    assertNe$(text, nullptr);
    assert$(text->data() == "hi");

    return Ok();
}

test$("parse-svg-case-fix") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write(
        "<html><svg viewbox=\"0 0 0 0\"><foreignobject></foreignobject></svg></html>"s,
        diags
    );

    assert$(dom->nodeType() == NodeType::DOCUMENT);
    assert$(dom->hasChildren());

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    assert$(html->qualifiedName == Html::HTML_TAG);
    assert$(html->hasChildren());

    auto head = html->firstChild()->as<Element>();
    assertNe$(head, nullptr);
    assert$(head->qualifiedName == Html::HEAD_TAG);

    auto body = head->nextSibling()->as<Element>();
    assertNe$(body, nullptr);
    assert$(body->qualifiedName == Html::BODY_TAG);

    auto svg = body->firstChild()->as<Element>();
    assertNe$(svg, nullptr);
    assert$(svg->qualifiedName == Svg::SVG_TAG);
    assert$(svg->countChildren() == 1);
    assert$(svg->hasAttribute(Svg::VIEW_BOX_ATTR));

    auto foreignObject = svg->firstChild()->as<Element>();
    assertNe$(foreignObject, nullptr);
    assert$(foreignObject->qualifiedName == Svg::FOREIGN_OBJECT_TAG);

    return Ok();
}

test$("parse-misnested-content-in-table") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write("<table><div>fostered</div><tr><td>cell</td></tr></table>", diags);

    auto html = dom->firstChild()->as<Element>();
    auto body = html->lastChild()->as<Element>();

    auto fostered = body->firstChild()->as<Element>();
    assertNe$(fostered, nullptr);
    assert$(fostered->qualifiedName == Html::DIV_TAG);

    auto table = fostered->nextSibling()->as<Element>();
    assertNe$(table, nullptr);
    assert$(table->qualifiedName == Html::TABLE_TAG);

    return Ok();
}

test$("parse-duplicate-body-merges-attributes") {
    Gc::Heap gc;
    auto dom = Dom::Document::create(gc, Ref::Url(), Ref::Uti::PUBLIC_HTML);
    Html::HtmlParser parser{gc, dom};

    auto diags = Diag::Collector::ignore();
    parser.write("<html><body class=\"first\"><body class=\"second\" dir=\"rtl\"></body></html>"s, diags);

    auto html = dom->firstChild()->as<Element>();
    assertNe$(html, nullptr);
    auto body = html->firstChild()->nextSibling()->as<Element>();
    assertNe$(body, nullptr);

    assertEq$(body->attributes.len(), 2uz);
    assertEq$(body->getAttribute(Dom::QualifiedName{NONE, "class"_sym}), "first"s);
    assert$(body->classList.contains("first"s));
    assert$(not body->classList.contains("second"s));

    assertEq$(body->getAttribute(Dom::QualifiedName{NONE, "dir"_sym}), "rtl"s);

    return Ok();
}

} // namespace Vaev::Dom::Tests
