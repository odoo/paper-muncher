#include <karm/entry>

import Karm.Cli;
import PaperMuncher;
import Karm.Print;
import Karm.Logger;

import Vaev.Engine;

using namespace Karm;
using namespace Karm::Literals;
using namespace Karm::Ref::Literals;

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
    auto formatArg = Cli::option<Ref::Uti>('f', "format"s, "Override the output file format"s);
    auto densityArg = Cli::option<Str>(NONE, "density"s, "Density of the output document in css units (e.g. 96dpi)"s, "1x"s);
    auto backgroundArg = Cli::option<Str>(NONE, "background"s, "Background color of the output document (default: white for html, transparent for svg)"s, ""s);

    Cli::Section inOutSection{
        "Input/Output Options"s,
        {inputsArg, outputArg, batchArg, formatArg, densityArg, backgroundArg},
    };

    auto paperArg = Cli::option<Str>(NONE, "paper"s, "Paper size for printing (default: A4)"s, "A4"s);
    auto orientationArg = Cli::option<Str>(NONE, "orientation"s, "Page orientation (default: portrait)"s, "portrait"s);
    auto marginArg = Cli::option<Print::Margins::Named>(NONE, "margins"s, "Page margins (default: default)"s, Print::Margins::DEFAULT);
    Cli::Section paperSection{
        .title = "Paper Options"s,
        .options = {paperArg, orientationArg, marginArg},
        .epilog = "Use --paper list to display the list of supported paper size"s
    };

    auto widthArg = Cli::option<Str>(NONE, "width"s, "Width of the output document in css units (e.g. 800px)"s, ""s);
    auto heightArg = Cli::option<Str>(NONE, "height"s, "Height of the output document in css units (e.g. 600px)"s, ""s);
    auto scaleArg = Cli::option<Str>(NONE, "scale"s, "Scale of the input document in css units (e.g. 1x)"s, "1x"s);
    auto extendArg = Cli::option<PaperMuncher::Extend>(NONE, "extend"s, "How content extending past the initial viewport is handled (default: crop)"s, PaperMuncher::Extend::CROP);
    auto flowArg = Cli::option<PaperMuncher::Flow>(NONE, "flow"s, "Flow of the document (default: paginate for PDF, otherwise continuous)"s, PaperMuncher::Flow::AUTO);

    Cli::Section viewportSection{
        "Viewport Options"s,
        {widthArg, heightArg, scaleArg},
    };

    Cli::Section flowSection{
        .title = "Document flow"s,
        .options = {flowArg},
        .epilog = "auto: Paginate for PDF, otherwise continuous\n"
                  "paginated: If the content exceeds the viewport, create new pages\n"
                  "continuous: If the content exceeds the viewport, extend the viewport"s
    };

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
            "Image formats: BMP, PNG, JPEG, TGA, QOI, SVG"s
    };

    Cli::Command cmd{
        "paper-muncher"s,
        "Convert web pages (HTML, XHTML, or SVG) into printable or viewable documents like PDFs or images."s,
        {
            runtimeSection,
            inOutSection,
            paperSection,
            viewportSection,
            flowSection,
            extendSection,
            formatSection,
        }
    };

    co_trya$(cmd.execAsync(env));
    if (not cmd)
        co_return Ok();

    if (paperArg.value() == "list") {
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

    options.scale = co_try$(Vaev::parseValue<Vaev::Resolution>(scaleArg.value()));
    options.density = co_try$(Vaev::parseValue<Vaev::Resolution>(densityArg.value()));

    if (widthArg.value())
        options.width = co_try$(Vaev::parseValue<Vaev::Length>(widthArg.value()));

    if (heightArg.value())
        options.height = co_try$(Vaev::parseValue<Vaev::Length>(heightArg.value()));

    if (backgroundArg.value())
        options.background = co_try$(Vaev::parseValue<Vaev::Color>(backgroundArg.value()));

    options.stock = co_try$(Print::lookupStockByName(paperArg.value()));
    options.orientation = co_try$(Vaev::parseValue<Print::Orientation>(orientationArg.value()));
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

    options.outputFormat =
        formatArg.has()
            ? formatArg.value()
            : (Ref::Uti::fromSuffix(output.path.suffix() ?: "pdf"s));
    options.flow = flowArg.value();
    options.extend = extendArg.value();

    auto client = PaperMuncher::defaultHttpClient(sandboxedArg.value());
    co_return co_await PaperMuncher::runBatchAsync(client, inputs, output, options, ct);
}
