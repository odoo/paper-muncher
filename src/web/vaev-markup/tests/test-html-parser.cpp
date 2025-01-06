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

} // namespace Vaev::Markup::Tests
