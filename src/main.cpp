#include <karm/entry>

import Karm.Cli;
import Karm.Print;
import Karm.Logger;
import Karm.Math;

import Vaev.Engine;
import PaperMuncher;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Ref::Literals;

template <Vaev::ValueParseable T>
    requires(not Meta::Enum<T>)
struct Cli::ValueParser<T> {
    static Res<> usage(Io::TextWriter& w) {
        return w.writeStr("value"s);
    }

    static Res<T> parse(Cursor<Token>& c) {
        if (c.ended() or c->kind != Token::OPERAND)
            return Error::invalidData("expected css value");

        auto res = try$(Vaev::parseValue<T>(c->value));
        c.next();
        return Ok(std::move(res));
    }
};

template <>
struct Cli::ValueParser<Vaev::Color> {
    static Res<> usage(Io::TextWriter& w) {
        return w.writeStr("value"s);
    }

    static Res<Vaev::Color> parse(Cursor<Token>& c) {
        if (c.ended() or c->kind != Token::OPERAND)
            return Error::invalidData("expected css color");

        auto res = try$(Vaev::parseValue<Vaev::Color>(c->value));
        c.next();
        return Ok(std::move(res));
    }
};

template <>
struct Cli::ValueParser<Print::PaperStock> {
    static Res<> usage(Io::TextWriter& w) {
        return w.writeStr("value"s);
    }

    static Res<Print::PaperStock> parse(Cursor<Token>& c) {
        if (c.ended() or c->kind != Token::OPERAND)
            return Error::invalidData("expected paper stock");

        auto res = try$(Print::lookupStockByName(c->value));
        c.next();
        return Ok(std::move(res));
    }
};

Async::Task<> entryPointAsync(Sys::Env& env, Async::CancellationToken ct) {
    auto sandboxedArg = Cli::flag(NONE, "sandboxed"s, "Disallow local file and http access"s);
    auto verboseArg = Cli::flag(NONE, "verbose"s, "Enable verbose logging"s);
    auto quietArg = Cli::flag(NONE, "quiet"s, "Suppress non-fatal logging"s);
    Cli::Section runtimeSection{
        "Runtime Options"s,
        {sandboxedArg, verboseArg, quietArg},
    };

    auto inputsArg = Cli::operand<Vec<Str>>("inputs"s, "Input files (default: stdin)"s, {"-"s});
    auto outputArg = Cli::option<Str>('o', "output"s, "Output file (default: stdout)"s, "-"s);
    auto batchArg = Cli::option<PaperMuncher::Batch>(NONE, "batch"s, "What to do when multiple document are passed as input (default: concat)"s, PaperMuncher::Batch::CONCAT);
    auto formatArg = Cli::option<Opt<Ref::Uti>>('f', "format"s, "Override the output file format"s, NONE);
    auto densityArg = Cli::option<Vaev::Resolution>(NONE, "density"s, "Density of the output document in css units (e.g. 96dpi)"s, Vaev::Resolution::fromDppx(1));
    auto backgroundArg = Cli::option<Opt<Vaev::Color>>(NONE, "background"s, "Background color of the output document (default: white for html, transparent for svg)"s, NONE);

    Cli::Section inOutSection{
        .title = "Input/Output Options"s,
        .options = {inputsArg, outputArg, batchArg, formatArg, densityArg, backgroundArg},
        .epilog = "When handling multiple inputs, separate batch mode generates individually named files matching the source stems."s
    };

    enum struct PaperList {
        LIST,
        _LEN
    };

    auto paperArg = Cli::option<Union<Print::PaperStock, PaperList>>(NONE, "paper"s, "Paper size for printing (default: A4)"s, Print::A4);
    auto orientationArg = Cli::option<Print::Orientation>(NONE, "orientation"s, "Page orientation (default: portrait)"s, Print::Orientation::PORTRAIT);
    auto marginArg = Cli::option<Union<Print::MarginOption, Math::Insets<Vaev::Length>>>(NONE, "margins"s, "Page margins (default: default)"s, Print::MarginOption::DEFAULT);
    Cli::Section paperSection{
        .title = "Paper Options"s,
        .options = {paperArg, orientationArg, marginArg},
        .epilog = "Use '--paper list' to display all supported standard paper sizes."s
    };

    auto widthArg = Cli::option<Opt<Vaev::Length>>(NONE, "width"s, "Width of the output document in css units (e.g. 800px)"s, NONE);
    auto heightArg = Cli::option<Opt<Vaev::Length>>(NONE, "height"s, "Height of the output document in css units (e.g. 600px)"s, NONE);
    auto scaleArg = Cli::option<Vaev::Resolution>(NONE, "scale"s, "Scale of the input document in css units (e.g. 1x)"s, Vaev::Resolution::fromDppx(1));

    Cli::Section viewportSection{
        .title = "Viewport Options"s,
        .options = {widthArg, heightArg, scaleArg},
        .epilog = "Explicit --width and --height dimensions take precedence over --paper dimensions."s
    };

    auto headerArg = Cli::option<Opt<Ref::Url>>(NONE, "header"s, "Add a header to the document"s, NONE);
    auto headerSizeArg = Cli::option<Union<Vaev::Keywords::Auto, Vaev::Length>>(NONE, "header-size"s, "Add a header to the document"s, Vaev::Keywords::AUTO);
    auto footerArg = Cli::option<Opt<Ref::Url>>(NONE, "footer"s, "Add a footer to the document"s, NONE);
    auto footerSizeArg = Cli::option<Union<Vaev::Keywords::Auto, Vaev::Length>>(NONE, "footer-size"s, "Add a header to the document"s, Vaev::Keywords::AUTO);

    Cli::Section decorationSection{
        .title = "Document decoration"s,
        .options = {
            headerArg,
            headerSizeArg,
            footerArg,
            footerSizeArg,
        },
        .epilog = "Headers and footers repeat on every page, appearing respectively above and below the main content within the page margins."s
    };

    auto flowArg = Cli::option<PaperMuncher::Flow>(NONE, "flow"s, "Flow of the document (default: auto)"s, PaperMuncher::Flow::AUTO);
    Cli::Section flowSection{
        .title = "Document flow"s,
        .options = {flowArg},
        .epilog = "auto: Paginate for PDF, otherwise continuous\n"
                  "paginated: If the content exceeds the viewport, create new pages\n"
                  "continuous: If the content exceeds the viewport, extend the viewport"s
    };

    auto extendArg = Cli::option<PaperMuncher::Extend>(NONE, "extend"s, "How content extending past the initial viewport is handled (default: crop)"s, PaperMuncher::Extend::CROP);
    Cli::Section extendSection{
        .title = "Document extend"s,
        .options = {extendArg},
        .epilog = "crop: The document is cropped to the container\n"
                  "fit: Container is resized to fit the document"s
    };

    Cli::Section formatSection{
        .title = "Supported Formats"s,
        .prolog =
            "Input: HTML, XHTML, SVG, Markdown\n"
            "Output: PDF or image\n"
            "Image: BMP, PNG, JPEG, TGA, QOI, SVG"s
    };

    Cli::Command cmd{
        "paper-muncher"s,
        "Convert web pages (HTML, XHTML, or SVG) into printable or viewable documents like PDFs or images."s,
        {
            runtimeSection,
            inOutSection,
            paperSection,
            viewportSection,
            decorationSection,
            flowSection,
            extendSection,
            formatSection,
        }
    };

    co_trya$(cmd.execAsync(env));
    if (not cmd)
        co_return Ok();

    if (paperArg.value() == PaperList::LIST) {
        for (auto& serie : Print::SERIES) {
            Sys::println("{}:", serie.name);
            for (auto& stock : serie.stocks)
                Sys::println("  {}   {:.02} x {.02}", stock.name, stock.minorAxis.cast<f64>(), stock.majorAxis.cast<f64>());
        }
        co_return Ok();
    }

    auto level = INFO;
    if (verboseArg.value())
        level = PRINT;
    if (quietArg.value())
        level = FATAL;
    setLogLevel(level);

    if (sandboxedArg.value())
        co_try$(Sys::enterSandbox());

    PaperMuncher::Option options{};

    options.scale = scaleArg.value();
    options.density = densityArg.value();
    options.width = widthArg.value();
    options.height = heightArg.value();
    options.background = backgroundArg.value();
    options.stock = paperArg.value().unwrap<Print::PaperStock>();
    options.orientation = orientationArg.value();
    options.margins = marginArg.value();

    Vec<Ref::Url> inputs;
    for (auto& i : inputsArg.value())
        if (i == "-"s)
            inputs.pushBack("fd:stdin"_url);
        else
            inputs.pushBack(Ref::parseUrlOrPath(i, env.cwd()));

    Ref::Url output = "fd:stdout"_url;
    if (outputArg.value() != "-"s)
        output = Ref::parseUrlOrPath(outputArg.value(), env.cwd());

    options.header = headerArg.value();
    options.footer = footerArg.value();
    options.headerSize = headerSizeArg.value();
    options.footerSize = footerSizeArg.value();

    options.outputFormat = formatArg.value().unwrapOrElse([&] -> Ref::Uti {
        if (output.path.suffix())
            Ref::Uti::fromSuffix(output.path.suffix());
        return Ref::Uti::PUBLIC_PDF;
    });
    options.flow = flowArg.value();
    options.extend = extendArg.value();

    auto client = PaperMuncher::defaultHttpClient(sandboxedArg.value());
    co_return co_await PaperMuncher::runBatchAsync(client, inputs, output, options, ct);
}
