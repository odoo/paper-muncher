module;

#include <karm-core/macros.h>

export module Vaev.Engine:dom.window;

import Karm.Gc;
import Karm.Http;
import Karm.Gfx;
import Karm.Math;

import :style.media;
import :dom.document;
import :loader.loader;
import :layout.base;
import :driver.render;
import :driver.print;

using namespace Karm;

namespace Vaev::Dom {

// https://html.spec.whatwg.org/multipage/nav-history-apis.html#the-window-object
export struct Window {
    Gc::Heap _heap;
    Rc<Http::Client> _client;
    Style::Media _media = Style::Media::defaultMedia();

    Gc::Ptr<Document> _document = nullptr;
    Opt<Driver::RenderResult> _render = NONE;

    Window(Rc<Http::Client> client)
        : _client(client) {}

    static Rc<Window> create(Rc<Http::Client> client = Http::defaultClient()) {
        return makeRc<Window>(client);
    }

    void changeMedia(Style::Media media) {
        _media = media;
        invalidateRender();
    }

    void changeViewport(Vec2Au viewport) {
        if (_media.changeViewport(viewport))
            invalidateRender();
    }

    Async::Task<> loadLocationAsync(Ref::Url url, Ref::Uti intent = Ref::Uti::PUBLIC_OPEN) {
        if (intent == Ref::Uti::PUBLIC_OPEN) {
            _document = co_trya$(
                Vaev::Loader::fetchDocumentAsync(
                    _heap, *_client, url
                )
            );
        } else if (intent == Ref::Uti::PUBLIC_MODIFY) {
            _document = co_trya$(Vaev::Loader::viewSourceAsync(_heap, *_client, url));
        } else {
            co_return Error::invalidInput("unsupported intent");
        }
        invalidateRender();
        co_return Ok();
    }

    Ref::Url location() const {
        return _document.upgrade()->url();
    }

    Gc::Ptr<Document> document() {
        return _document;
    }

    Driver::RenderResult& ensureRender() {
        if (_render)
            return *_render;
        Vec2Au viewportSize = {_media.width, _media.height};
        _render = Driver::render(_document.upgrade(), _media, {.small = viewportSize});
        return *_render;
    }

    void computeStyle() {
        Font::Database db;
        if (not db.loadSystemFonts())
            logWarn("not all fonts were properly loaded into database");
        Style::Computer computer{_media, *_document->styleSheets, db};
        computer.build();
        computer.styleDocument(*_document);
    }

    void invalidateRender() {
        _render = NONE;
    }

    Rc<Scene::Node> render() {
        return ensureRender().scenes;
    }

    [[clang::coro_wrapper]]
    Generator<Print::Page> print(Print::Settings settings) const {
        return Driver::print(_document.upgrade(), settings);
    }
};

} // namespace Vaev::Dom
