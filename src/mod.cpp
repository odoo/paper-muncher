module;

#include <karm-core/macros.h>

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

namespace PaperMuncher {

export enum struct Flow {
    AUTO,
    PAGINATE,
    CONTINUOUS,
    _LEN,
};

export enum struct Extend {
    CROP, //< The document is cropped to the container
    FIT,  //< Container is resized to fit the document
    _LEN,
};

export Rc<Http::Client> defaultHttpClient(bool sandboxed) {
    Vec<Rc<Http::Transport>> transports;

    transports.pushBack(Http::pipeTransport());

    if (sandboxed) {
        // NOTE: Only allow access to bundle assets and standard input/output.
        transports.pushBack(Http::localTransport({"bundle"s, "fd"s, "data"s}));
    } else {
        transports.pushBack(Http::httpTransport());
        transports.pushBack(Http::localTransport(Http::LocalTransportPolicy::ALLOW_ALL));
    }

    auto client = makeRc<Http::Client>(
        Http::multiplexTransport(std::move(transports))
    );
    client->userAgent = "Paper-Muncher/" stringify$(__ck_version_value) ""s;

    return client;
}

export struct Option {
    Vaev::Resolution scale = Vaev::Resolution::fromDppx(1);
    Vaev::Resolution density = Vaev::Resolution::fromDppx(1);
    Opt<Vaev::Length> width = NONE;
    Opt<Vaev::Length> height = NONE;
    Print::PaperStock paper = Print::A4;
    Print::Orientation orientation = Print::Orientation::PORTRAIT;
    Print::Margins margins = Print::Margins::DEFAULT;
    Ref::Uti outputFormat = Ref::Uti::PUBLIC_DATA;
    Flow flow = Flow::AUTO;
    Extend extend = Extend::CROP;

    auto preparePrintSettings() -> Print::Settings {
        Vaev::Layout::Resolver resolver;
        resolver.viewport.dpi = this->scale;

        auto paper = this->paper;

        if (this->orientation == Print::Orientation::LANDSCAPE)
            paper = paper.landscape();

        if (this->width or this->height) {
            paper.name = "custom";
            if (this->width)
                paper.width = resolver.resolve(*this->width).cast<f64>();

            if (this->height)
                paper.height = resolver.resolve(*this->height).cast<f64>();
        }

        return {
            .paper = paper,
            .margins = this->margins,
            .scale = this->scale.toDppx(),
        };
    }

    Vaev::Style::Media prepareMedia() {
        Vaev::Layout::Resolver resolver;
        auto width = this->width ? resolver.resolve(*this->width) : 800_au;
        auto height = this->height ? resolver.resolve(*this->height) : 600_au;
        return Vaev::Style::Media::forRender({width, height}, this->scale);
    }
};

export Async::Task<> run(
    Rc<Http::Client> client,
    Vec<Ref::Url> const& inputs,
    Ref::Url const& output,
    Option options = {}
) {
    if (options.flow == Flow::AUTO)
        options.flow =
            options.outputFormat == Ref::Uti::PUBLIC_PDF
                ? Flow::PAGINATE
                : Flow::CONTINUOUS;

    auto printer = co_try$(
        Print::FilePrinter::create(
            options.outputFormat,
            {
                .density = options.density.toDppx(),
            }
        )
    );

    for (auto& input : inputs) {
        logInfo("loading {}...", input);
        auto window = Vaev::Dom::Window::create(client);
        co_trya$(window->loadLocationAsync(input));

        logInfo("rendering {}...", input);
        if (options.flow == Flow::PAGINATE) {
            auto settings = options.preparePrintSettings();
            window->print(settings) | forEach([&](Print::Page& page) {
                page.print(
                    *printer,
                    {.showBackgroundGraphics = true}
                );
            });
        } else {
            auto media = options.prepareMedia();
            window->changeMedia(media);

            auto scene = window->render();

            // NOTE: Override the background of HTML document, since no
            //       one really expect a html document to be transparent
            if (window->document()->documentElement()->namespaceUri() == Vaev::Html::NAMESPACE) {
                scene = makeRc<Scene::Clear>(scene, Gfx::WHITE);
            }

            Print::PaperStock paper{
                "image",
                media.width.cast<f64>(),
                media.height.cast<f64>(),
            };

            if (options.extend == Extend::FIT) {
                auto overflow = window->ensureRender().frag->scrollableOverflow();
                paper.width = overflow.width.cast<f64>();
                paper.height = overflow.height.cast<f64>();
            }

            Print::Page page = {paper, scene};
            page.print(
                *printer,
                {.showBackgroundGraphics = true}
            );
        }
    }

    logInfo("saving {}...", output);
    Io::BufferWriter bw;
    co_try$(printer->write(bw));
    co_trya$(client->doAsync(
        Http::Request::from(
            Http::Method::PUT,
            output,
            Http::Body::from(bw.take())
        )
    ));

    co_return Ok();
}

} // namespace PaperMuncher