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
import Karm.Http;

namespace PaperMuncher {

struct PrintOption {
    Vive::Resolution scale = Vive::Resolution::fromDppx(1);
    Vive::Resolution density = Vive::Resolution::fromDppx(1);
    Opt<Vive::Length> width = NONE;
    Opt<Vive::Length> height = NONE;
    Print::PaperStock paper = Print::A4;
    Print::Orientation orientation = Print::Orientation::PORTRAIT;
    Mime::Uti outputFormat = Mime::Uti::PUBLIC_PDF;
};

Async::Task<> printAsync(
    Mime::Url const& input,
    Mime::Url const& output,
    PrintOption options = {}
) {
    Gc::Heap heap;
    auto client = Http::defaultClient();
    client->userAgent = "Paper-Muncher/" stringify$(__ck_version_value) ""s;

    auto dom = co_trya$(Vive::Driver::fetchDocumentAsync(heap, *client, input));

    Vive::Layout::Resolver resolver;
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

    Vive::Driver::print(
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

Vive::Style::Media constructMediaForRender(Vive::Resolution scale, Vec2Au size) {
    return {
        .type = Vive::MediaType::SCREEN,
        .width = size.width,
        .height = size.height,
        .aspectRatio = (Vive::Number)size.width / (Vive::Number)size.height,
        .orientation = Print::Orientation::PORTRAIT,

        .resolution = scale,
        .scan = Vive::Scan::PROGRESSIVE,
        .grid = false,
        .update = Vive::Update::NONE,

        .overflowBlock = Vive::OverflowBlock::NONE,
        .overflowInline = Vive::OverflowInline::NONE,

        .color = 8,
        .colorIndex = 0,
        .monochrome = 0,
        .colorGamut = Vive::ColorGamut::SRGB,
        .pointer = Vive::Pointer::NONE,
        .hover = Vive::Hover::NONE,
        .anyPointer = Vive::Pointer::NONE,
        .anyHover = Vive::Hover::NONE,

        .prefersReducedMotion = Vive::ReducedMotion::REDUCE,
        .prefersReducedTransparency = Vive::ReducedTransparency::REDUCE,
        .prefersContrast = Vive::Contrast::NO_PREFERENCE,
        .forcedColors = Vive::Colors::NONE,
        .prefersColorScheme = Vive::ColorScheme::LIGHT,
        .prefersReducedData = Vive::ReducedData::NO_PREFERENCE,

        // NOTE: Deprecated Media Features
        .deviceWidth = size.width,
        .deviceHeight = size.height,
        .deviceAspectRatio = (Vive::Number)size.width / (Vive::Number)size.height,
    };
}

struct RenderOption {
    Vive::Resolution scale = Vive::Resolution::fromDpi(96);
    Vive::Resolution density = Vive::Resolution::fromDpi(96);
    Vive::Length width = 800_au;
    Vive::Length height = 600_au;
    Mime::Uti outputFormat = Mime::Uti::PUBLIC_BMP;
    bool wireframe = false;
};

Async::Task<> renderAsync(
    Mime::Url const& input,
    Mime::Url const& output,
    RenderOption options = {}
) {
    Gc::Heap heap;
    auto client = Http::defaultClient();
    client->userAgent = "Paper-Muncher/1.0"s;

    auto dom = co_trya$(Vive::Driver::fetchDocumentAsync(heap, *client, input));

    Vive::Layout::Resolver resolver;
    resolver.viewport.dpi = options.scale;

    Vec2Au imageSize = {
        resolver.resolve(options.width),
        resolver.resolve(options.height),
    };

    auto media = constructMediaForRender(options.scale, imageSize);
    auto [style, layout, paint, frags, canvasColor] = Vive::Driver::render(*dom, media, {.small = imageSize});

    auto image = Gfx::Surface::alloc(
        imageSize.cast<isize>() * options.density.toDppx(),
        Gfx::RGBA8888
    );

    Gfx::CpuCanvas g;
    g.begin(*image);

    if (canvasColor.alpha < 255) {
        g.clear(Gfx::WHITE);
        auto rectangle = Math::Rectf{0, 0, options.width._val, options.height._val};
        g.fillStyle(canvasColor);
        g.fill(rectangle.cast<i64>(), Math::Radiif{0});
    } else
        g.clear(canvasColor);

    g.scale(options.density.toDppx());
    paint->paint(g);
    if (options.wireframe)
        Vive::Layout::wireframe(*frags, g);
    g.end();

    Io::BufferWriter bw;
    co_try$(
        Image::save(
            image->pixels(),
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
    auto outputMimeArg = Cli::option<Str>(NONE, "output-mime"s, "Overide the output MIME type"s, ""s);

    Cli::Command cmd{
        "paper-muncher"s,
        NONE,
        "Munch the web into crisp documents"s,
    };

    auto scaleArg = Cli::option<Str>(NONE, "scale"s, "Scale of the input document in css units (e.g. 1x)"s, "1x"s);
    auto densityArg = Cli::option<Str>(NONE, "density"s, "Density of the output document in css units (e.g. 96dpi)"s, "1x"s);
    auto widthArg = Cli::option<Str>('w', "width"s, "Width of the output document in css units (e.g. 800px)"s, ""s);
    auto heightArg = Cli::option<Str>('h', "height"s, "Height of the output document in css units (e.g. 600px)"s, ""s);
    auto paperArg = Cli::option<Str>(NONE, "paper"s, "Paper size for printing (default: A4)"s, "A4"s);
    auto orientationArg = Cli::option<Str>(NONE, "orientation"s, "Page orientation (default: portrait)"s, "portrait"s);
    auto wireframeArg = Cli::flag(NONE, "wireframe"s, "Render wireframe of the layout"s);

    cmd.subCommand(
        "print"s,
        'p',
        "Render a web page into a printable document"s,
        {
            inputArg,
            outputArg,
            outputMimeArg,
            densityArg,

            paperArg,
            orientationArg,

            widthArg,
            heightArg,
            scaleArg,
        },
        [=](Sys::Context&) -> Async::Task<> {
            PaperMuncher::PrintOption options;

            options.scale = co_try$(Vive::Style::parseValue<Vive::Resolution>(scaleArg.unwrap()));
            options.density = co_try$(Vive::Style::parseValue<Vive::Resolution>(densityArg.unwrap()));

            if (widthArg.unwrap())
                options.width = co_try$(Vive::Style::parseValue<Vive::Length>(widthArg.unwrap()));

            if (heightArg.unwrap())
                options.height = co_try$(Vive::Style::parseValue<Vive::Length>(heightArg.unwrap()));

            options.paper = co_try$(Print::findPaperStock(paperArg.unwrap()));
            options.orientation = co_try$(Vive::Style::parseValue<Print::Orientation>(orientationArg.unwrap()));

            Mime::Url input = "fd:stdin"_url;
            if (inputArg.unwrap() != "-"s)
                input = Mime::parseUrlOrPath(inputArg, co_try$(Sys::pwd()));

            Mime::Url output = "fd:stdout"_url;
            if (outputArg.unwrap() != "-"s)
                output = Mime::parseUrlOrPath(outputArg, co_try$(Sys::pwd()));

            if (outputMimeArg.unwrap() != ""s)
                options.outputFormat = co_try$(Mime::Uti::fromMime(Mime::Mime{outputMimeArg}));

            co_return co_await PaperMuncher::printAsync(input, output, options);
        }
    );

    cmd.subCommand(
        "render"s,
        'r',
        "Render a web page into an image"s,
        {
            inputArg,
            outputArg,
            outputMimeArg,
            densityArg,

            widthArg,
            heightArg,
            scaleArg,

            wireframeArg,
        },
        [=](Sys::Context&) -> Async::Task<> {
            PaperMuncher::RenderOption options{};

            options.scale = co_try$(Vive::Style::parseValue<Vive::Resolution>(scaleArg.unwrap()));
            options.density = co_try$(Vive::Style::parseValue<Vive::Resolution>(densityArg.unwrap()));

            if (widthArg.unwrap())
                options.width = co_try$(Vive::Style::parseValue<Vive::Length>(widthArg.unwrap()));

            if (heightArg.unwrap())
                options.height = co_try$(Vive::Style::parseValue<Vive::Length>(heightArg.unwrap()));

            options.wireframe = wireframeArg.unwrap();

            Mime::Url input = "fd:stdin"_url;
            if (inputArg.unwrap() != "-"s)
                input = Mime::parseUrlOrPath(inputArg, co_try$(Sys::pwd()));

            Mime::Url output = "fd:stdout"_url;
            if (outputArg.unwrap() != "-"s)
                output = Mime::parseUrlOrPath(outputArg, co_try$(Sys::pwd()));

            if (outputMimeArg.unwrap() != ""s)
                options.outputFormat = co_try$(Mime::Uti::fromMime({outputMimeArg}));

            co_return co_await PaperMuncher::renderAsync(input, output, options);
        }
    );

    co_return co_await cmd.execAsync(ctx);
}
