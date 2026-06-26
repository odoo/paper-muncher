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
    client->userAgent = "Mozilla/5.0 Paper-Muncher/" stringify$(__ck_version_value) ""s;
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
    Union<Print::MarginOption, Math::Insets<Vaev::Length>> margins = Print::MarginOption::DEFAULT;
    Ref::Uti outputFormat = Ref::Uti::PUBLIC_DATA;
    Batch batch = Batch::CONCAT;
    Flow flow = Flow::AUTO;
    Extend extend = Extend::CROP;
    Opt<Ref::Url> header = NONE;
    Opt<Ref::Url> footer = NONE;
    Union<Vaev::Keywords::Auto, Vaev::Length> headerSize = Vaev::Keywords::AUTO;
    Union<Vaev::Keywords::Auto, Vaev::Length> footerSize = Vaev::Keywords::AUTO;

    auto derivePrintSettings() const -> Print::Settings {
        Vaev::Layout::Resolver resolver;

        auto stock = this->stock;
        if (this->width or this->height)
            stock = Print::PaperStock::custom(
                this->width ? resolver.resolve(*this->width) : stock.minorAxis,
                this->height ? resolver.resolve(*this->height) : stock.majorAxis
            );

        Print::Margins margins = Print::MarginOption::DEFAULT;
        this->margins.visit(
            [&](Print::MarginOption n) {
                margins = n;
            },
            [&](Math::Insets<Vaev::Length> const& insets) {
                margins = insets.map([&](Vaev::Length l) {
                    return resolver.resolve(l);
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

struct HeaderFooterDecorator : Vaev::Driver::PageDecorator {
    Opt<Rc<Vaev::Dom::Window>> headerWindow;
    Opt<Rc<Vaev::Dom::Window>> footerWindow;
    Union<Vaev::Keywords::Auto, Vaev::Length> headerSize = Vaev::Keywords::AUTO;
    Union<Vaev::Keywords::Auto, Vaev::Length> footerSize = Vaev::Keywords::AUTO;
    Map<Math::Vec2Au, Pair<Vaev::Au>> _memo;

    Vaev::RectAu layout(Vaev::Style::Media const& media, Vaev::Driver::PageLayoutInfos const& infos) override {
        auto [headerHeight, footerHeight] = _memo.lookupOrPut(infos.pageDecoration.size(), [&] {
            Vaev::Layout::Resolver resolver;
            Vaev::Au headerHeight = 0_au;
            Vaev::Au footerHeight = 0_au;

            if (headerSize == Vaev::Keywords::AUTO) {
                if (auto& [w] = headerWindow) {
                    w->changeMedia(media);
                    w->changeViewport(infos.pageDecoration.size());
                    headerHeight = w->ensureRender().frag->borderBox().height;
                }
            } else {
                headerHeight = resolver.resolve(headerSize.unwrap<Vaev::Length>());
            }

            if (footerSize == Vaev::Keywords::AUTO) {
                if (auto& [w] = footerWindow) {
                    w->changeMedia(media);
                    w->changeViewport(infos.pageDecoration.size());
                    footerHeight = w->ensureRender().frag->borderBox().height;
                }
            } else {
                footerHeight = resolver.resolve(footerSize.unwrap<Vaev::Length>());
            }

            return Pair{headerHeight, footerHeight};
        });
        return infos.pageDecoration.shrink({headerHeight, 0_au, footerHeight});
    }

    void decorate(Vaev::Style::Media const& media, Vaev::Driver::PageLayoutInfos const& infos, [[maybe_unused]] usize pageCount, Scene::Stack& pageStack) override {
        auto decorationWidth = infos.pageDecoration.width;
        auto [headerHeight, footerHeight] = _memo.lookup(infos.pageDecoration.size()).unwrap();

        if (auto& [w] = headerWindow) {
            w->changeMedia(media);
            w->changeViewport({decorationWidth, headerHeight});
            auto tranform = Math::Trans2f::translate(infos.pageDecoration.topStart().cast<f64>());
            pageStack.add(makeRc<Scene::Transform>(w->render(), tranform));
        }

        if (auto& [w] = footerWindow) {
            w->changeMedia(media);
            w->changeViewport({decorationWidth, footerHeight});
            auto transform = Math::Trans2f::translate((infos.pageDecoration.bottomStart() - Math::Vec2Au{0_au, footerHeight}).cast<f64>());
            pageStack.add(makeRc<Scene::Transform>(w->render(), transform));
        }
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
        Vaev::Layout::Resolver resolver;
        HeaderFooterDecorator decorator;
        if (auto& [header] = options.header) {
            logInfo("loading header {}...", header);
            auto window = Vaev::Dom::Window::create(client);
            co_trya$(window->loadLocationAsync(header, Ref::Uti::PUBLIC_OPEN, ct));
            decorator.headerWindow = window;
        }

        decorator.headerSize = options.headerSize;

        if (auto& [footer] = options.footer) {
            logInfo("loading footer {}...", footer);
            auto window = Vaev::Dom::Window::create(client);
            co_trya$(window->loadLocationAsync(footer, Ref::Uti::PUBLIC_OPEN, ct));
            decorator.footerWindow = window;
        }

        decorator.footerSize = options.footerSize;

        auto settings = options.derivePrintSettings();
        window->print(settings, decorator) | ForEach([&](Print::Page& page) {
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
            co_trya$(runSingleAsync(
                client,
                input,
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
            Http::Body::from(bw.take())
        );
        request->header.put(Http::Header::CONNECTION, "close"s);
        co_trya$(client->doAsync(request, ct));
    } else {
        for (auto [input, index] : iter(inputs) | Index()) {
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

            auto request = Http::Request::from(
                Http::Method::PUT,
                fileUrl,
                Http::Body::from(bw.take())
            );
            if (index + 1 == inputs.len())
                request->header.put(Http::Header::CONNECTION, "close"s);

            co_trya$(client->doAsync(request, ct));
        }
    }

    co_return Ok();
}

} // namespace PaperMuncher
