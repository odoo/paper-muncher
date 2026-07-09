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
    auto sandboxedArg = Cli::flag(NONE, "sandboxed"s, "Disallow access to local files and the network"s);
    auto verboseArg = Cli::flag(NONE, "verbose"s, "Enable verbose logging"s);
    auto quietArg = Cli::flag(NONE, "quiet"s, "Suppress all logging except fatal errors"s);
    Cli::Section runtimeSection{
        "Runtime Options"s,
        {sandboxedArg, verboseArg, quietArg},
    };

    auto inputsArg = Cli::operand<Vec<Str>>("inputs"s, "Input files (default: stdin)"s, {"-"s});
    auto outputArg = Cli::option<Str>('o', "output"s, "Output file (default: stdout)"s, "-"s);
    auto batchArg = Cli::option<PaperMuncher::Batch>(NONE, "batch"s, "How to handle multiple input documents (default: concat)"s, PaperMuncher::Batch::CONCAT);
    auto formatArg = Cli::option<Opt<Ref::Uti>>('f', "format"s, "Override the output format (default: inferred from the output file extension)"s, NONE);
    auto densityArg = Cli::option<Vaev::Resolution>(NONE, "density"s, "Pixel density of the output document, in CSS resolution units (e.g. 96dpi)"s, Vaev::Resolution::fromDppx(1));

    Cli::Section inOutSection{
        .title = "Input/Output Options"s,
        .options = {inputsArg, outputArg, batchArg, formatArg, densityArg},
        .epilog = "With multiple inputs, batch mode 'separate' writes one output file per input, named after the source file."s
    };

    enum struct PaperList {
        LIST,
        _LEN
    };

    auto paperArg = Cli::option<Union<Print::PaperStock, PaperList>>(NONE, "paper"s, "Paper size of the output pages (default: A4)"s, Print::A4);
    auto orientationArg = Cli::option<Print::Orientation>(NONE, "orientation"s, "Page orientation (default: portrait)"s, Print::Orientation::PORTRAIT);
    auto marginArg = Cli::option<Union<Print::MarginOption, Math::Insets<Vaev::AbsoluteLength>>>(NONE, "margins"s, "Page margins, in CSS units or as a named preset (default: default)"s, Print::MarginOption::DEFAULT);
    auto backgroundArg = Cli::option<Opt<Vaev::Color>>(NONE, "background"s, "Background color of the output document (default: white for HTML, transparent for SVG)"s, NONE);
    Cli::Section paperSection{
        .title = "Paper Options"s,
        .options = {paperArg, orientationArg, marginArg, backgroundArg},
        .epilog = "Use '--paper list' to display all supported standard paper sizes."s
    };

    auto widthArg = Cli::option<Opt<Vaev::AbsoluteLength>>(NONE, "width"s, "Viewport width, in CSS units (e.g. 800px)"s, NONE);
    auto heightArg = Cli::option<Opt<Vaev::AbsoluteLength>>(NONE, "height"s, "Viewport height, in CSS units (e.g. 600px)"s, NONE);
    auto scaleArg = Cli::option<Vaev::Resolution>(NONE, "scale"s, "Scale factor applied to the input document (e.g. 1x)"s, Vaev::Resolution::fromDppx(1));

    Cli::Section viewportSection{
        .title = "Viewport Options"s,
        .options = {widthArg, heightArg, scaleArg},
        .epilog = "Explicit --width and --height values take precedence over the --paper dimensions."s
    };

    auto headerArg = Cli::option<Opt<Ref::Url>>(NONE, "header"s, "Document to render as the page header"s, NONE);
    auto headerSizeArg = Cli::option<Union<Vaev::Keywords::Auto, Vaev::AbsoluteLength>>(NONE, "header-size"s, "Height of the page header (default: auto)"s, Vaev::Keywords::AUTO);
    auto footerArg = Cli::option<Opt<Ref::Url>>(NONE, "footer"s, "Document to render as the page footer"s, NONE);
    auto footerSizeArg = Cli::option<Union<Vaev::Keywords::Auto, Vaev::AbsoluteLength>>(NONE, "footer-size"s, "Height of the page footer (default: auto)"s, Vaev::Keywords::AUTO);

    Cli::Section decorationSection{
        .title = "Document decoration"s,
        .options = {
            headerArg,
            headerSizeArg,
            footerArg,
            footerSizeArg,
        },
        .epilog = "Headers and footers repeat on every page, above and below the main content, within the page margins."s
    };

    auto flowArg = Cli::option<PaperMuncher::Flow>(NONE, "flow"s, "How content flows across pages (default: auto)"s, PaperMuncher::Flow::AUTO);
    Cli::Section flowSection{
        .title = "Document flow"s,
        .options = {flowArg},
        .epilog = "auto: Paginate when producing a PDF, continuous otherwise\n"
                  "paginated: Content exceeding the viewport is split across additional pages\n"
                  "continuous: The viewport grows to fit all content on a single page"s
    };

    auto extendArg = Cli::option<PaperMuncher::Extend>(NONE, "extend"s, "How content overflowing the initial viewport is handled (default: crop)"s, PaperMuncher::Extend::CROP);
    Cli::Section extendSection{
        .title = "Document extend"s,
        .options = {extendArg},
        .epilog = "crop: Overflowing content is clipped to the container\n"
                  "fit: The container is resized to fit the content"s
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
        "Convert web pages (HTML, XHTML, SVG, or Markdown) into printable or viewable documents such as PDFs or images."s,
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

    if (verboseArg.value() and quietArg.value())
        co_return Error::invalidInput("conflicting arguments --verbose and --quiet provided");

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
    options.batch = batchArg.value();

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
            return Ref::Uti::fromSuffix(output.path.suffix());
        return Ref::Uti::PUBLIC_PDF;
    });
    options.flow = flowArg.value();
    options.extend = extendArg.value();

    auto client = PaperMuncher::defaultHttpClient(sandboxedArg.value());
    co_return co_await PaperMuncher::runBatchAsync(client, inputs, output, options, ct);
}
