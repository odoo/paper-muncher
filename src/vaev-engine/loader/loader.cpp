module;

#include <karm/macros>

export module Vaev.Engine:loader.loader;

import Karm.Gc;
import Karm.Http;
import Karm.Core;
import Karm.Debug;
import Karm.Md;
import Karm.Ref;
import Karm.Sys;
import Karm.Logger;
import Karm.Image;

import :dom.document;
import :html;
import :xml;
import :style;

namespace Vaev::Loader {

// https://mimesniff.spec.whatwg.org/#computed-mime-type
// https://mimesniff.spec.whatwg.org/#determining-the-computed-mime-type-of-a-resource
static Ref::Uti _determineComputedType(Ref::Url url, Rc<Http::Response> response, Bytes body) {
    auto contentType = response->header.contentType().unwrapOr(Ref::Uti::PUBLIC_DATA);

    if (contentType == Ref::Uti::PUBLIC_DATA)
        contentType = Ref::Uti::fromSuffix(url.path.suffix());

    if (contentType == Ref::Uti::PUBLIC_DATA) {
        contentType = Ref::sniffBytes(bytes(body));
        logWarn("{} has unspecified content-type, sniffing yielded '{}'", url, contentType);
    }

    return contentType;
}

// https://html.spec.whatwg.org/#navigate-html
static Res<Gc::Ref<Dom::Document>> _loadHtmlDocument(Gc::Heap& heap, Ref::Url url, Ref::Uti contentType, Str body) {
    auto dom = Dom::Document::create(heap, url, contentType);
    Html::HtmlParser parser{heap, dom};
    Diag::Collector diags;
    parser.write(body, diags);
    if (diags.any()) {
        Diag::SimpleRenderer render{url};
        render.render(Sys::err(), diags);
    }
    return Ok(dom);
}

// https://html.spec.whatwg.org/#read-xml
static Res<Gc::Ref<Dom::Document>> _loadXmlDocument(Gc::Heap& heap, Ref::Url url, Ref::Uti contentType, Str body) {
    auto dom = Dom::Document::create(heap, url, contentType);
    Io::SScan scan{body};
    Xml::XmlParser parser{heap};
    try$(parser.parse(scan, NONE, *dom));
    return Ok(dom);
}

// https://html.spec.whatwg.org/#read-text
static Res<Gc::Ref<Dom::Document>> _loadTextDocument(Gc::Heap& heap, Ref::Url url, Ref::Uti contentType, Str body) {
    auto dom = Dom::Document::create(heap, url, contentType);
    auto text = heap.alloc<Dom::Text>();
    text->appendData(body);
    auto bodyEl = heap.alloc<Dom::Element>(Html::BODY_TAG);
    bodyEl->appendChild(text);
    dom->appendChild(bodyEl);
    return Ok(dom);
}

// https://html.spec.whatwg.org/#read-media
static Res<Gc::Ref<Dom::Document>> _loadMediaDocument(Gc::Heap& heap, Ref::Url url, Ref::Uti contentType) {
    auto dom = Dom::Document::create(heap, url, contentType);
    auto element = heap.alloc<Dom::Element>(Html::IMG_TAG);
    element->setAttribute(Html::SRC_ATTR, url.str());
    auto bodyEl = heap.alloc<Dom::Element>(Html::BODY_TAG);
    bodyEl->appendChild(element);
    dom->appendChild(bodyEl);
    return Ok(dom);
}

// https://html.spec.whatwg.org/#populating-a-session-history-entry:navigate-html
Async::Task<Gc::Ref<Dom::Document>> _loadDocumentAsync(Gc::Heap& heap, Ref::Url url, Rc<Http::Response> resp, Async::CancellationToken ct) {
    if (not resp->body)
        co_return Error::invalidInput("response body is missing");

    auto body = co_trya$(Aio::readAllTextAsync<Utf8>(**resp->body, ct));

    // 1. Let type be the computed type of navigationParams's response.
    auto contentType = _determineComputedType(url, resp, bytes(body));

    // 2. If the user agent has been configured to process resources of the
    //    given type using some mechanism other than rendering the content
    //    in a navigable, then skip this step. Otherwise, if the type is one
    //    of the following types:

    // an HTML MIME type
    if (contentType.conformsTo(Ref::Uti::PUBLIC_HTML)) {
        // Return the result of loading an HTML document, given navigationParams.
        co_return _loadHtmlDocument(heap, url, contentType, body);
    }
    // an XML MIME type that is not an explicitly supported XML MIME type
    else if (contentType.conformsTo(Ref::Uti::PUBLIC_XML)) {
        // Return the result of loading an XML document given navigationParams and type.
        co_return _loadXmlDocument(heap, url, contentType, body);
    }
    // NOSPEC: Handle markdown as HTML MIME type
    else if (contentType.conformsTo(Ref::Uti::PUBLIC_MARKDOWN)) {
        auto doc = Md::parse(body);
        auto rendered = Md::renderHtml(doc);
        co_return _loadHtmlDocument(heap, url, contentType, rendered);
    }
    // a JavaScript MIME type
    // a JSON MIME type that is not an explicitly supported JSON MIME type
    // "text/css"
    // "text/plain"
    // "text/vtt"
    else if (contentType.conformsTo(Ref::Uti::PUBLIC_TEXT)) {
        // Return the result of loading a text document given navigationParams and type.
        co_return _loadTextDocument(heap, url, contentType, body);
    }
    // a supported image, video, or audio type
    else if (contentType.conformsTo(Ref::Uti::PUBLIC_IMAGE) or
             contentType.conformsTo(Ref::Uti::PUBLIC_AV)) {
        // Return the result of loading a media document given navigationParams and type.
        co_return _loadMediaDocument(heap, url, contentType);
    } else {
        logError("unsupported content type: {}", contentType);
        co_return Error::invalidInput("unsupported content type");
    }
}

export Async::Task<Gc::Ref<Dom::Document>> viewSourceAsync(Gc::Heap& heap, Http::Client& client, Ref::Url const& url, Async::CancellationToken ct) {
    auto resp = co_trya$(client.getAsync(url, ct));
    if (not resp->body)
        co_return Error::invalidInput("response body is missing");
    auto respBody = resp->body.unwrap();
    auto buf = co_trya$(Aio::readAllTextAsync<Utf8>(*respBody, ct));

    auto dom = Dom::Document::create(heap, url, Ref::Uti::PUBLIC_TEXT);
    auto body = heap.alloc<Dom::Element>(Html::BODY_TAG);
    dom->appendChild(body);
    auto pre = heap.alloc<Dom::Element>(Html::PRE_TAG);
    body->appendChild(pre);
    auto text = heap.alloc<Dom::Text>(buf);
    pre->appendChild(text);

    co_return Ok(dom);
}

Async::Task<Style::StyleSheet> _fetchStylesheetAsync(Http::Client& client, Dom::Document& document, Ref::Url url, Style::Origin origin, Async::CancellationToken ct) {
    auto resp = co_trya$(client.getAsync(url, ct));
    if (not resp->body)
        co_return Error::notFound("could not load stylesheet");

    auto respBody = resp->body.unwrap();
    auto buf = co_trya$(Aio::readAllTextAsync<Utf8>(*respBody, ct));

    Io::SScan s{buf};
    Diag::Collector diags;
    auto stylesheet = Style::StyleSheet::parse(document.registeredPropertySet, s, diags, url, origin);

    if (diags.any()) {
        Diag::SimpleRenderer render{url};
        render.render(Sys::err(), diags);
    }

    co_return Ok(stylesheet);
}

Async::Task<Rc<Scene::Node>> _fetchImageContentAsync(Http::Client& client, Ref::Url url, Async::CancellationToken ct);

Rc<Scene::Node> _missingImagePlaceholder() {
    auto placeholder = Karm::Image::loadOrFallback("bundle://vaev-engine/missing.qoi"_url).unwrap();
    return makeRc<Scene::Image>(placeholder->bound().cast<f64>(), placeholder);
}

Async::Task<> _fetchResourcesAsync(Http::Client& client, Dom::Document& document, Gc::Ref<Dom::Node> node, Async::CancellationToken ct) {
    auto el = node->is<Dom::Element>();
    if (el and el->qualifiedName == Html::IMG_TAG) {
        auto src = el->getAttribute(Html::SRC_ATTR);
        if (not src) {
            el->imageContent = _missingImagePlaceholder();
            logWarn("image element missing src attribute");
            co_return Error::invalidInput("link element missing src");
        }

        auto url = Ref::Url::parse(*src, node->baseURI());
        auto image = co_await _fetchImageContentAsync(client, url, ct);
        if (not image) {
            el->imageContent = _missingImagePlaceholder();
            logWarn("failed to fetch image from {}: {}", url, image);
            co_return Error::invalidInput("failed to fetch image");
        }

        el->imageContent = image.take();
    } else if (el and el->qualifiedName == Html::STYLE_TAG) {
        auto text = el->textContent();
        Io::SScan textScan{text};
        Diag::Collector diags;
        auto sheet = Style::StyleSheet::parse(document.registeredPropertySet, textScan, diags, node->baseURI());
        if (diags.any()) {
            Diag::SimpleRenderer render{Io::format("{}:<style>", node->baseURI())};
            render.render(Sys::err(), diags);
        }
        document.styleSheets->add(std::move(sheet));
    } else if (el and el->qualifiedName == Html::LINK_TAG) {
        auto rel = el->getAttribute(Html::REL_ATTR);
        if (rel == "stylesheet"s) {
            auto href = el->getAttribute(Html::HREF_ATTR);
            if (not href) {
                logWarn("link element missing href attribute");
                co_return Error::invalidInput("link element missing href");
            }

            auto url = Ref::Url::parse(*href, node->baseURI());
            auto sheet = co_await _fetchStylesheetAsync(client, document, url, Style::Origin::AUTHOR, ct);

            if (not sheet) {
                logWarn("failed to fetch stylesheet from {}: {}", url, sheet);
                co_return Error::invalidInput("failed to fetch stylesheet");
            }

            document.styleSheets->add(sheet.take());
        }
    } else {
        for (auto child = node->firstChild(); child; child = child->nextSibling())
            (void)co_await _fetchResourcesAsync(client, document, *child, ct);
    }

    co_return Ok();
}

Async::_Task<Rc<Font::Database>> _loadFontfacesAsync(Http::Client& client, Dom::Document const& document, Async::CancellationToken ct);

static auto dumpDom = Debug::Flag::debug("web-dom", "Dump the loaded DOM tree");
static auto dumpStylesheets = Debug::Flag::debug("web-stylesheets", "Dump the loaded stylesheets");

// https://fetch.spec.whatwg.org/#scheme-fetch
export Async::Task<Gc::Ref<Dom::Document>> fetchDocumentAsync(Gc::Heap& heap, Http::Client& client, Ref::Url const& url, Async::CancellationToken ct) {
    Ref::Url resolvedUrl = url;

    // If request’s current URL’s path is the string "blank",
    if (url.scheme == "about" and url.path.str() == "blank") {
        // then return a new response whose status message is `OK`,
        // header list is « (`Content-Type`, `text/html;charset=utf-8`) »
        // and body is the empty byte sequence as a body.
        resolvedUrl = Ref::Url::data("text/html"_mime, {});
    }

    auto response = co_trya$(client.getAsync(resolvedUrl, ct));
    auto document = co_trya$(_loadDocumentAsync(heap, url, response, ct));

    document->styleSheets->add((co_await _fetchStylesheetAsync(client, *document, "bundle://vaev-engine/html.css"_url, Style::Origin::USER_AGENT, ct))
                                   .take("user agent stylesheet not available"));

    if (document->quirkMode == Dom::QuirkMode::YES) {
        logWarn("quirky document, using quirky stylesheet");
        document->styleSheets->add((co_await _fetchStylesheetAsync(client, *document, "bundle://vaev-engine/html-quirk.css"_url, Style::Origin::USER_AGENT, ct))
                                       .take("user agent stylesheet not available"));
    }

    if (document->contentType() == Ref::Uti::PUBLIC_MARKDOWN) {
        document->styleSheets->add((co_await _fetchStylesheetAsync(client, *document, "bundle://vaev-engine/markdown.css"_url, Style::Origin::USER_AGENT, ct))
                                       .take("user agent stylesheet not available"));
    }

    document->styleSheets->add((co_await _fetchStylesheetAsync(client, *document, "bundle://vaev-engine/print.css"_url, Style::Origin::USER_AGENT, ct))
                                   .take("user agent stylesheet not available"));

    document->styleSheets->add((co_await _fetchStylesheetAsync(client, *document, "bundle://vaev-engine/svg.css"_url, Style::Origin::USER_AGENT, ct))
                                   .take("user agent stylesheet not available"));

    document->styleSheets->add((co_await _fetchStylesheetAsync(client, *document, "bundle://vaev-engine/math.css"_url, Style::Origin::USER_AGENT, ct))
                                   .take("user agent stylesheet not available"));

    (void)co_await _fetchResourcesAsync(client, *document, document, ct);
    (void)co_await _loadFontfacesAsync(client, *document, ct);

    if (dumpDom)
        logDebugIf(dumpDom, "document tree: {}", document);

    if (dumpStylesheets)
        logDebugIf(dumpStylesheets, "document stylesheets: {}", document->styleSheets);

    co_return Ok(document);
}

} // namespace Vaev::Loader
