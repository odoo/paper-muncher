module;

#include <karm/macros>

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
    mutable Gc::Heap _heap;
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

    Async::Task<> loadLocationAsync(Ref::Url url, Ref::Uti intent, Async::CancellationToken ct) {
        if (intent == Ref::Uti::PUBLIC_OPEN) {
            _document = co_trya$(
                Loader::fetchDocumentAsync(
                    _heap, *_client, url, ct
                )
            );
        } else if (intent == Ref::Uti::PUBLIC_MODIFY) {
            _document = co_trya$(Loader::viewSourceAsync(_heap, *_client, url, ct));
        } else {
            co_return Error::invalidInput("unsupported intent");
        }

        invalidateRender();
        co_return Ok();
    }

    [[clang::coro_wrapper]]
    Async::Task<> refreshAsync(Async::CancellationToken ct) {
        return loadLocationAsync(document()->url(), Ref::Uti::PUBLIC_OPEN, ct);
    }

    Ref::Url location() const {
        return _document.upgrade()->url();
    }

    Gc::Ptr<Document> document() const {
        return _document;
    }

    Driver::RenderResult& ensureRender() {
        if (_render)
            return *_render;
        _render = Some(
            Driver::render(
                _heap,
                _document.upgrade(),
                _media,
                {.small = _media.viewportSize()}
            )
        );
        return *_render;
    }

    void computeStyle() {
        Style::Computer computer{
            _heap,
            _media,
            _document->registeredPropertySet,
            *_document->styleSheets,
            _document->fontDatabase,
        };
        computer.build();
        computer.styleDocument(*_document);
    }

    void invalidateRender() {
        _render = NONE;
    }

    RectAu scrollableOverflow() {
        return ensureRender().stacking->scrollableOverflow();
    }

    RectAu borderBox() {
        return ensureRender().fragments->borderBox();
    }

    Gfx::Snapshot snapshot() {
        Gfx::Snapshot::Recorder snapshot{_media.viewportSize().ceili()};
        ensureRender().stacking->paintRoot(snapshot);
        return snapshot.finalize();
    }

    Rc<Gfx::Image> rasterize() {
        auto image = Gfx::Image::alloc(_media.viewportSize().ceili());
        Gfx::CpuCanvas canvas;
        canvas.begin(image->mutPixels());
        ensureRender().stacking->paintRoot(canvas);
        canvas.end();
        return image;
    }

    Rc<Layout::Fragment> hittest(Math::Vec2f at) {
        auto stacking = ensureRender().stacking;
        return stacking->hitest(at, {}).unwrapOr(stacking->fragment);
    }

    void paint(Gfx::Canvas& canvas) {
        ensureRender().stacking->paintRoot(canvas);
    }

    [[clang::coro_wrapper]]
    Yield<Gfx::Snapshot> print(Print::Settings const& settings, Opt<Driver::PageDecorator&> decorator = NONE) const {
        return Driver::print(_heap, _document.upgrade(), settings, decorator);
    }
};

} // namespace Vaev::Dom
