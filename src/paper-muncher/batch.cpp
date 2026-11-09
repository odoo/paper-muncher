module;

#include <karm/macros>

export module PaperMuncher:batch;

import Karm.Gc;
import Karm.Http;
import Karm.Image;
import Karm.Print;
import Karm.Debug;
import Karm.Sys;
import Karm.Gfx;
import Karm.Math;
import Karm.Logger;
import Karm.Core;
import Karm.Ref;

import Vaev.Engine;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Math::Literals;
using namespace Karm::Fmt::Literals;
using namespace Karm::Ref::Literals;
using namespace Vaev::Literals;

namespace PaperMuncher {

//< What to do when multiple document are passed as input.
export enum struct Batch {
    CONCAT,   //< Concat them as a single document.
    SEPARATE, //< Save them as separate document.

    _LEN,
};

export enum struct Flow {
    AUTO,       //< Paginate for PDF, otherwise continuous
    PAGINATE,   //< If the content exceeds the viewport, create new pages
    CONTINUOUS, //< If the content exceeds the viewport, extend the viewport

    _LEN,
};

export enum struct Extend {
    CROP, //< The document is cropped to the container
    FIT,  //< Container is resized to fit the document

    _LEN,
};

export struct BatchItem {
    Ref::Url input;
    Opt<Ref::Url> header = NONE;
    Opt<Ref::Url> footer = NONE;
};

Rc<Http::Transport> _createHttpTransport(bool sandboxed) {
    if (sandboxed) {
        return Http::multiplexTransport({
            Http::cacheTransport(Http::pipeTransport()),
            Http::localTransport({"bundle"s, "fd"s, "data"s}),
        });
    }

    return Http::multiplexTransport({
        Http::cacheTransport({
            Http::pipeTransport(),
            Http::httpTransport(),
        }),
        Http::localTransport(Http::LocalTransportPolicy::ALLOW_ALL),
    });
}

export Rc<Http::Client> defaultHttpClient(bool sandboxed) {
    auto transport = _createHttpTransport(sandboxed);
    auto client = makeRc<Http::Client>(transport);
    client->userAgent = "Mozilla/5.0 Paper-Muncher/" stringify$(__ck_version_value) ""s;
    return client;
}

export struct Options {
    Vaev::Style::Dppx scale = 1_dppx;
    Vaev::Style::Dppx density = 1_dppx;
    Opt<Vaev::Style::Px> width = NONE;
    Opt<Vaev::Style::Px> height = NONE;
    Opt<Vaev::Style::Color> background = NONE;
    Print::PaperStock stock = Print::A4;
    Print::Orientation orientation = Print::Orientation::PORTRAIT;
    Union<Print::MarginOption, Math::Insets<Vaev::Style::Px>> margins = Print::MarginOption::DEFAULT;
    Ref::Uti outputFormat = Ref::Uti::PUBLIC_DATA;
    Batch batch = Batch::CONCAT;
    Flow flow = Flow::AUTO;
    Extend extend = Extend::CROP;
    Union<Vaev::Style::Keywords::Auto, Vaev::Style::Px> headerSize = Vaev::Style::Keywords::AUTO;
    Union<Vaev::Style::Keywords::Auto, Vaev::Style::Px> footerSize = Vaev::Style::Keywords::AUTO;

    auto derivePrintSettings() const -> Print::Settings {
        auto stock = this->stock;
        if (this->width or this->height)
            stock = Print::PaperStock::custom(
                this->width ? Vaev::Layout::resolve(*this->width) : stock.minorAxis,
                this->height ? Vaev::Layout::resolve(*this->height) : stock.majorAxis
            );

        Print::Margins margins = Print::MarginOption::DEFAULT;
        this->margins.visit(
            [&](Print::MarginOption n) {
                margins = n;
            },
            [&](Math::Insets<Vaev::Style::Px> const& insets) {
                margins = insets.map([&](Vaev::Style::Px l) {
                    return Vaev::Layout::resolve(l);
                });
            }
        );

        return {
            .stock = stock,
            .orientation = this->orientation,
            .margins = margins,
            .scale = this->scale.value(),
        };
    }

    Vaev::Style::DisplayArea deriveArea() const {
        auto width = this->width ? this->width->value() : 800.0;
        auto height = this->height ? this->height->value() : 600.0;
        return Vaev::Style::DisplayArea::forRender(
            {width, height},
            this->scale
        );
    }
};

struct HeaderFooterDecorator : Vaev::Driver::PageDecorator {
    Opt<Rc<Vaev::WebView>> headerWebView;
    Opt<Rc<Vaev::WebView>> footerWebView;
    Union<Vaev::Style::Keywords::Auto, Vaev::Style::Px> headerSize = Vaev::Style::Keywords::AUTO;
    Union<Vaev::Style::Keywords::Auto, Vaev::Style::Px> footerSize = Vaev::Style::Keywords::AUTO;
    Map<Math::Vec2Au, Pair<Vaev::Layout::Au>> _memo;

    Vaev::Layout::RectAu layout(Vaev::Style::DisplayArea const& area, Vaev::Style::ViewerPrefs const& prefs, Vaev::Driver::PageLayoutInfos const& infos) override {
        auto [headerHeight, footerHeight] = _memo.lookupOrPut(infos.pageDecoration.size(), [&] {
            auto headerHeightMemo = headerSize.visit(
                [&](Vaev::Style::Keywords::Auto){
                    if (auto& [w] = headerWebView) {
                        w->changeArea(area.withViewport(Vaev::Layout::toPixel(infos.pageDecoration.size())));
                        w->changePrefs(prefs);
                        return w->borderBox().height;
                    } else {
                        return 0_au;
                    }
                },
                [&](Vaev::Style::Px const& px) {
                    return Vaev::Layout::resolve(px);
                }
            );

            auto footerHeightMemo = footerSize.visit(
                [&](Vaev::Style::Keywords::Auto){
                    if (auto& [w] = footerWebView) {
                        w->changeArea(area.withViewport(Vaev::Layout::toPixel(infos.pageDecoration.size())));
                        w->changePrefs(prefs);
                        return w->borderBox().height;
                    } else {
                        return 0_au;
                    }
                },
                [&](Vaev::Style::Px const& px) {
                    return Vaev::Layout::resolve(px);
                }
            );

            return Pair{headerHeightMemo, footerHeightMemo};
        });
        return infos.pageDecoration.shrink({headerHeight, 0_au, footerHeight});
    }

    void decorate(Vaev::Style::DisplayArea const& area, Vaev::Style::ViewerPrefs const& prefs, Vaev::Driver::PageLayoutInfos const& infos, usize pageCount, Gfx::Canvas& g) override {
        auto decorationWidth = infos.pageDecoration.width;
        auto [headerHeight, footerHeight] = _memo.lookup(infos.pageDecoration.size()).expect();

        Vaev::Style::CounterSet pageCounters;
        pageCounters.instantiateCounter(nullptr, {Vaev::Style::CustomIdent{"page"_sym}, false}, static_cast<i64>(infos.pageNumber));
        pageCounters.instantiateCounter(nullptr, {Vaev::Style::CustomIdent{"pages"_sym}, false}, static_cast<i64>(pageCount));

        if (auto& [w] = headerWebView) {
            w->changeArea(area.withViewport(Vaev::Layout::toPixel(Vaev::Layout::Vec2Au{decorationWidth, headerHeight})));
            w->changePrefs(prefs);
            w->changeInitialCounterSet(pageCounters);
            auto origin = infos.pageDecoration.topStart();
            g.push();
            // Painted over the page content: keep the decoration's background out of it.
            g.clip(Vaev::Layout::RectAu{origin, {decorationWidth, headerHeight}}.cast<f64>());
            g.transform(Math::Trans2f::translate(origin.cast<f64>()));
            w->paint(g);
            g.pop();
        }

        if (auto& [w] = footerWebView) {
            w->changeArea(area.withViewport(Vaev::Layout::toPixel(Vaev::Layout::Vec2Au{decorationWidth, footerHeight})));
            w->changePrefs(prefs);
            w->changeInitialCounterSet(pageCounters);
            auto origin = infos.pageDecoration.bottomStart() - Math::Vec2Au{0_au, footerHeight};
            g.push();
            g.clip(Vaev::Layout::RectAu{origin, {decorationWidth, footerHeight}}.cast<f64>());
            g.transform(Math::Trans2f::translate(origin.cast<f64>()));
            w->paint(g);
            g.pop();
        }
    }
};

Async::Task<Rc<Vaev::WebView>> _loadHeaderFooterWebViewAsync(Rc<Vaev::Loader::ResourceLoader> loader, Ref::Url const& url, Async::CancellationToken ct) {
    auto webview = Vaev::WebView::create(loader);
    co_trya$(webview->loadLocationAsync(url, Ref::Uti::PUBLIC_OPEN, ct));

    auto sheet = co_trya$(loader->fetchStylesheetAsync(*webview->document(), "bundle://vaev-engine/wkhtmltopdf-polyfill.css"_url, Vaev::Style::Origin::USER_AGENT, ct));
    webview->document()->styleSheets->add(std::move(sheet));

    co_return Ok(webview);
}

Async::Task<> runSingleAsync(
    Rc<Vaev::Loader::ResourceLoader> loader,
    BatchItem const& item,
    Print::Printer& output,
    Options const& options,
    Async::CancellationToken ct
) {
    logInfo("loading {}...", item.input);
    auto webview = Vaev::WebView::create(loader);
    co_trya$(webview->loadLocationAsync(item.input, Ref::Uti::PUBLIC_OPEN, ct));

    logInfo("rendering {}...", item.input);
    if (options.flow == Flow::PAGINATE) {
        HeaderFooterDecorator decorator;
        if (auto const& [header] = item.header) {
            logInfo("loading header {}...", header);
            decorator.headerWebView = Some(co_trya$(_loadHeaderFooterWebViewAsync(loader, header, ct)));
            decorator.headerSize = options.headerSize;
        }

        if (auto const& [footer] = item.footer) {
            logInfo("loading footer {}...", footer);
            decorator.footerWebView = Some(co_trya$(_loadHeaderFooterWebViewAsync(loader, footer, ct)));
            decorator.footerSize = options.footerSize;
        }

        auto settings = options.derivePrintSettings();
        webview->print(settings, Some(decorator)) | ForEach([&](Gfx::Snapshot& page) {
            page.replay(output.beginPage(page.size().cast<f64>())).expect();
        });
    } else {
        auto area = options.deriveArea();
        webview->changeArea(area);
        webview->changePrefs(Vaev::Style::ViewerPrefs::forRender());

        auto size = area.deviceSize().cast<Math::Au>();

        if (options.extend == Extend::FIT) {
            auto overflow = webview->scrollableOverflow();
            size.width = overflow.width;
            size.height = overflow.height;
        }
        auto& page = output.beginPage(size.cast<f64>());

        if (options.background.has())
            page.clear(Vaev::Style::resolve(*options.background, Gfx::ALPHA));

        // NOTE: Override the background of HTML document, since no
        //       one really expect a html document to be transparent
        else if (webview->document()->documentElement()->namespaceUri() == Vaev::Html::NAMESPACE) {
            page.clear(Gfx::WHITE);
        }

        webview->paint(page);
    }

    co_return Ok();
}

export Async::Task<> runBatchAsync(
    Rc<Http::Client> client,
    Vec<BatchItem> const& items,
    Ref::Url const& output,
    Options options,
    Async::CancellationToken ct
) {
    if (options.flow == Flow::AUTO)
        options.flow =
            options.outputFormat == Ref::Uti::PUBLIC_PDF
                ? Flow::PAGINATE
                : Flow::CONTINUOUS;

    auto loader = Vaev::Loader::ResourceLoader::create(client);

    if (options.batch == Batch::CONCAT) {
        auto printer = co_try$(
            Print::FilePrinter::create(
                options.outputFormat,
                {
                    .density = options.density.value(),
                }
            )
        );
        for (auto& item : items)
            co_trya$(runSingleAsync(
                loader,
                item,
                *printer,
                options,
                ct
            ));
        logInfo("saving {}...", output);
        Io::BufferWriter bw;
        co_try$(printer->write(bw));
        auto request = Http::Request::from(
            Http::Method::PUT,
            output,
            Some(Http::Body::from(bw.take()))
        );
        request->header.put(Http::Header::CONNECTION, "close"s);
        co_trya$(client->doAsync(request, ct));
    } else {
        for (auto [item, index] : iter(items) | Index()) {
            auto fileUrl = output / "{}.{}"_f(item.input.path.stem(), options.outputFormat.primarySuffix());
            auto printer = co_try$(
                Print::FilePrinter::create(
                    options.outputFormat,
                    {
                        .density = options.density.value(),
                    }
                )
            );
            co_trya$(runSingleAsync(loader, item, *printer, options, ct));

            logInfo("saving {}...", fileUrl);
            Io::BufferWriter bw;
            co_try$(printer->write(bw));

            auto request = Http::Request::from(
                Http::Method::PUT,
                fileUrl,
                Some(Http::Body::from(bw.take()))
            );
            if (index + 1 == items.len())
                request->header.put(Http::Header::CONNECTION, "close"s);

            co_trya$(client->doAsync(request, ct));
        }
    }

    co_return Ok();
}

} // namespace PaperMuncher
