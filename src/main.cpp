#include <karm-gfx/cpu/canvas.h>
#include <karm-logger/logger.h>
#include <karm-math/au.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/proc.h>

import Karm.Cli;
import Karm.Gc;
import Karm.Http;
import Karm.Image;
import Karm.Print;
import Karm.Debug;
import Vaev.Engine;

using namespace Karm;

namespace PaperMuncher {

static Rc<Http::Client> _createHttpClient(bool unsecure) {
    Vec<Rc<Http::Transport>> transports;

    transports.pushBack(Http::pipeTransport());

    if (unsecure) {
        transports.pushBack(Http::httpTransport());
        transports.pushBack(Http::localTransport(Http::LocalTransportPolicy::ALLOW_ALL));
    } else {
        // NOTE: Only allow access to bundle assets and standard input/output.
        transports.pushBack(Http::localTransport({"bundle"s, "fd"s}));
    }

    auto client = makeRc<Http::Client>(
        Http::multiplexTransport(std::move(transports))
    );
    client->userAgent = "Paper-Muncher/" stringify$(__ck_version_value) ""s;

    return client;
}

struct PrintOption {
    Vaev::Resolution scale = Vaev::Resolution::fromDppx(1);
    Vaev::Resolution density = Vaev::Resolution::fromDppx(1);
    Opt<Vaev::Length> width = NONE;
    Opt<Vaev::Length> height = NONE;
    Print::PaperStock paper = Print::A4;
    Print::Orientation orientation = Print::Orientation::PORTRAIT;
    Mime::Uti outputFormat = Mime::Uti::PUBLIC_PDF;

    auto preparePrintSettings(this auto const& self) -> Print::Settings {
        Vaev::Layout::Resolver resolver;
        resolver.viewport.dpi = self.scale;

        auto paper = self.paper;

        if (self.width) {
            paper.name = "custom";
            paper.width = resolver.resolve(*self.width).template cast<f64>();
        }

        if (self.height) {
            paper.name = "custom";
            paper.height = resolver.resolve(*self.height).template cast<f64>();
        }

        return {
            .paper = paper,
            .orientation = self.orientation,
            .scale = self.scale.toDppx(),
        };
    }
};

static Async::Task<> printAsync(
    Rc<Http::Client> client,
    Vec<Mime::Url> const& inputs,
    Mime::Url const& output,
    PrintOption options = {}
) {
    Gc::Heap heap;

    auto printer = co_try$(
        Print::FilePrinter::create(
            options.outputFormat,
            {
                .density = options.density.toDppx(),
            }
        )
    );

    for (auto& input : inputs) {
        logInfo("rendering {}...", input);
        auto dom = co_trya$(Vaev::Loader::fetchDocumentAsync(heap, *client, input));
        Vaev::Driver::print(
            *dom,
            options.preparePrintSettings()
        ) | forEach([&](Print::Page& page) {
            page.print(
                *printer,
                {
                    .showBackgroundGraphics = true,
                }
            );
        });
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

struct RenderOption {
    Vaev::Resolution scale = Vaev::Resolution::fromDpi(96);
    Vaev::Resolution density = Vaev::Resolution::fromDpi(96);
    Vaev::Length width = 800_au;
    Vaev::Length height = 600_au;
    Mime::Uti outputFormat = Mime::Uti::PUBLIC_BMP;
};

static Async::Task<> renderAsync(
    Rc<Http::Client> client,
    Mime::Url const& input,
    Mime::Url const& output,
    RenderOption options = {}
) {
    Gc::Heap heap;

    auto dom = co_trya$(Vaev::Loader::fetchDocumentAsync(heap, *client, input));

    Vaev::Layout::Resolver resolver;
    Vec2Au imageSize = {
        resolver.resolve(options.width),
        resolver.resolve(options.height),
    };

    auto surface = Vaev::Driver::renderToSurface(dom, imageSize, options.scale);
    Io::BufferWriter bw;
    co_try$(
        Image::save(
            *surface,
            bw,
            {
                .format = options.outputFormat,
            }
        )
    );

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

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto inputArg = Cli::operand<Str>("input"s, "Input files (default: stdin)"s, "-"s);
    auto inputsArg = Cli::operand<Vec<Str>>("inputs"s, "Input files (default: stdin)"s, {"-"s});
    auto outputArg = Cli::option<Str>('o', "output"s, "Output file (default: stdout)"s, "-"s);
    auto formatArg = Cli::option<Str>('f', "format"s, "Override the output file format"s, ""s);
    auto unsecureArg = Cli::flag(NONE, "unsecure"s, "Allow local file and http access"s);
    auto verboseArg = Cli::flag(NONE, "verbose"s, "Makes paper-muncher be more talkative, it might yap about how its day's going"s);

    Cli::Command cmd{
        "paper-muncher"s,
        "Munch the web into crisp documents"s,
        {unsecureArg, verboseArg},
        [=](Sys::Context&) -> Async::Task<> {
            setLogLevel(verboseArg ? PRINT : ERROR);
            if (not unsecureArg)
                co_try$(Sys::enterSandbox());
            co_return Ok();
        }
    };

    auto scaleArg = Cli::option<Str>(NONE, "scale"s, "Scale of the input document in css units (e.g. 1x)"s, "1x"s);
    auto densityArg = Cli::option<Str>(NONE, "density"s, "Density of the output document in css units (e.g. 96dpi)"s, "1x"s);
    auto widthArg = Cli::option<Str>('w', "width"s, "Width of the output document in css units (e.g. 800px)"s, ""s);
    auto heightArg = Cli::option<Str>('h', "height"s, "Height of the output document in css units (e.g. 600px)"s, ""s);
    auto paperArg = Cli::option<Str>(NONE, "paper"s, "Paper size for printing (default: A4)"s, "A4"s);
    auto orientationArg = Cli::option<Str>(NONE, "orientation"s, "Page orientation (default: portrait)"s, "portrait"s);

    cmd.subCommand(
        "print"s,
        "Render a web page into a printable document"s,
        {
            inputsArg,
            outputArg,
            formatArg,
            densityArg,

            paperArg,
            orientationArg,

            widthArg,
            heightArg,
            scaleArg,
        },
        [=](Sys::Context&) -> Async::Task<> {
            PaperMuncher::PrintOption options{};

            options.scale = co_try$(Vaev::parseValue<Vaev::Resolution>(scaleArg.unwrap()));
            options.density = co_try$(Vaev::parseValue<Vaev::Resolution>(densityArg.unwrap()));

            if (widthArg.unwrap())
                options.width = co_try$(Vaev::parseValue<Vaev::Length>(widthArg.unwrap()));

            if (heightArg.unwrap())
                options.height = co_try$(Vaev::parseValue<Vaev::Length>(heightArg.unwrap()));

            options.paper = co_try$(Print::findPaperStock(paperArg.unwrap()));
            options.orientation = co_try$(Vaev::parseValue<Print::Orientation>(orientationArg.unwrap()));

            Vec<Mime::Url> inputs;
            for (auto& i : inputsArg.unwrap())
                if (i == "-"s)
                    inputs.pushBack("fd:stdin"_url);
                else
                    inputs.pushBack(Mime::parseUrlOrPath(i, co_try$(Sys::pwd())));

            Mime::Url output = "fd:stdout"_url;
            if (outputArg.unwrap() != "-"s)
                output = Mime::parseUrlOrPath(outputArg, co_try$(Sys::pwd()));

            if (formatArg.unwrap() != ""s)
                options.outputFormat = co_try$(Mime::Uti::fromMime({formatArg}));

            auto client = PaperMuncher::_createHttpClient(unsecureArg);
            co_return co_await PaperMuncher::printAsync(client, inputs, output, options);
        }
    );

    cmd.subCommand(
        "render"s,
        "Render a web page into an image"s,
        {
            inputArg,
            outputArg,
            formatArg,
            densityArg,

            widthArg,
            heightArg,
            scaleArg,
        },
        [=](Sys::Context&) -> Async::Task<> {
            PaperMuncher::RenderOption options{};

            options.scale = co_try$(Vaev::parseValue<Vaev::Resolution>(scaleArg.unwrap()));
            options.density = co_try$(Vaev::parseValue<Vaev::Resolution>(densityArg.unwrap()));

            if (widthArg.unwrap())
                options.width = co_try$(Vaev::parseValue<Vaev::Length>(widthArg.unwrap()));

            if (heightArg.unwrap())
                options.height = co_try$(Vaev::parseValue<Vaev::Length>(heightArg.unwrap()));

            Mime::Url input = "fd:stdin"_url;
            if (inputArg.unwrap() != "-"s)
                input = Mime::parseUrlOrPath(inputArg, co_try$(Sys::pwd()));

            Mime::Url output = "fd:stdout"_url;
            if (outputArg.unwrap() != "-"s)
                output = Mime::parseUrlOrPath(outputArg, co_try$(Sys::pwd()));

            if (formatArg.unwrap() != ""s)
                options.outputFormat = co_try$(Mime::Uti::fromMime({formatArg}));

            auto client = PaperMuncher::_createHttpClient(unsecureArg);

            co_return co_await renderAsync(client, input, output, options);
        }
    );

    co_return co_await cmd.execAsync(ctx);
}
