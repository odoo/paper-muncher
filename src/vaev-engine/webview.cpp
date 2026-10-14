module;

#include <karm/macros>

export module Vaev.Engine:webview;

import Karm.Gc;
import Karm.Http;
import Karm.Gfx;
import Karm.Math;
import Karm.Logger;

import :style;
import :dom.document;
import :loader.loader;
import :layout;
import :paint;
import :values;
import :driver.print;

using namespace Karm;

namespace Vaev {

static auto dumpFragments = Debug::Flag::debug("web-fragments"s, "Dump the constructed fragments"s);
static auto dumpStacking = Debug::Flag::debug("web-stacking"s, "Dump the stacking context tree"s);

static auto layout2 = Debug::Flag::feature("layout2-render"s, "Enable the work in progress layout engine rewrite in render mode"s);

export struct RenderResult {
    Rc<Layout::Tree> tree;
    Rc<Layout::Fragment> fragments;
    Rc<Paint::StackingContext> stacking;
};

export struct WebView {
    mutable Gc::Heap _heap;
    Rc<Http::Client> _client;
    Style::Media _media = Style::Media::defaultMedia();

    Gc::Ptr<Dom::Document> _document = nullptr;
    Opt<RenderResult> _render = NONE;
    Style::CounterSet _initialCounterSet = {};

    WebView(Rc<Http::Client> client)
        : _client(client) {}

    static Rc<WebView> create(Rc<Http::Client> client = Http::defaultClient()) {
        return makeRc<WebView>(client);
    }

    void changeMedia(Style::Media media) {
        if (_media != media) {
            _media = media;
            invalidateRender();
        }
    }

    void changeInitialCounterSet(Style::CounterSet counterSet) {
        _initialCounterSet = std::move(counterSet);
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

    Gc::Ptr<Dom::Document> document() const {
        return _document;
    }

    RenderResult& ensureRender() {
        if (_render)
            return *_render;

        computeStyle();

        Style::Viewport viewport = {.small = _media.scaledViewport()};
        auto tree = makeRc<Layout::Tree>(
            Layout::buildDocument(_document.upgrade()),
            viewport
        );

        Opt<Rc<Layout::Fragment>> rootFrag = NONE;
        if (layout2) {
            // FIXME: Extract into a layoutRoot, and investigate the right init params.
            rootFrag = Some(Layout2::layout(
                *tree, tree->root,
                Layout2::Constraints{
                    .knownSize = {Some(viewport.small.width), NONE},
                    .containingBlock = {
                        Some(viewport.small.width),
                        Some(viewport.small.height),
                    },
                    .availableSpace = {
                        viewport.small.width,
                        Layout2::MAX_CONTENT,
                    },

                    // https://www.w3.org/TR/CSS22/box.html#collapsing-margins
                    // - Margins of the root element's box do not collapse.
                    .collapseMargins = false,

                    // NoSpec AF >.<
                    .margins = Layout2::LogicalInsets<Au>::all(0_au),
                }
            ).is<Layout2::Placed>()->fragment);

            Layout2::absolutize(*rootFrag);
        } else {
            rootFrag = Layout::layoutRoot(
                *tree,
                {
                    .generateFragment = true,
                    .knownSize = {Some(viewport.small.width), NONE},
                    .availableSpace = {viewport.small.width, 0_au},
                    .containingBlock = {viewport.small.width, viewport.small.height},
                }
            ).fragment;
        }

        auto stacking = Paint::StackingContext::establishStackingContext(*rootFrag);

        if (dumpFragments)
            logDebugIf(dumpFragments, "fragments: {}", *rootFrag);

        if (dumpStacking)
            logDebugIf(dumpStacking, "stacking: {}", stacking);

        _render = Some(RenderResult{
            tree,
            *rootFrag,
            stacking,
        });
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
        computer.styleDocument(*_document, _initialCounterSet);
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

} // namespace Vaev
