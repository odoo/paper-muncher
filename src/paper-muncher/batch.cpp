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

import :absoluteLength;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Math::Literals;
using namespace Karm::Fmt::Literals;
using namespace Karm::Ref::Literals;

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

export struct Option {
    Vaev::Css::PureResolution scale = Vaev::Css::PureResolution::fromDppx(1);
    Vaev::Css::PureResolution density = Vaev::Css::PureResolution::fromDppx(1);
    Opt<AbsoluteLength> width = NONE;
    Opt<AbsoluteLength> height = NONE;
    Opt<Vaev::Css::Color> background = NONE;
    Print::PaperStock stock = Print::A4;
    Print::Orientation orientation = Print::Orientation::PORTRAIT;
    Union<Print::MarginOption, Math::Insets<AbsoluteLength>> margins = Print::MarginOption::DEFAULT;
    Ref::Uti outputFormat = Ref::Uti::PUBLIC_DATA;
    Batch batch = Batch::CONCAT;
    Flow flow = Flow::AUTO;
    Extend extend = Extend::CROP;
    Union<Vaev::Keywords::Auto, AbsoluteLength> headerSize = Vaev::Keywords::AUTO;
    Union<Vaev::Keywords::Auto, AbsoluteLength> footerSize = Vaev::Keywords::AUTO;

    auto derivePrintSettings() const -> Print::Settings {
        auto stock = this->stock;
        if (this->width or this->height)
            stock = Print::PaperStock::custom(
                this->width ? Vaev::Layout::Au{this->width->toPx()} : stock.minorAxis,
                this->height ? Vaev::Layout::Au{this->height->toPx()} : stock.majorAxis
            );

        Print::Margins margins = Print::MarginOption::DEFAULT;
        this->margins.visit(
            [&](Print::MarginOption n) {
                margins = n;
            },
            [&](Math::Insets<AbsoluteLength> const& insets) {
                margins = insets.map([&](AbsoluteLength l) {
                    return Vaev::Layout::Au{l.toPx()};
                });
            }
        );

        return {
            .stock = stock,
            .orientation = this->orientation,
            .margins = margins,
            .scale = this->scale.toDppx(),
        };
    }

    Vaev::Style::DisplayArea deriveArea() const {
        auto width = this->width ? this->width->toPx() : 800.0;
        auto height = this->height ? this->height->toPx() : 600.0;
        return Vaev::Style::DisplayArea::forRender(
            {width, height},
            Vaev::Style::Dppx{this->scale.toDppx()}
        );
    }
};

struct HeaderFooterDecorator : Vaev::Driver::PageDecorator {
    Opt<Rc<Vaev::WebView>> headerWebView;
    Opt<Rc<Vaev::WebView>> footerWebView;
    Union<Vaev::Keywords::Auto, AbsoluteLength> headerSize = Vaev::Keywords::AUTO;
    Union<Vaev::Keywords::Auto, AbsoluteLength> footerSize = Vaev::Keywords::AUTO;
    Map<Math::Vec2Au, Pair<Vaev::Layout::Au>> _memo;

    Vaev::Layout::RectAu layout(Vaev::Style::DisplayArea const& area, Vaev::Style::ViewerPrefs const& prefs, Vaev::Driver::PageLayoutInfos const& infos) override {
        auto [headerHeight, footerHeight] = _memo.lookupOrPut(infos.pageDecoration.size(), [&] {
            Vaev::Layout::Au headerHeight = 0_au;
            Vaev::Layout::Au footerHeight = 0_au;

            if (headerSize == Vaev::Keywords::AUTO) {
                if (auto& [w] = headerWebView) {
                    w->changeArea(area.withViewport(Vaev::Layout::toPixel(infos.pageDecoration.size())));
                    w->changePrefs(prefs);
                    headerHeight = w->borderBox().height;
                }
            } else {
                headerHeight = Vaev::Layout::Au{headerSize.expect<AbsoluteLength>().toPx()};
            }

            if (footerSize == Vaev::Keywords::AUTO) {
                if (auto& [w] = footerWebView) {
                    w->changeArea(area.withViewport(Vaev::Layout::toPixel(infos.pageDecoration.size())));
                    w->changePrefs(prefs);
                    footerHeight = w->borderBox().height;
                }
            } else {
                footerHeight = Vaev::Layout::Au{footerSize.expect<AbsoluteLength>().toPx()};
            }

            return Pair{headerHeight, footerHeight};
        });
        return infos.pageDecoration.shrink({headerHeight, 0_au, footerHeight});
    }

    void decorate(Vaev::Style::DisplayArea const& area, Vaev::Style::ViewerPrefs const& prefs, Vaev::Driver::PageLayoutInfos const& infos, usize pageCount, Gfx::Canvas& g) override {
        auto decorationWidth = infos.pageDecoration.width;
        auto [headerHeight, footerHeight] = _memo.lookup(infos.pageDecoration.size()).expect();

        Vaev::Style::CounterSet pageCounters;
        pageCounters.instantiateCounter(nullptr, {Vaev::CustomIdent{"page"_sym}, false}, static_cast<Vaev::Integer>(infos.pageNumber));
        pageCounters.instantiateCounter(nullptr, {Vaev::CustomIdent{"pages"_sym}, false}, static_cast<Vaev::Integer>(pageCount));

        if (auto& [w] = headerWebView) {
            w->changeArea(area.withViewport(Vaev::Layout::toPixel(Vaev::Layout::Vec2Au{decorationWidth, headerHeight})));
            w->changePrefs(prefs);
            w->changeInitialCounterSet(pageCounters);
            g.push();
            g.transform(Math::Trans2f::translate(infos.pageDecoration.topStart().cast<f64>()));
            w->paint(g);
            g.pop();
        }

        if (auto& [w] = footerWebView) {
            w->changeArea(area.withViewport(Vaev::Layout::toPixel(Vaev::Layout::Vec2Au{decorationWidth, footerHeight})));
            w->changePrefs(prefs);
            w->changeInitialCounterSet(pageCounters);
            g.push();
            g.transform(Math::Trans2f::translate((infos.pageDecoration.bottomStart() - Math::Vec2Au{0_au, footerHeight}).cast<f64>()));
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
    Option const& options,
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
    Option options,
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
                    .density = options.density.toDppx(),
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
                        .density = options.density.toDppx(),
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
