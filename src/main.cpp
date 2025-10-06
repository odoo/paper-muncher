#include <karm-sys/entry.h>

import Karm.Cli;
import Karm.Gc;
import Karm.Http;
import Karm.Image;
import Karm.Print;
import Karm.Debug;
import Karm.Sys;
import Karm.Gfx;
import Karm.Math;
import Karm.Logger;

import Vaev.Engine;

using namespace Karm;

template <>
struct Cli::ValueParser<Print::Margins> {
    static Res<> usage(Io::TextWriter& w) {
        return w.writeStr("margin"s);
    }

    static Res<Print::Margins> parse(Cursor<Token>& c) {
        if (c.ended() or c->kind != Token::OPERAND)
            return Error::invalidInput("expected margin");

        auto value = c.next().value;

        if (value == "default")
            return Ok(Print::Margins::DEFAULT);
        else if (value == "none")
            return Ok(Print::Margins::NONE);
        else if (value == "minimum")
            return Ok(Print::Margins::MINIMUM);
        else
            return Error::invalidInput("expected margin");
    }
};

namespace PaperMuncher {

static Rc<Http::Client> _createHttpClient(bool unsecure) {
    Vec<Rc<Http::Transport>> transports;

    transports.pushBack(Http::pipeTransport());

    if (unsecure) {
        transports.pushBack(Http::httpTransport());
        transports.pushBack(Http::localTransport(Http::LocalTransportPolicy::ALLOW_ALL));
    } else {
        // NOTE: Only allow access to bundle assets and standard input/output.
        transports.pushBack(Http::localTransport({"bundle"s, "fd"s, "data"s}));
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
    Print::Margins margins = Print::Margins::DEFAULT;
    Ref::Uti outputFormat = Ref::Uti::PUBLIC_PDF;

    auto preparePrintSettings(this auto const& self) -> Print::Settings {
        Vaev::Layout::Resolver resolver;
        resolver.viewport.dpi = self.scale;

        auto paper = self.paper;

        if (self.orientation == Print::Orientation::LANDSCAPE)
            paper = paper.landscape();

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
            .margins = self.margins,
            .scale = self.scale.toDppx(),
        };
    }
};

static Async::Task<> printAsync(
    Rc<Http::Client> client,
    Vec<Ref::Url> const& inputs,
    Ref::Url const& output,
    PrintOption options = {}
) {
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
        auto window = Vaev::Dom::Window::create(client);
        co_trya$(window->loadLocationAsync(input));
        window->print(options.preparePrintSettings()) | forEach([&](Print::Page& page) {
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
    Ref::Uti outputFormat = Ref::Uti::PUBLIC_BMP;
};

static Async::Task<> renderAsync(
    Rc<Http::Client> client,
    Ref::Url const& input,
    Ref::Url const& output,
    RenderOption options = {}
) {
    auto window = Vaev::Dom::Window::create(client);
    co_trya$(window->loadLocationAsync(input));
    Vaev::Layout::Resolver resolver;
    Vec2Au imageSize = {
        resolver.resolve(options.width),
        resolver.resolve(options.height),
    };
    window->changeMedia(Vaev::Style::Media::forRender(imageSize.cast<Au>(), options.scale));

    Rc<Http::Body> body = Http::Body::empty();

    Image::Saver saves{.format = options.outputFormat, .density = options.density.toDppx()};
    body = Http::Body::from(co_try$(Image::save(window->render(), imageSize.cast<isize>(), saves)));

    co_trya$(client->doAsync(
        Http::Request::from(Http::Method::PUT, output, body)
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
        {
            {
                "Global Options"s,
                {
                    unsecureArg,
                    verboseArg,
                },
            },
        },
        [=](Sys::Context&) -> Async::Task<> {
            setLogLevel(verboseArg.value() ? PRINT : ERROR);
            if (not unsecureArg.value())
                co_try$(Sys::enterSandbox());
            co_return Ok();
        }
    };

    auto scaleArg = Cli::option<Str>(NONE, "scale"s, "Scale of the input document in css units (e.g. 1x)"s, "1x"s);
    auto densityArg = Cli::option<Str>(NONE, "density"s, "Density of the output document in css units (e.g. 96dpi)"s, "1x"s);
    auto widthArg = Cli::option<Str>(NONE, "width"s, "Width of the output document in css units (e.g. 800px)"s, ""s);
    auto heightArg = Cli::option<Str>(NONE, "height"s, "Height of the output document in css units (e.g. 600px)"s, ""s);
    auto paperArg = Cli::option<Str>(NONE, "paper"s, "Paper size for printing (default: A4)"s, "A4"s);
    auto orientationArg = Cli::option<Str>(NONE, "orientation"s, "Page orientation (default: portrait)"s, "portrait"s);
    auto marginArg = Cli::option<Print::Margins>(NONE, "margins"s, "Page margins (default: default)"s, Print::Margins::DEFAULT);

    cmd.subCommand(
        "print"s,
        "Render a web page into a printable document"s,
        {
            {
                "Input/Output Options"s,
                {
                    inputsArg,
                    outputArg,
                    formatArg,
                    densityArg,
                },
            },

            {
                "Paper Options"s,
                {
                    paperArg,
                    orientationArg,
                    marginArg,
                },
            },

            {
                "Viewport Options"s,
                {
                    widthArg,
                    heightArg,
                    scaleArg,
                },
            },
        },
        [=](Sys::Context&) -> Async::Task<> {
            PaperMuncher::PrintOption options{};

            options.scale = co_try$(Vaev::parseValue<Vaev::Resolution>(scaleArg.value()));
            options.density = co_try$(Vaev::parseValue<Vaev::Resolution>(densityArg.value()));

            if (widthArg.value())
                options.width = co_try$(Vaev::parseValue<Vaev::Length>(widthArg.value()));

            if (heightArg.value())
                options.height = co_try$(Vaev::parseValue<Vaev::Length>(heightArg.value()));

            options.paper = co_try$(Print::findPaperStock(paperArg.value()));
            options.orientation = co_try$(Vaev::parseValue<Print::Orientation>(orientationArg.value()));
            options.margins = marginArg.value();

            Vec<Ref::Url> inputs;
            for (auto& i : inputsArg.value())
                if (i == "-"s)
                    inputs.pushBack("fd:stdin"_url);
                else
                    inputs.pushBack(Ref::parseUrlOrPath(i, co_try$(Sys::pwd())));

            Ref::Url output = "fd:stdout"_url;
            if (outputArg.value() != "-"s)
                output = Ref::parseUrlOrPath(outputArg.value(), co_try$(Sys::pwd()));

            if (formatArg.value() != ""s)
                options.outputFormat = co_try$(Ref::Uti::fromMime({formatArg.value()}));

            auto client = PaperMuncher::_createHttpClient(unsecureArg.value());

            co_return co_await PaperMuncher::printAsync(client, inputs, output, options);
        }
    );

    cmd.subCommand(
        "render"s,
        "Render a web page into an image"s,
        {
            {
                "Input/Output Options"s,
                {
                    inputArg,
                    outputArg,
                    formatArg,
                    densityArg,
                },
            },

            {
                "Viewport Options"s,
                {

                    widthArg,
                    heightArg,
                    scaleArg,
                },
            },
        },
        [=](Sys::Context&) -> Async::Task<> {
            PaperMuncher::RenderOption options{};

            options.scale = co_try$(Vaev::parseValue<Vaev::Resolution>(scaleArg.value()));
            options.density = co_try$(Vaev::parseValue<Vaev::Resolution>(densityArg.value()));

            if (widthArg.value())
                options.width = co_try$(Vaev::parseValue<Vaev::Length>(widthArg.value()));

            if (heightArg.value())
                options.height = co_try$(Vaev::parseValue<Vaev::Length>(heightArg.value()));

            Ref::Url input = "fd:stdin"_url;
            if (inputArg.value() != "-"s)
                input = Ref::parseUrlOrPath(inputArg.value(), co_try$(Sys::pwd()));

            Ref::Url output = "fd:stdout"_url;
            if (outputArg.value() != "-"s)
                output = Ref::parseUrlOrPath(outputArg.value(), co_try$(Sys::pwd()));

            if (formatArg.value() != ""s) {
                options.outputFormat = co_try$(Ref::Uti::fromMime({formatArg.value()}));
            } else {
                auto mime = Ref::sniffSuffix(output.path.suffix());
                options.outputFormat = mime ? co_try$(Ref::Uti::fromMime(*mime)) : Ref::Uti::PUBLIC_BMP;
            }

            auto client = PaperMuncher::_createHttpClient(unsecureArg.value());

            co_return co_await renderAsync(client, input, output, options);
        }
    );

    co_return co_await cmd.execAsync(ctx);
}
