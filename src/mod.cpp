module;

#include <karm/macros>

export module PaperMuncher;

import Karm.Gc;
import Karm.Http;
import Karm.Image;
import Karm.Print;
import Karm.Debug;
import Karm.Sys;
import Karm.Gfx;
import Karm.Math;
import Karm.Logger;
import Karm.Scene;
import Karm.Core;
import Karm.Ref;

import Vaev.Engine;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Math::Literals;
using namespace Karm::Fmt::Literals;

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
    client->userAgent = "Paper-Muncher/" stringify$(__ck_version_value) ""s;
    return client;
}

export struct Option {
    Vaev::Resolution scale = Vaev::Resolution::fromDppx(1);
    Vaev::Resolution density = Vaev::Resolution::fromDppx(1);
    Opt<Vaev::Length> width = NONE;
    Opt<Vaev::Length> height = NONE;
    Opt<Vaev::Color> background = NONE;
    Print::PaperStock stock = Print::A4;
    Print::Orientation orientation = Print::Orientation::PORTRAIT;
    Print::Margins margins = Print::Margins::DEFAULT;
    Ref::Uti outputFormat = Ref::Uti::PUBLIC_DATA;
    Batch batch = Batch::CONCAT;
    Flow flow = Flow::AUTO;
    Extend extend = Extend::CROP;

    auto derivePrintSettings() const -> Print::Settings {
        Vaev::Layout::Resolver resolver;
        resolver.viewport.dpi = this->density;

        auto stock = this->stock;
        if (this->width or this->height)
            stock = Print::PaperStock::custom(
                this->width ? resolver.resolve(*this->width) : stock.minorAxis,
                this->height ? resolver.resolve(*this->height) : stock.majorAxis
            );

        return {
            .stock = stock,
            .orientation = this->orientation,
            .margins = this->margins,
            .scale = this->scale.toDppx(),
        };
    }

    Vaev::Style::Media deriveMedia() {
        Vaev::Layout::Resolver resolver;
        auto width = this->width ? resolver.resolve(*this->width) : 800_au;
        auto height = this->height ? resolver.resolve(*this->height) : 600_au;
        return Vaev::Style::Media::forRender(
            {width, height},
            this->scale
        );
    }
};

Async::Task<> runSingleAsync(
    Rc<Http::Client> client,
    Ref::Url const& input,
    Print::Printer& output,
    Option options,
    Async::CancellationToken ct
) {
    logInfo("loading {}...", input);
    auto window = Vaev::Dom::Window::create(client);
    co_trya$(window->loadLocationAsync(input, Ref::Uti::PUBLIC_OPEN, ct));

    logInfo("rendering {}...", input);
    if (options.flow == Flow::PAGINATE) {
        auto settings = options.derivePrintSettings();
        window->print(settings) | ForEach([&](Print::Page& page) {
            page.print(
                output,
                {.showBackgroundGraphics = true}
            );
        });
    } else {
        auto media = options.deriveMedia();
        window->changeMedia(media);

        auto scene = window->render();

        if (options.background.has()) {
            scene = makeRc<Scene::Clear>(scene, Vaev::resolve(*options.background, Gfx::ALPHA));
        }

        // NOTE: Override the background of HTML document, since no
        //       one really expect a html document to be transparent
        else if (window->document()->documentElement()->namespaceUri() == Vaev::Html::NAMESPACE) {
            scene = makeRc<Scene::Clear>(scene, Gfx::WHITE);
        }

        Math::Vec2Au size{
            media.width,
            media.height,
        };

        if (options.extend == Extend::FIT) {
            auto overflow = window->ensureRender().frag->scrollableOverflow();
            size.width = overflow.width;
            size.height = overflow.height;
        }

        Print::Page page = {
            size.cast<f64>(),
            scene,
        };
        page.print(
            output,
            {.showBackgroundGraphics = true}
        );
    }

    co_return Ok();
}

export Async::Task<> runBatchAsync(
    Rc<Http::Client> client,
    Vec<Ref::Url> const& inputs,
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
        for (auto& input : inputs)
            co_trya$(runSingleAsync(client, input, *printer, options, ct));
        logInfo("saving {}...", output);
        Io::BufferWriter bw;
        co_try$(printer->write(bw));
        co_trya$(client->doAsync(
            Http::Request::from(
                Http::Method::PUT,
                output,
                Http::Body::from(bw.take())
            ),
            ct
        ));
    } else {
        for (auto& input : inputs) {
            auto fileUrl = output / "{}.{}"_f(input.path.stem(), options.outputFormat.primarySuffix());
            auto printer = co_try$(
                Print::FilePrinter::create(
                    options.outputFormat,
                    {
                        .density = options.density.toDppx(),
                    }
                )
            );
            co_trya$(runSingleAsync(client, input, *printer, options, ct));
            logInfo("saving {}...", fileUrl);
            Io::BufferWriter bw;
            co_try$(printer->write(bw));
            co_trya$(client->doAsync(
                Http::Request::from(
                    Http::Method::PUT,
                    fileUrl,
                    Http::Body::from(bw.take())
                ),
                ct
            ));
        }
    }

    co_return Ok();
}

} // namespace PaperMuncher
