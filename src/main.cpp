#include <karm-cli/args.h>
#include <karm-gfx/cpu/canvas.h>
#include <karm-image/saver.h>
#include <karm-print/file-printer.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <vaev-driver/fetcher.h>
#include <vaev-driver/print.h>
#include <vaev-driver/render.h>
#include <vaev-layout/paint.h>
#include <vaev-layout/values.h>

namespace PaperMuncher {

struct PrintOption {
    Vaev::Resolution scale = Vaev::Resolution::fromDppx(1);
    Vaev::Resolution density = Vaev::Resolution::fromDppx(1);
    Opt<Vaev::Length> width = NONE;
    Opt<Vaev::Length> height = NONE;
    Print::PaperStock paper = Print::A4;
    Print::Orientation orientation = Print::Orientation::PORTRAIT;
    Mime::Uti outputFormat = Mime::Uti::PUBLIC_PDF;
};

Res<> print(
    Mime::Url const& input,
    Mime::Url const& output,
    Vaev::Driver::Fetcher& fetcher,
    PrintOption options = {}
) {
    Gc::Heap heap;
    auto mime = Mime::sniffSuffix(input.path.suffix()).unwrapOr("application/xhtml+xml"_mime);
    auto dom = try$(Vaev::Driver::loadDocument(fetcher, heap, input, mime));

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
        .headerFooter = true,
        .backgroundGraphics = true,
    };

    auto printer = try$(Print::FilePrinter::create(
        options.outputFormat,
        {
            .density = options.density.toDppx(),
        }
    ));

    Vaev::Driver::print(
        fetcher,
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

    auto writer = try$(fetcher.transfer(output));
    try$(printer->write(*writer));
    try$(writer->done());
    return Ok();
}

Vaev::Style::Media constructMediaForRender(Vaev::Resolution scale, Vec2Au size) {
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
        .prefersContrast = Vaev::Contrast::NO_PREFERENCE,
        .forcedColors = Vaev::Colors::NONE,
        .prefersColorScheme = Vaev::ColorScheme::LIGHT,
        .prefersReducedData = Vaev::ReducedData::NO_PREFERENCE,

        // NOTE: Deprecated Media Features
        .deviceWidth = size.width,
        .deviceHeight = size.height,
        .deviceAspectRatio = (Vaev::Number)size.width / (Vaev::Number)size.height,
    };
}

struct RenderOption {
    Vaev::Resolution scale = Vaev::Resolution::fromDpi(96);
    Vaev::Resolution density = Vaev::Resolution::fromDpi(96);
    Vaev::Length width = 800_au;
    Vaev::Length height = 600_au;
    Mime::Uti outputFormat = Mime::Uti::PUBLIC_BMP;
    bool wireframe = false;
};

Res<> render(
    Mime::Url const& input,
    Mime::Url const& output,
    Vaev::Driver::Fetcher& fetcher,
    RenderOption options = {}
) {
    Gc::Heap heap;
    auto mime = Mime::sniffSuffix(input.path.suffix()).unwrapOr("application/xhtml+xml"_mime);
    auto dom = try$(Vaev::Driver::loadDocument(fetcher, heap, input, mime));

    Vaev::Layout::Resolver resolver;
    resolver.viewport.dpi = options.scale;

    Vec2Au imageSize = {
        resolver.resolve(options.width),
        resolver.resolve(options.height),
    };

    auto media = constructMediaForRender(options.scale, imageSize);
    auto [style, layout, paint, frags] = Vaev::Driver::render(fetcher, *dom, media, {.small = imageSize});

    auto image = Gfx::Surface::alloc(
        imageSize.cast<isize>() * options.density.toDppx(),
        Gfx::RGBA8888
    );

    Gfx::CpuCanvas g;
    g.begin(*image);
    g.clear(Gfx::WHITE);
    g.scale(options.density.toDppx());
    paint->paint(g);
    if (options.wireframe)
        Vaev::Layout::wireframe(*frags, g);
    g.end();

    auto writer = try$(fetcher.transfer(output));
    try$(Image::save(image->pixels(), *writer));
    try$(writer->done());
    return Ok();
}

} // namespace PaperMuncher

Async::Task<> entryPointAsync(Sys::Context& ctx) {
    auto inputArg = Cli::operand<Str>("input"s, "Input file (default: stdin)"s, "-"s);
    auto outputArg = Cli::option<Str>('o', "output"s, "Output file (default: stdout)"s, "-"s);
    auto outputMimeArg = Cli::option<Str>(NONE, "output-mime"s, "Overide the output MIME type"s, ""s);

    Cli::Command cmd{
        "paper-munch"s,
        NONE,
        "A next generation document generation tool"s,
    };

    auto scaleArg = Cli::option<Str>(NONE, "scale"s, "Scale of the output document in css units (e.g. 1x)"s, "1x"s);
    auto densityArg = Cli::option<Str>(NONE, "density"s, "Density of the output document in css units (e.g. 96dpi)"s, "1x"s);
    auto widthArg = Cli::option<Str>('w', "width"s, "Width of the output document in css units (e.g. 800px)"s, ""s);
    auto heightArg = Cli::option<Str>('h', "height"s, "Height of the output document in css units (e.g. 600px)"s, ""s);
    auto paperArg = Cli::option<Str>(NONE, "paper"s, "Paper size for printing (default: A4)"s, "A4"s);
    auto orientationArg = Cli::option<Str>(NONE, "orientation"s, "Page orientation (default: portrait)"s, "portrait"s);
    auto wireframeArg = Cli::flag(NONE, "wireframe"s, "Render wireframe of the layout"s);

    cmd.subCommand(
        "print"s,
        'p',
        "Render document for printing"s,
        {
            inputArg,
            outputArg,
            outputMimeArg,
            scaleArg,
            densityArg,
            widthArg,
            heightArg,
            paperArg,
            orientationArg,
        },
        [=](Sys::Context&) -> Async::Task<> {
            PaperMuncher::PrintOption options;

            options.scale = co_try$(Vaev::Style::parseValue<Vaev::Resolution>(scaleArg.unwrap()));
            options.density = co_try$(Vaev::Style::parseValue<Vaev::Resolution>(densityArg.unwrap()));

            if (widthArg.unwrap())
                options.width = co_try$(Vaev::Style::parseValue<Vaev::Length>(widthArg.unwrap()));

            if (heightArg.unwrap())
                options.height = co_try$(Vaev::Style::parseValue<Vaev::Length>(heightArg.unwrap()));

            options.paper = co_try$(Print::findPaperStock(paperArg.unwrap()));

            options.orientation = co_try$(Vaev::Style::parseValue<Print::Orientation>(orientationArg.unwrap()));

            Mime::Url inputUrl = "about:stdin"_url;
            Mime::Url outputUrl = "about:stdout"_url;

            Opt<Sys::FileReader> inputFile;
            if (inputArg.unwrap() != "-"s)
                inputUrl = Mime::parseUrlOrPath(inputArg);

            if (outputArg.unwrap() != "-"s)
                outputUrl = Mime::parseUrlOrPath(outputArg);

            if (outputMimeArg.unwrap() != ""s)
                options.outputFormat = co_try$(Mime::Uti::fromMime(Mime::Mime{outputMimeArg}));

            Vaev::Driver::FileFetcher fetcher;

            co_return PaperMuncher::print(inputUrl, outputUrl, fetcher, options);
        }
    );

    cmd.subCommand(
        "render"s,
        'r',
        "Render document to image"s,
        {
            inputArg,
            outputArg,
            scaleArg,
            densityArg,
            widthArg,
            heightArg,
            outputMimeArg,
            wireframeArg,
        },
        [=](Sys::Context&) -> Async::Task<> {
            PaperMuncher::RenderOption options{};

            options.scale = co_try$(Vaev::Style::parseValue<Vaev::Resolution>(scaleArg.unwrap()));
            options.density = co_try$(Vaev::Style::parseValue<Vaev::Resolution>(densityArg.unwrap()));

            if (widthArg.unwrap())
                options.width = co_try$(Vaev::Style::parseValue<Vaev::Length>(widthArg.unwrap()));

            if (heightArg.unwrap())
                options.height = co_try$(Vaev::Style::parseValue<Vaev::Length>(heightArg.unwrap()));

            options.wireframe = wireframeArg.unwrap();

            Mime::Url inputUrl = "about:stdin"_url;
            Mime::Url outputUrl = "about:stdout"_url;

            Opt<Sys::FileReader> inputFile;
            if (inputArg.unwrap() != "-"s)
                inputUrl = Mime::parseUrlOrPath(inputArg);

            if (outputArg.unwrap() != "-"s)
                outputUrl = Mime::parseUrlOrPath(outputArg);

            if (outputMimeArg.unwrap() != ""s)
                options.outputFormat = co_try$(Mime::Uti::fromMime(Mime::Mime{outputMimeArg}));

            Vaev::Driver::FileFetcher fetcher;

            co_return PaperMuncher::render(inputUrl, outputUrl, fetcher, options);
        }
    );

    co_return co_await cmd.execAsync(ctx);
}
