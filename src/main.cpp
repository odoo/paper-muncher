#include <karm-cli/args.h>
#include <karm-gc/heap.h>
#include <karm-gfx/cpu/canvas.h>
#include <karm-image/saver.h>
#include <karm-print/file-printer.h>
#include <karm-print/page.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/proc.h>
#include <vaev-style/computer.h>

import Vaev.Driver;
import Vaev.Layout;
import Vaev.Loader;
import Karm.Http;

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
        multiplexTransport(std::move(transports))
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
};

static Async::Task<> printAsync(
    Rc<Http::Client> client,
    Mime::Url const& input,
    Mime::Url const& output,
    PrintOption options = {}
) {
    Gc::Heap heap;

    auto dom = co_trya$(Vaev::Loader::fetchDocumentAsync(heap, *client, input));

    Vaev::Layout::Resolver resolver;
    resolver.viewport.dpi = options.scale;

    auto paper = options.paper;

    if (options.orientation == Print::Orientation::LANDSCAPE)
        paper = paper.landscape();

    if (options.width) {
        paper.name = "custom";
        paper.width = resolver.resolve(*options.width).cast<f64>();
    }

    if (options.height) {
        paper.name = "custom";
        paper.height = resolver.resolve(*options.height).cast<f64>();
    }

    Print::Settings settings = {
        .paper = paper,
        .scale = options.scale.toDppx(),
    };

    auto printer = co_try$(
        Print::FilePrinter::create(
            options.outputFormat,
            {
                .density = options.density.toDppx(),
            }
        )
    );

    Vaev::Driver::print(
        *dom,
        settings
    ) | forEach([&](Print::Page& page) {
        page.print(
            *printer,
            {
                .showBackgroundGraphics = true,
            }
        );
    });

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

static Vaev::Style::Media constructMediaForRender(Vaev::Resolution scale, Vec2Au size) {
    return {
        .type = Vaev::MediaType::SCREEN,
        .width = size.width,
        .height = size.height,
        .aspectRatio = (Vaev::Number)size.width / (Vaev::Number)size.height,
        .orientation = Print::Orientation::PORTRAIT,

        .resolution = scale,
        .scan = Vaev::Scan::PROGRESSIVE,
        .grid = false,
        .update = Vaev::Update::NONE,

        .overflowBlock = Vaev::OverflowBlock::NONE,
        .overflowInline = Vaev::OverflowInline::NONE,

        .color = 8,
        .colorIndex = 0,
        .monochrome = 0,
        .colorGamut = Vaev::ColorGamut::SRGB,
        .pointer = Vaev::Pointer::NONE,
        .hover = Vaev::Hover::NONE,
        .anyPointer = Vaev::Pointer::NONE,
        .anyHover = Vaev::Hover::NONE,

        .prefersReducedMotion = Vaev::ReducedMotion::REDUCE,
        .prefersReducedTransparency = Vaev::ReducedTransparency::REDUCE,
        .prefersContrast = Vaev::Contrast::MORE,
        .forcedColors = Vaev::Colors::NONE,
        .prefersColorScheme = Vaev::ColorScheme::LIGHT,
        .prefersReducedData = Vaev::ReducedData::NO_PREFERENCE,

        // NOTE: Deprecated Media Features
        .deviceWidth = size.width,
        .deviceHeight = size.height,
        .deviceAspectRatio = static_cast<Vaev::Number>(size.width) / static_cast<Vaev::Number>(size.height),
    };
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
    resolver.viewport.dpi = options.scale;

    Vec2Au imageSize = {
        resolver.resolve(options.width),
        resolver.resolve(options.height),
    };

    auto media = constructMediaForRender(options.scale, imageSize);
    auto [layout, scene, frags] = Vaev::Driver::render(*dom, media, {.small = imageSize});

    auto surface = scene->snapshot(imageSize.cast<f64>(), options.density.toDppx());

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
    auto inputArg = Cli::operand<Str>("input"s, "Input file (default: stdin)"s, "-"s);
    auto outputArg = Cli::option<Str>('o', "output"s, "Output file (default: stdout)"s, "-"s);
    auto formatArg = Cli::option<Str>('f', "format"s, "Override the output file format"s, ""s);
    auto unsecureArg = Cli::flag(NONE, "unsecure"s, "Allow local file and http access"s);
    auto verboseArg = Cli::flag('v', "verbose"s, "Makes paper-muncher be more talkative, it might yap about how its day's going"s);

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
            inputArg,
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

            Mime::Url input = "fd:stdin"_url;
            if (inputArg.unwrap() != "-"s)
                input = Mime::parseUrlOrPath(inputArg, co_try$(Sys::pwd()));

            Mime::Url output = "fd:stdout"_url;
            if (outputArg.unwrap() != "-"s)
                output = Mime::parseUrlOrPath(outputArg, co_try$(Sys::pwd()));

            if (formatArg.unwrap() != ""s)
                options.outputFormat = co_try$(Mime::Uti::fromMime({formatArg}));

            auto client = PaperMuncher::_createHttpClient(unsecureArg);
            co_return co_await PaperMuncher::printAsync(client, input, output, options);
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
