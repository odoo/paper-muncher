module;

#include <karm-core/macros.h>

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

Async::Task<Gc::Ref<Dom::Document>> _loadDocumentAsync(Gc::Heap& heap, Ref::Url url, Rc<Http::Response> resp, Async::CancellationToken ct) {
    auto dom = heap.alloc<Dom::Document>(url);

    auto mime = resp->header.contentType();

    if (not mime.has())
        mime = Ref::sniffSuffix(url.path.suffix());

    if (not resp->body)
        co_return Error::invalidInput("response body is missing");

    auto respBody = resp->body.unwrap();
    auto buf = co_trya$(Aio::readAllUtf8Async(*respBody, ct));

    if (not mime.has() or mime->is("application/octet-stream"_mime)) {
        mime = Ref::sniffBytes(bytes(buf));
        logWarn("{} has unspecified mime type, mime sniffing yielded '{}'", url, mime);
    }

    if (mime->is("text/html"_mime)) {
        Html::HtmlParser parser{heap, dom};
        parser.write(buf);

        co_return Ok(dom);
    } else if (mime->is("application/xhtml+xml"_mime)) {
        Io::SScan scan{buf};
        Xml::XmlParser parser{heap};
        co_try$(parser.parse(scan, Html::NAMESPACE, *dom));

        co_return Ok(dom);
    } else if (mime->is("image/svg+xml"_mime)) {
        Io::SScan scan{buf};
        Xml::XmlParser parser{heap};
        co_try$(parser.parse(scan, Svg::NAMESPACE, *dom));

        co_return Ok(dom);
    } else if (mime->is("text/markdown"_mime)) {
        auto doc = Md::parse(buf);
        logDebug("markdown: {}", doc);
        auto html = Md::renderHtml(doc);

        Html::HtmlParser parser{heap, dom};
        parser.write(html);

        co_return Ok(dom);
    } else if (mime->is("text/plain"_mime)) {
        auto text = heap.alloc<Dom::Text>();
        text->appendData(buf);

        auto body = heap.alloc<Dom::Element>(Html::BODY_TAG);
        body->appendChild(text);

        dom->appendChild(body);
        co_return Ok(dom);
    } else {
        logError("unsupported MIME type: {}", mime);

        co_return Error::invalidInput("unsupported MIME type");
    }
}

export Async::Task<Gc::Ref<Dom::Document>> viewSourceAsync(Gc::Heap& heap, Http::Client& client, Ref::Url const& url, Async::CancellationToken ct) {
    auto resp = co_trya$(client.getAsync(url, ct));
    if (not resp->body)
        co_return Error::invalidInput("response body is missing");
    auto respBody = resp->body.unwrap();
    auto buf = co_trya$(Aio::readAllUtf8Async(*respBody, ct));

    auto dom = heap.alloc<Dom::Document>(url);
    auto body = heap.alloc<Dom::Element>(Html::BODY_TAG);
    dom->appendChild(body);
    auto pre = heap.alloc<Dom::Element>(Html::PRE_TAG);
    body->appendChild(pre);
    auto text = heap.alloc<Dom::Text>(buf);
    pre->appendChild(text);

    co_return Ok(dom);
}

Async::Task<Style::StyleSheet> _fetchStylesheetAsync(Style::PropertyRegistry& registry, Http::Client& client, Ref::Url url, Style::Origin origin, Async::CancellationToken ct) {
    auto resp = co_trya$(client.getAsync(url, ct));
    if (not resp->body)
        co_return Error::notFound("could not load stylesheet");

    auto respBody = resp->body.unwrap();
    auto buf = co_trya$(Aio::readAllUtf8Async(*respBody, ct));

    Io::SScan s{buf};
    co_return Ok(Style::StyleSheet::parse(registry, s, url, origin));
}

Async::Task<Rc<Scene::Node>> _fetchImageContentAsync(Http::Client& client, Ref::Url url, Async::CancellationToken ct);

Rc<Scene::Node> _missingImagePlaceholder() {
    auto placeholder = Karm::Image::loadOrFallback("bundle://vaev-engine/missing.qoi"_url).unwrap();
    return makeRc<Scene::Image>(placeholder->bound().cast<f64>(), placeholder);
}

Async::Task<> _fetchResourcesAsync(Style::PropertyRegistry& registry, Http::Client& client, Gc::Ref<Dom::Node> node, Style::StyleSheetList& sb, Async::CancellationToken ct) {
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
        auto sheet = Style::StyleSheet::parse(registry, textScan, node->baseURI());
        sb.add(std::move(sheet));
    } else if (el and el->qualifiedName == Html::LINK_TAG) {
        auto rel = el->getAttribute(Html::REL_ATTR);
        if (rel == "stylesheet"s) {
            auto href = el->getAttribute(Html::HREF_ATTR);
            if (not href) {
                logWarn("link element missing href attribute");
                co_return Error::invalidInput("link element missing href");
            }

            auto url = Ref::Url::parse(*href, node->baseURI());
            auto sheet = co_await _fetchStylesheetAsync(registry, client, url, Style::Origin::AUTHOR, ct);

            if (not sheet) {
                logWarn("failed to fetch stylesheet from {}: {}", url, sheet);
                co_return Error::invalidInput("failed to fetch stylesheet");
            }

            sb.add(sheet.take());
        }
    } else {
        for (auto child = node->firstChild(); child; child = child->nextSibling())
            (void)co_await _fetchResourcesAsync(registry, client, *child, sb, ct);
    }

    co_return Ok();
}

Async::_Task<Rc<Font::Database>> _loadFontfacesAsync(Http::Client& client, Style::StyleSheetList const& stylesheets, Async::CancellationToken ct);

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

    auto resp = co_trya$(client.getAsync(resolvedUrl, ct));
    auto dom = co_trya$(_loadDocumentAsync(heap, url, resp, ct));
    auto stylesheets = heap.alloc<Style::StyleSheetList>();

    stylesheets->add((co_await _fetchStylesheetAsync(dom->registeredPropertySet, client, "bundle://vaev-engine/html.css"_url, Style::Origin::USER_AGENT, ct))
                         .take("user agent stylesheet not available"));

    stylesheets->add((co_await _fetchStylesheetAsync(dom->registeredPropertySet, client, "bundle://vaev-engine/print.css"_url, Style::Origin::USER_AGENT, ct))
                         .take("user agent stylesheet not available"));

    stylesheets->add((co_await _fetchStylesheetAsync(dom->registeredPropertySet, client, "bundle://vaev-engine/svg.css"_url, Style::Origin::USER_AGENT, ct))
                         .take("user agent stylesheet not available"));

    (void)co_await _fetchResourcesAsync(dom->registeredPropertySet, client, *dom, *stylesheets, ct);
    dom->styleSheets = stylesheets;
    dom->fontDatabase = co_await _loadFontfacesAsync(client, *stylesheets, ct);

    if (dumpDom)
        logDebugIf(dumpDom, "document tree: {}", dom);

    if (dumpStylesheets)
        logDebugIf(dumpStylesheets, "document stylesheets: {}", stylesheets);

    co_return Ok(dom);
}

} // namespace Vaev::Loader
