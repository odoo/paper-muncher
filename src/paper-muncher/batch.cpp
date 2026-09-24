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

static Res<> _validateDecorationCount(Str name, usize count, usize inputCount) {
    if (count == 0 or count == 1 or count == inputCount)
        return Ok();

    return Error::invalidInput(
        "expected --{} to receive zero documents, one document, or one document per input (got {} documents for {} inputs)"_f(
            name, count, inputCount
        )
    );
}

static bool _isUrlNone(Ref::Url const& url) {
    return Io::toLowerCase(url.path.basename()).unwrap() == "none"s;
}

static Opt<Ref::Url> _resolveDecoration(Vec<Ref::Url> const& docs, usize inputCount, usize index) {
    if (docs.len() == 1) {
        if (_isUrlNone(docs[0]))
            return NONE;
        return Some(docs[0]);
    }

    if (docs.len() == inputCount) {
        if (_isUrlNone(docs[index]))
            return NONE;
        return Some(docs[index]);
    }

    return NONE;
}

export Res<Vec<BatchItem>> makeBatchItems(
    Vec<Ref::Url> const& inputs,
    Vec<Ref::Url> const& headers,
    Vec<Ref::Url> const& footers
) {
    try$(_validateDecorationCount("header"s, headers.len(), inputs.len()));
    try$(_validateDecorationCount("footer"s, footers.len(), inputs.len()));

    Vec<BatchItem> items;
    for (auto [input, index] : iter(inputs) | Index()) {
        items.pushBack(BatchItem{
            .input = input,
            .header = _resolveDecoration(headers, inputs.len(), index),
            .footer = _resolveDecoration(footers, inputs.len(), index),
        });
    }

    return Ok(std::move(items));
}

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
    Vaev::Resolution scale = Vaev::Resolution::fromDppx(1);
    Vaev::Resolution density = Vaev::Resolution::fromDppx(1);
    Opt<Vaev::AbsoluteLength> width = NONE;
    Opt<Vaev::AbsoluteLength> height = NONE;
    Opt<Vaev::Color> background = NONE;
    Print::PaperStock stock = Print::A4;
    Print::Orientation orientation = Print::Orientation::PORTRAIT;
    Union<Print::MarginOption, Math::Insets<Vaev::AbsoluteLength>> margins = Print::MarginOption::DEFAULT;
    Ref::Uti outputFormat = Ref::Uti::PUBLIC_DATA;
    Batch batch = Batch::CONCAT;
    Flow flow = Flow::AUTO;
    Extend extend = Extend::CROP;
    Union<Vaev::Keywords::Auto, Vaev::AbsoluteLength> headerSize = Vaev::Keywords::AUTO;
    Union<Vaev::Keywords::Auto, Vaev::AbsoluteLength> footerSize = Vaev::Keywords::AUTO;

    auto derivePrintSettings() const -> Print::Settings {
        auto stock = this->stock;
        if (this->width or this->height)
            stock = Print::PaperStock::custom(
                this->width ? Vaev::resolve(*this->width) : stock.minorAxis,
                this->height ? Vaev::resolve(*this->height) : stock.majorAxis
            );

        Print::Margins margins = Print::MarginOption::DEFAULT;
        this->margins.visit(
            [&](Print::MarginOption n) {
                margins = n;
            },
            [&](Math::Insets<Vaev::AbsoluteLength> const& insets) {
                margins = insets.map([&](Vaev::AbsoluteLength l) {
                    return Vaev::resolve(l);
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

    Vaev::Style::Media deriveMedia() const {
        Vaev::Layout::Resolver resolver;
        auto width = this->width ? resolver.resolve(*this->width) : 800_au;
        auto height = this->height ? resolver.resolve(*this->height) : 600_au;
        return Vaev::Style::Media::forRender(
            {width, height},
            this->scale
        );
    }
};

struct HeaderFooterDecorator : Vaev::Driver::PageDecorator {
    Opt<Rc<Vaev::Dom::Window>> headerWindow;
    Opt<Rc<Vaev::Dom::Window>> footerWindow;
    Union<Vaev::Keywords::Auto, Vaev::AbsoluteLength> headerSize = Vaev::Keywords::AUTO;
    Union<Vaev::Keywords::Auto, Vaev::AbsoluteLength> footerSize = Vaev::Keywords::AUTO;
    Map<Math::Vec2Au, Pair<Vaev::Au>> _memo;

    Vaev::RectAu layout(Vaev::Style::Media const& media, Vaev::Driver::PageLayoutInfos const& infos) override {
        auto [headerHeight, footerHeight] = _memo.lookupOrPut(infos.pageDecoration.size(), [&] {
            Vaev::Au headerHeight = 0_au;
            Vaev::Au footerHeight = 0_au;

            if (headerSize == Vaev::Keywords::AUTO) {
                if (auto& [w] = headerWindow) {
                    w->changeMedia(media);
                    w->changeViewport(infos.pageDecoration.size());
                    headerHeight = w->borderBox().height;
                }
            } else {
                headerHeight = Vaev::resolve(headerSize.unwrap<Vaev::AbsoluteLength>());
            }

            if (footerSize == Vaev::Keywords::AUTO) {
                if (auto& [w] = footerWindow) {
                    w->changeMedia(media);
                    w->changeViewport(infos.pageDecoration.size());
                    footerHeight = w->borderBox().height;
                }
            } else {
                footerHeight = Vaev::resolve(footerSize.unwrap<Vaev::AbsoluteLength>());
            }

            return Pair{headerHeight, footerHeight};
        });
        return infos.pageDecoration.shrink({headerHeight, 0_au, footerHeight});
    }

    void decorate(Vaev::Style::Media const& media, Vaev::Driver::PageLayoutInfos const& infos, usize pageCount, Gfx::Canvas& g) override {
        auto decorationWidth = infos.pageDecoration.width;
        auto [headerHeight, footerHeight] = _memo.lookup(infos.pageDecoration.size()).unwrap();

        Vaev::Style::CounterSet pageCounters;
        pageCounters.instantiateCounter(nullptr, {Vaev::CustomIdent{"page"_sym}, false}, static_cast<Vaev::Integer>(infos.pageNumber));
        pageCounters.instantiateCounter(nullptr, {Vaev::CustomIdent{"pages"_sym}, false}, static_cast<Vaev::Integer>(pageCount));

        if (auto& [w] = headerWindow) {
            w->changeMedia(media);
            w->changeViewport({decorationWidth, headerHeight});
            w->changeInitialCounterSet(pageCounters);
            g.push();
            g.transform(Math::Trans2f::translate(infos.pageDecoration.topStart().cast<f64>()));
            w->paint(g);
            g.pop();
        }

        if (auto& [w] = footerWindow) {
            w->changeMedia(media);
            w->changeViewport({decorationWidth, footerHeight});
            w->changeInitialCounterSet(pageCounters);
            g.push();
            g.transform(Math::Trans2f::translate((infos.pageDecoration.bottomStart() - Math::Vec2Au{0_au, footerHeight}).cast<f64>()));
            w->paint(g);
            g.pop();
        }
    }
};

Async::Task<Rc<Vaev::Dom::Window>> _loadHeaderFooterWindowAsync(Rc<Http::Client> client, Ref::Url const& url, Async::CancellationToken ct) {
    auto window = Vaev::Dom::Window::create(client);
    co_trya$(window->loadLocationAsync(url, Ref::Uti::PUBLIC_OPEN, ct));

    auto sheet = co_trya$(Vaev::Loader::fetchStylesheetAsync(*client, *window->document(), "bundle://vaev-engine/wkhtmltopdf-polyfill.css"_url, Vaev::Style::Origin::USER_AGENT, ct));
    window->document()->styleSheets->add(std::move(sheet));

    co_return Ok(window);
}

Async::Task<> runSingleAsync(
    Rc<Http::Client> client,
    BatchItem const& item,
    Print::Printer& output,
    Option const& options,
    Async::CancellationToken ct
) {
    logInfo("loading {}...", item.input);
    auto window = Vaev::Dom::Window::create(client);
    co_trya$(window->loadLocationAsync(item.input, Ref::Uti::PUBLIC_OPEN, ct));

    logInfo("rendering {}...", item.input);
    if (options.flow == Flow::PAGINATE) {
        HeaderFooterDecorator decorator;
        if (auto const& [header] = item.header) {
            logInfo("loading header {}...", header);
            decorator.headerWindow = Some(co_trya$(_loadHeaderFooterWindowAsync(client, header, ct)));
            decorator.headerSize = options.headerSize;
        }

        if (auto const& [footer] = item.footer) {
            logInfo("loading footer {}...", footer);
            decorator.footerWindow = Some(co_trya$(_loadHeaderFooterWindowAsync(client, footer, ct)));
            decorator.footerSize = options.footerSize;
        }

        auto settings = options.derivePrintSettings();
        window->print(settings, Some(decorator)) | ForEach([&](Gfx::Snapshot& page) {
            page.replay(output.beginPage(page.size().cast<f64>())).unwrap();
        });
    } else {
        auto media = options.deriveMedia();
        window->changeMedia(media);

        Math::Vec2Au size{
            media.width,
            media.height,
        };

        if (options.extend == Extend::FIT) {
            auto overflow = window->scrollableOverflow();
            size.width = overflow.width;
            size.height = overflow.height;
        }
        auto& page = output.beginPage(size.cast<f64>());

        if (options.background.has())
            page.clear(Vaev::resolve(*options.background, Gfx::ALPHA));

        // NOTE: Override the background of HTML document, since no
        //       one really expect a html document to be transparent
        else if (window->document()->documentElement()->namespaceUri() == Vaev::Html::NAMESPACE) {
            page.clear(Gfx::WHITE);
        }

        window->paint(page);
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
                client,
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
            co_trya$(runSingleAsync(client, item, *printer, options, ct));

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
