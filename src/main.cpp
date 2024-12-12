#include <karm-cli/args.h>
#include <karm-gfx/cpu/canvas.h>
#include <karm-image/saver.h>
#include <karm-io/emit.h>
#include <karm-io/funcs.h>
#include <karm-print/image-printer.h>
#include <karm-print/pdf-printer.h>
#include <karm-sys/entry.h>
#include <karm-sys/file.h>
#include <karm-sys/time.h>
#include <karm-ui/app.h>
#include <vaev-driver/fetcher.h>
#include <vaev-driver/print.h>
#include <vaev-driver/render.h>
#include <vaev-layout/values.h>
#include <vaev-markup/html.h>
#include <vaev-markup/xml.h>
#include <vaev-style/values.h>

namespace Vaev::Tools {

Res<> cssDumpStylesheet(Mime::Url const &url) {
    auto start = Sys::now();
    auto stylesheet = try$(Vaev::Driver::fetchStylesheet(url));
    auto elapsed = Sys::now() - start;
    logInfo("fetched in {}ms", elapsed.toUSecs() / 1000.0);
    Sys::println("{#}", stylesheet);
    return Ok();
}

Res<> cssDumpSst(Mime::Url const &url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));

    auto start = Sys::now();

    Io::SScan s{buf};
    Vaev::Css::Lexer lex{s};
    Vaev::Css::Sst sst = Vaev::Css::consumeRuleList(lex, true);

    auto elapsed = Sys::now() - start;
    logInfo("parsed in {}ms", elapsed.toUSecs() / 1000.0);

    Sys::println("{}", sst);

    return Ok();
}

Res<> cssDumpTokens(Mime::Url const &url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));
    Io::SScan s{buf};
    Vaev::Css::Lexer lex{s};
    while (not lex.ended())
        Sys::println("{}", lex.next());
    return Ok();
}

Res<> styleListProps() {
    Vaev::Style::StyleProp::any([]<typename T>(Meta::Type<T>) {
        Sys::println("{}", T::name());
        return false;
    });
    return Ok();
}

Res<> markupDumpDom(Mime::Url const &url) {
    auto dom = try$(Vaev::Driver::fetchDocument(url));
    Sys::println("{}", dom);
    return Ok();
}

Res<> markupDumpTokens(Mime::Url const &url) {
    auto file = try$(Sys::File::open(url));
    auto buf = try$(Io::readAllUtf8(file));

    Vec<Vaev::Markup::HtmlToken> tokens;

    struct VecSink : public Vaev::Markup::HtmlSink {
        Vec<Vaev::Markup::HtmlToken> &tokens;

        VecSink(Vec<Vaev::Markup::HtmlToken> &tokens)
            : tokens(tokens) {
        }

        void accept(Vaev::Markup::HtmlToken const &token) override {
            tokens.pushBack(token);
        }
    };

    VecSink sink{tokens};
    Vaev::Markup::HtmlLexer lexer{};
    lexer.bind(sink);

    for (auto r : iterRunes(buf))
        lexer.consume(r);

    for (auto &t : tokens)
        Sys::println("{}", t);

    return Ok();
}

struct PrintOption {
    bool dumpStyle = false;
    bool dumpDom = false;
    bool dumpLayout = false;
    bool dumpPaint = false;
    bool printToBMP = false;

    Resolution scale = Resolution::fromDppx(1);
    Opt<Length> width = NONE;
    Opt<Length> height = NONE;
    Print::PaperStock paper = Print::A4;
    Print::Orientation orientation = Print::Orientation::PORTRAIT;
};

Res<> print(Mime::Url const &, Strong<Markup::Document> dom, Io::Writer &output, PrintOption options = {}) {
    Layout::Resolver resolver;
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
        .backgroundGraphics = false,
    };
    auto pages = Vaev::Driver::print(
        *dom,
        settings
    );

    if (options.dumpDom)
        Sys::println("--- START OF DOM ---\n{}\n--- END OF DOM ---\n", dom);

    if (options.dumpPaint)
        Sys::println("--- START OF PAINT ---\n{}\n--- END OF PAINT ---\n", pages);

    Strong<Print::FilePrinter> printer =
        options.printToBMP
            ? Strong<Print::FilePrinter>{makeStrong<Print::ImagePrinter>()}
            : makeStrong<Print::PdfPrinter>();

    for (auto &page : pages) {
        page->print(
            *printer,
            {
                .showBackgroundGraphics = true,
            }
        );
    }

    return printer->write(output);
}

Res<> print(Mime::Url const &url, Io::Reader &input, Io::Writer &output, PrintOption options = {}) {
    auto dom = try$(Vaev::Driver::loadDocument(url, "application/xhtml+xml"_mime, input));
    return print(url, dom, output, options);
}

Res<> print(Mime::Url const &url, Io::Writer &output, PrintOption options = {}) {
    auto dom = try$(Vaev::Driver::fetchDocument(url));
    return print(url, dom, output, options);
}

Vaev::Style::Media constructMediaForRender(Resolution scale, Vec2Px size) {
    return {
        .type = Vaev::MediaType::SCREEN,
        .width = size.width,
        .height = size.height,
        .aspectRatio = (Number)size.width / (Number)size.height,
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
    };
}

struct RenderOption {
    bool dumpStyle = false;
    bool dumpDom = false;
    bool dumpLayout = false;
    bool dumpPaint = false;

    Resolution scale = Resolution::fromDpi(96);
    Length width = 800_px;
    Length height = 600_px;
};

Res<> render(Mime::Url const &, Strong<Markup::Document> dom, Io::Writer &output, RenderOption options = {}) {
    Layout::Resolver resolver;
    resolver.viewport.dpi = options.scale;

    Vec2Px imageSize = {
        resolver.resolve(options.width),
        resolver.resolve(options.height),
    };

    auto media = constructMediaForRender(options.scale, imageSize);
    auto [style, layout, paint, _] = Vaev::Driver::render(*dom, media, {.small = imageSize});

    if (options.dumpDom)
        Sys::println("--- START OF DOM ---\n{}\n--- END OF DOM ---\n", dom);

    if (options.dumpStyle)
        Sys::println("--- START OF STYLE ---\n{}\n--- END OF STYLE ---\n", style);

    if (options.dumpLayout)
        Sys::println("--- START OF LAYOUT ---\n{}\n--- END OF LAYOUT ---\n", layout);

    if (options.dumpPaint)
        Sys::println("--- START OF PAINT ---\n{}\n--- END OF PAINT ---\n", paint);

    auto image = Gfx::Surface::alloc(imageSize.cast<isize>(), Gfx::RGBA8888);
    Gfx::CpuCanvas g;
    g.begin(*image);
    g.clear(Gfx::WHITE);
    paint->paint(g);
    g.end();

    try$(Karm::Image::save(image->pixels(), output));

    return Ok();
}

Res<> render(Mime::Url const &input, Io::Reader &reader, Io::Writer &output, RenderOption options = {}) {
    auto dom = try$(Vaev::Driver::loadDocument(input, "application/xhtml+xml"_mime, reader));
    return render(input, dom, output, options);
}

Res<> render(Mime::Url const &input, Io::Writer &output, RenderOption options = {}) {
    auto dom = try$(Vaev::Driver::fetchDocument(input));
    return render(input, dom, output, options);
}

} // namespace Vaev::Tools

Async::Task<> entryPointAsync(Sys::Context &ctx) {
    auto inputArg = Cli::operand<Str>("input"s, "Input file (default: stdin)"s, "-"s);
    auto outputArg = Cli::option<Str>('o', "output"s, "Output file (default: stdout)"s, "-"s);

    Cli::Command cmd{
        "paper-munch"s,
        NONE,
        "A next generation document generation tool"s,
    };

    Cli::Command &cssCmd = cmd.subCommand(
        "css"s,
        'c',
        "CSS related commands"s
    );

    cssCmd.subCommand(
        "dump-stylesheet"s,
        's',
        "Dump a stylesheet"s,
        {inputArg},
        [inputArg](Sys::Context &) -> Async::Task<> {
            auto input = co_try$(Mime::parseUrlOrPath(inputArg));
            co_return Vaev::Tools::cssDumpStylesheet(input);
        }
    );

    cssCmd.subCommand(
        "dump-sst"s,
        'a',
        "Dump a stylesheet"s,
        {inputArg},
        [inputArg](Sys::Context &) -> Async::Task<> {
            auto input = co_try$(Mime::parseUrlOrPath(inputArg));
            co_return Vaev::Tools::cssDumpSst(input);
        }
    );

    cssCmd.subCommand(
        "dump-tokens"s,
        't',
        "Dump CSS tokens"s,
        {inputArg},
        [inputArg](Sys::Context &) -> Async::Task<> {
            auto input = co_try$(Mime::parseUrlOrPath(inputArg));
            co_return Vaev::Tools::cssDumpTokens(input);
        }
    );

    Cli::Command &styleCmd = cmd.subCommand(
        "style"s,
        's',
        "Style related commands"s
    );

    styleCmd.subCommand(
        "list-props"s,
        'l',
        "List all style properties"s,
        {},
        [=](Sys::Context &) -> Async::Task<> {
            co_return Vaev::Tools::styleListProps();
        }
    );

    Cli::Command &markupCmd = cmd.subCommand(
        "markup"s,
        'm',
        "Markup related commands"s
    );

    markupCmd.subCommand(
        "dump-dom"s,
        NONE,
        "Dump the DOM tree"s,
        {inputArg},
        [=](Sys::Context &) -> Async::Task<> {
            auto input = co_try$(Mime::parseUrlOrPath(inputArg));
            co_return Vaev::Tools::markupDumpDom(input);
        }
    );

    markupCmd.subCommand(
        "dump-tokens"s,
        't',
        "Dump HTML tokens"s,
        {inputArg},
        [=](Sys::Context &) -> Async::Task<> {
            auto input = co_try$(Mime::parseUrlOrPath(inputArg));
            co_return Vaev::Tools::markupDumpTokens(input);
        }
    );

    Cli::Flag dumpStyleArg = Cli::flag('s', "dump-style"s, "Dump the stylesheet"s);
    Cli::Flag dumpDomArg = Cli::flag('d', "dump-dom"s, "Dump the DOM tree"s);
    Cli::Flag dumpLayoutArg = Cli::flag('l', "dump-layout"s, "Dump the layout tree"s);
    Cli::Flag dumpPaintArg = Cli::flag('p', "dump-paint"s, "Dump the paint tree"s);
    Cli::Option scaleArg = Cli::option<Str>(NONE, "scale"s, "Scale of the output document in css units (e.g. 96dpi)"s, "1x"s);
    Cli::Option widthArg = Cli::option<Str>('w', "width"s, "Width of the output document in css units (e.g. 800px)"s, ""s);
    Cli::Option heightArg = Cli::option<Str>('h', "height"s, "Height of the output document in css units (e.g. 600px)"s, ""s);
    Cli::Option paperArg = Cli::option<Str>(NONE, "paper"s, "Paper size for printing (default: A4)"s, "A4"s);
    Cli::Option orientationArg = Cli::option<Str>(NONE, "orientation"s, "Page orientation (default: portrait)"s, "portrait"s);

    cmd.subCommand(
        "print"s,
        'p',
        "Render document for printing"s,
        {inputArg,
         outputArg,
         dumpStyleArg,
         dumpDomArg,
         dumpLayoutArg,
         dumpPaintArg,
         scaleArg,
         widthArg,
         heightArg,
         paperArg,
         orientationArg
        },
        [=](Sys::Context &) -> Async::Task<> {
            Vaev::Tools::PrintOption options{
                .dumpStyle = dumpStyleArg,
                .dumpDom = dumpDomArg,
                .dumpLayout = dumpLayoutArg,
                .dumpPaint = dumpPaintArg,
            };

            options.scale = co_try$(Vaev::Style::parseValue<Vaev::Resolution>(scaleArg.unwrap()));

            if (widthArg.unwrap())
                options.width = co_try$(Vaev::Style::parseValue<Vaev::Length>(widthArg.unwrap()));

            if (heightArg.unwrap())
                options.height = co_try$(Vaev::Style::parseValue<Vaev::Length>(heightArg.unwrap()));

            options.paper = co_try$(Print::findPaperStock(paperArg.unwrap()));

            options.orientation = co_try$(Vaev::Style::parseValue<Print::Orientation>(orientationArg.unwrap()));

            Mime::Url inputUrl = "about:stdin"_url;
            MutCursor<Io::Reader> input = &Sys::in();
            MutCursor<Io::Writer> output = &Sys::out();

            Opt<Sys::FileReader> inputFile;
            if (inputArg.unwrap() != "-"s) {
                inputUrl = co_try$(Mime::parseUrlOrPath(inputArg));
                inputFile = co_try$(Sys::File::open(inputUrl));
                input = &inputFile.unwrap();
            }

            Opt<Sys::FileWriter> outputFile;
            if (outputArg.unwrap() != "-"s) {
                auto outputUrl = co_try$(Mime::parseUrlOrPath(outputArg));
                outputFile = co_try$(Sys::File::create(outputUrl));
                output = &outputFile.unwrap();

                if (auto mimeTypeOutput = Mime::sniffSuffix(outputUrl.path.suffix())) {
                    options.printToBMP = mimeTypeOutput.unwrap() == "image/bmp"_mime;
                }
            }

            co_return Vaev::Tools::print(inputUrl, *input, *output, options);
        }
    );

    cmd.subCommand(
        "render"s,
        'r',
        "Render document to image"s,
        {
            inputArg,
            outputArg,
            dumpStyleArg,
            dumpDomArg,
            dumpLayoutArg,
            dumpPaintArg,
            scaleArg,
            widthArg,
            heightArg,
        },
        [=](Sys::Context &) -> Async::Task<> {
            Vaev::Tools::RenderOption options{
                .dumpStyle = dumpStyleArg,
                .dumpDom = dumpDomArg,
                .dumpLayout = dumpLayoutArg,
                .dumpPaint = dumpPaintArg,
            };

            options.scale = co_try$(Vaev::Style::parseValue<Vaev::Resolution>(scaleArg.unwrap()));

            if (widthArg.unwrap())
                options.width = co_try$(Vaev::Style::parseValue<Vaev::Length>(widthArg.unwrap()));

            if (heightArg.unwrap())
                options.height = co_try$(Vaev::Style::parseValue<Vaev::Length>(heightArg.unwrap()));

            Mime::Url inputUrl = "about:stdin"_url;
            MutCursor<Io::Reader> input = &Sys::in();
            MutCursor<Io::Writer> output = &Sys::out();

            Opt<Sys::FileReader> inputFile;
            if (inputArg.unwrap() != "-"s) {
                inputUrl = co_try$(Mime::parseUrlOrPath(inputArg));
                inputFile = co_try$(Sys::File::open(inputUrl));
                input = &inputFile.unwrap();
            }

            Opt<Sys::FileWriter> outputFile;
            if (outputArg.unwrap() != "-"s) {
                auto outputUrl = co_try$(Mime::parseUrlOrPath(outputArg));
                outputFile = co_try$(Sys::File::create(outputUrl));
                output = &outputFile.unwrap();
            }

            co_return Vaev::Tools::render(inputUrl, *input, *output, options);
        }
    );

    co_return co_await cmd.execAsync(ctx);
}
