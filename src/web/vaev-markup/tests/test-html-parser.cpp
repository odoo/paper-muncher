#include <karm-test/macros.h>
#include <vaev-markup/html.h>

namespace Vaev::Markup::Tests {

test$("parse-empty-document") {
    auto dom = makeStrong<Markup::Document>(Mime::Url());
    Markup::HtmlParser parser{dom};

    parser.write(""s);
    return Ok();
}

test$("parse-open-close-tag-with-structure") {
    auto dom = makeStrong<Markup::Document>(Mime::Url());
    Markup::HtmlParser parser{dom};

    parser.write("<html></html>"s);

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = try$(dom->firstChild().cast<Element>());
    expect$(html->tagName == Html::HTML);
    expect$(html->hasChildren());

    auto head = try$(html->firstChild().cast<Element>());
    expect$(head->tagName == Html::HEAD);

    auto body = try$(head->nextSibling().cast<Element>());
    expect$(body->tagName == Html::BODY);

    return Ok();
}

test$("parse-empty-tag") {
    auto dom = makeStrong<Markup::Document>(Mime::Url());
    Markup::HtmlParser parser{dom};

    parser.write("<html/>"s);

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    expect$(try$(dom->firstChild().cast<Element>())->tagName == Html::HTML);
    return Ok();
}

test$("parse-attr") {
    auto dom = makeStrong<Markup::Document>(Mime::Url());
    Markup::HtmlParser parser{dom};

    parser.write("<html lang=\"en\"/>"s);

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = try$(dom->firstChild().cast<Element>());
    expect$(html->hasAttribute(Html::LANG_ATTR));
    expect$(html->getAttribute(Html::LANG_ATTR) == "en");

    return Ok();
}

test$("parse-text") {
    auto dom = makeStrong<Markup::Document>(Mime::Url());
    Markup::HtmlParser parser{dom};

    parser.write("<html>text</html>"s);

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = try$(dom->firstChild().cast<Element>());
    expect$(html->tagName == Html::HTML);
    expect$(html->children().len() == 2);

    auto body = try$(html->firstChild()->nextSibling().cast<Element>());
    expect$(body->tagName == Html::BODY);
    expect$(body->hasChildren());

    auto text = body->firstChild();
    expect$(text->nodeType() == NodeType::TEXT);
    expect$(try$(text.cast<Text>())->data == "text");

    return Ok();
}

test$("parse-title") {
    auto dom = makeStrong<Markup::Document>(Mime::Url());
    Markup::HtmlParser parser{dom};

    parser.write("<title>the title</title>");

    expect$(dom->title() == "the title");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = try$(dom->firstChild().cast<Element>());
    expect$(html->children().len() == 2);

    auto head = try$(html->firstChild().cast<Element>());
    expect$(head->tagName == Html::HEAD);
    expect$(head->hasChildren());

    auto title = try$(head->firstChild().cast<Element>());
    expect$(title->tagName == Html::TITLE);
    expect$(title->hasChildren());

    auto text = try$(title->firstChild().cast<Text>());
    expect$(text->data == "the title");

    return Ok();
}

test$("parse-comment-with-gt-symb") {
    auto dom = makeStrong<Markup::Document>(Mime::Url());
    Markup::HtmlParser parser{dom};

    parser.write(
        "<title>im a title!</title>"
        "<!-- a b <meta> c d -->"
    );

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = try$(dom->firstChild().cast<Element>());
    auto head = try$(html->firstChild().cast<Element>());
    expect$(head->tagName == Html::HEAD);
    expect$(head->hasChildren());

    auto title = try$(head->firstChild().cast<Element>());
    expect$(title->tagName == Html::TITLE);

    auto comment = title->nextSibling();
    expect$(comment->nodeType() == NodeType::COMMENT);
    expect$(try$(comment.cast<Comment>())->data == " a b <meta> c d ");

    return Ok();
}

test$("parse-p-after-comment") {
    auto dom = makeStrong<Markup::Document>(Mime::Url());
    Markup::HtmlParser parser{dom};

    parser.write(
        "<!-- im a comment -->"
        "<p>im a p</p>"
    );

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto comment = try$(dom->firstChild().cast<Comment>());

    auto html = try$(comment->nextSibling().cast<Element>());
    expect$(html->children().len() == 2);

    auto body = try$(html->firstChild()->nextSibling().cast<Element>());
    expect$(body->tagName == Html::BODY);
    expect$(body->hasChildren());

    auto p = try$(body->firstChild().cast<Element>());
    expect$(p->tagName == Html::P);

    auto text = try$(p->firstChild().cast<Text>());
    expect$(text->data == "im a p");

    return Ok();
}

test$("parse-not-nested-p-and-els-inbody") {
    auto dom = makeStrong<Markup::Document>(Mime::Url());
    Markup::HtmlParser parser{dom};

    parser.write("<div>b</div><p>a<div>b</div><p>a<p>a");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = try$(dom->firstChild().cast<Element>());
    expect$(html->tagName == Html::HTML);
    expect$(html->children().len() == 2);

    auto body = try$(html->firstChild()->nextSibling().cast<Element>());
    expect$(body->tagName == Html::BODY);
    expect$(body->children().len() == 5);

    return Ok();
}

test$("parse-char-referece-as-text") {
    auto dom = makeStrong<Markup::Document>(Mime::Url());
    Markup::HtmlParser parser{dom};

    parser.write("<html><body>im there&sect;&Aacute;&sect;&seca;&seca&Aacute;im also there</body></html>");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = try$(dom->firstChild().cast<Element>());
    expect$(html->tagName == Html::HTML);
    expect$(html->children().len() > 1);

    auto body = try$(html->firstChild()->nextSibling().cast<Element>());
    expect$(body->tagName == Html::BODY);
    expect$(body->hasChildren());

    auto text = body->firstChild();
    expect$(text->nodeType() == NodeType::TEXT);

    expect$(try$(text.cast<Text>())->data == "im there§Á§&seca;&secaÁim also there");

    return Ok();
}

test$("parse-char-referece-as-attribute-value") {
    auto dom = makeStrong<Markup::Document>(Mime::Url());
    Markup::HtmlParser parser{dom};

    parser.write("<meta value=\"im there&sect;&Aacute;&sect;&seca;&seca&Aacute;im also there\">");

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = try$(dom->firstChild().cast<Element>());
    expect$(html->tagName == Html::HTML);
    expect$(html->children().len() == 2);

    auto head = try$(html->firstChild().cast<Element>());
    expect$(head->tagName == Html::HEAD);
    expect$(head->hasChildren());

    auto meta = try$(head->firstChild().cast<Element>());
    expect$(meta->tagName == Html::META);

    expect$(meta->hasAttribute(Html::VALUE_ATTR));
    expect$(meta->getAttribute(Html::VALUE_ATTR) == "im there§Á§&seca;&secaÁim also there");

    return Ok();
}

test$("parse-char-referece-spec-example") {
    // https://html.spec.whatwg.org/#named-character-reference-state : EXAMPLE
    auto dom = makeStrong<Markup::Document>(Mime::Url());
    Markup::HtmlParser parser{dom};

    parser.write(
        "<html><meta value=\"I'm &notit; I tell you\">"
        "<body><div>I'm &notit; I tell you</div><div>I'm &notin; I tell you</div></body></html>"
    );

    expect$(dom->nodeType() == NodeType::DOCUMENT);
    expect$(dom->hasChildren());

    auto html = try$(dom->firstChild().cast<Element>());
    expect$(html->tagName == Html::HTML);
    expect$(html->children().len() > 1);

    auto body = try$(html->firstChild()->nextSibling().cast<Element>());
    expect$(body->tagName == Html::BODY);
    expect$(body->hasChildren());

    {
        auto div1 = try$(body->firstChild().cast<Element>());
        expect$(div1->tagName == Html::DIV);
        expect$(div1->hasChildren());

        auto text1 = div1->firstChild();
        expect$(text1->nodeType() == NodeType::TEXT);
        expect$(try$(text1.cast<Text>())->data == "I'm ¬it; I tell you");
    }
    {
        auto div2 = try$(body->firstChild()->nextSibling().cast<Element>());
        expect$(div2->tagName == Html::DIV);
        expect$(div2->hasChildren());

        auto text2 = div2->firstChild();
        expect$(text2->nodeType() == NodeType::TEXT);

        expect$(try$(text2.cast<Text>())->data == "I'm ∉ I tell you");
    }
    {
        auto head = try$(html->firstChild().cast<Element>());
        expect$(head->tagName == Html::HEAD);
        expect$(head->hasChildren());

        auto meta = try$(head->firstChild().cast<Element>());
        expect$(meta->tagName == Html::META);

        expect$(meta->hasAttribute(Html::VALUE_ATTR));
        expect$(meta->getAttribute(Html::VALUE_ATTR) == "I'm &notit; I tell you");
    }

    return Ok();
}

} // namespace Vaev::Markup::Tests
