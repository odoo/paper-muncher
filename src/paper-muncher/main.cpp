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

template <Vaev::Css::ValueParseable T>
    requires(not Meta::Enum<T>)
struct Cli::ValueParser<T> {
    static Res<> usage(Io::TextWriter& w) {
        return w.writeStr("value"s);
    }

    static Res<T> parse(Cursor<Token>& c) {
        if (c.ended() or c->kind != Token::OPERAND)
            return Error::invalidData("expected css value");

        auto res = try$(Vaev::Css::parseValue<T>(c->value));
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

struct InputArg {
    Str input;
    Opt<Ref::Url> header = NONE;
    Opt<Ref::Url> footer = NONE;
};

template <>
struct Cli::ValueParser<InputArg> {
    static Res<> usage(Io::TextWriter& w) {
        return w.writeStr("input [--header <document>] [--footer <document>]"s);
    }

    static Res<InputArg> parse(Cursor<Token>& c) {
        if (c.ended() or c->kind != Token::OPERAND)
            return Error::invalidInput("expected input");

        InputArg res{.input = c.next().value};

        auto headerArg = Cli::option<Opt<Ref::Url>>(NONE, "header"s, "Document to render as the page header"s, NONE);
        auto footerArg = Cli::option<Opt<Ref::Url>>(NONE, "footer"s, "Document to render as the page footer"s, NONE);
        Cli::Command cmd{"input"s, ""s, {{.title = ""s, .options = {headerArg, footerArg}}}, true};
        try$(cmd.exec(c));

        res.header = headerArg.value();
        res.footer = footerArg.value();
        return Ok(res);
    }
};

Async::Task<> entryPointAsync(Sys::Env& env, Async::CancellationToken ct) {
    auto memoryArg = Cli::option<DataSize>(NONE, "memory"s, "Memory limit enforced by the sandbox; ignored without --sandboxed (default: 4GiB)"s, 4_GiB);
    auto sandboxedArg = Cli::flag(NONE, "sandboxed"s, "Disallow access to local files and the network"s);
    auto verboseArg = Cli::flag(NONE, "verbose"s, "Enable verbose logging"s);
    auto quietArg = Cli::flag(NONE, "quiet"s, "Suppress all logging except fatal errors"s);
    Cli::Section runtimeSection{
        .title = "Runtime Options"s,
        .options = {memoryArg, sandboxedArg, verboseArg, quietArg},
        .epilog = "The memory limit only applies when --sandboxed is used."s
    };

    enum struct FormatInfer {
        INFER,
        _LEN,
    };

    auto inputsArg = Cli::operand<Vec<InputArg>>("inputs"s, "Input files, each optionally followed by --header/--footer (default: stdin)"s, {{.input = "-"s}});
    auto outputArg = Cli::option<Str>(Some('o'), "output"s, "Output file (default: stdout)"s, "-"s);
    auto batchArg = Cli::option<PaperMuncher::Batch>(NONE, "batch"s, "How to handle multiple input documents (default: concat)"s, PaperMuncher::Batch::CONCAT);
    auto formatArg = Cli::option<Union<FormatInfer, Ref::Uti>>(Some('f'), "format"s, "Override the output format (default: inferred from the output file extension)"s, FormatInfer::INFER);
    auto densityArg = Cli::option<Vaev::Css::PureResolution>(NONE, "density"s, "Pixel density of the output document, in CSS resolution units (e.g. 96dpi)"s, Vaev::Css::PureResolution{Vaev::Css::UnitValue{1, Vaev::Css::UnitValue::Unit::DPPX}});

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
    auto marginArg = Cli::option<Union<Print::MarginOption, Math::Insets<PaperMuncher::AbsoluteLength>>>(NONE, "margins"s, "Page margins, in CSS units or as a named preset (default: default)"s, Print::MarginOption::DEFAULT);
    auto backgroundArg = Cli::option<Opt<Vaev::Css::Color>>(NONE, "background"s, "Background color of the output document (default: white for HTML, transparent for SVG)"s, NONE);
    Cli::Section paperSection{
        .title = "Paper Options"s,
        .options = {paperArg, orientationArg, marginArg, backgroundArg},
        .epilog = "Use '--paper list' to display all supported standard paper sizes."s
    };

    auto widthArg = Cli::option<Opt<PaperMuncher::AbsoluteLength>>(NONE, "width"s, "Viewport width, in CSS units (e.g. 800px)"s, NONE);
    auto heightArg = Cli::option<Opt<PaperMuncher::AbsoluteLength>>(NONE, "height"s, "Viewport height, in CSS units (e.g. 600px)"s, NONE);
    auto scaleArg = Cli::option<Vaev::Css::PureResolution>(NONE, "scale"s, "Scale factor applied to the input document (e.g. 1x)"s, Vaev::Css::PureResolution{Vaev::Css::UnitValue{1, Vaev::Css::UnitValue::Unit::DPPX}});

    Cli::Section viewportSection{
        .title = "Viewport Options"s,
        .options = {widthArg, heightArg, scaleArg},
        .epilog = "Explicit --width and --height values take precedence over the --paper dimensions."s
    };

    auto headerArg = Cli::option<Opt<Ref::Url>>(NONE, "header"s, "Document to render as the page header"s, NONE);
    auto headerSizeArg = Cli::option<Union<Vaev::Css::Keywords::Auto, PaperMuncher::AbsoluteLength>>(NONE, "header-size"s, "Height of the page header (default: auto)"s, Vaev::Css::Keywords::AUTO);
    auto footerArg = Cli::option<Opt<Ref::Url>>(NONE, "footer"s, "Document to render as the page footer"s, NONE);
    auto footerSizeArg = Cli::option<Union<Vaev::Css::Keywords::Auto, PaperMuncher::AbsoluteLength>>(NONE, "footer-size"s, "Height of the page footer (default: auto)"s, Vaev::Css::Keywords::AUTO);

    Cli::Section decorationSection{
        .title = "Document decoration"s,
        .options = {
            headerArg,
            headerSizeArg,
            footerArg,
            footerSizeArg,
        },
        .epilog = "Headers and footers repeat on every page, above and below the main content, within the page margins. --header and --footer apply to the input they follow; given before the first input, they apply to every input."s
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

    co_try$(cmd.exec(env));
    if (not cmd)
        co_return Ok();

    if (paperArg.value() == PaperList::LIST) {
        for (auto& serie : Print::SERIES) {
            Sys::println("{}:", serie.name);
            for (auto& stock : serie.stocks)
                Sys::println("  {}   {:.02} x {:.02}", stock.name, stock.minorAxis.cast<f64>(), stock.majorAxis.cast<f64>());
        }
        co_return Ok();
    }

    if (verboseArg.value() and quietArg.value())
        co_return Error::invalidInput("conflicting arguments --verbose and --quiet provided");

    auto level = INFO;
    if (verboseArg.value())
        level = DEBUG;
    if (quietArg.value())
        level = FATAL;
    setLogLevel(level);

    if (sandboxedArg.value())
        co_try$(PaperMuncher::hardenSandbox({
            .memory = memoryArg.value(),
        }));

    PaperMuncher::Option options{};

    options.scale = scaleArg.value();
    options.density = densityArg.value();
    options.width = widthArg.value();
    options.height = heightArg.value();
    options.background = backgroundArg.value();
    options.stock = paperArg.value().expect<Print::PaperStock>();
    options.orientation = orientationArg.value();
    options.margins = marginArg.value();
    options.batch = batchArg.value();

    Vec<PaperMuncher::BatchItem> items;
    for (auto& i : inputsArg.value()) {
        items.pushBack({
            .input = i.input == "-"s ? "fd:stdin"_url : Ref::parseUrlOrPath(i.input, env.cwd()),
            .header = i.header ? i.header : headerArg.value(),
            .footer = i.footer ? i.footer : footerArg.value(),
        });
    }

    Ref::Url output = "fd:stdout"_url;
    if (outputArg.value() != "-"s)
        output = Ref::parseUrlOrPath(outputArg.value(), env.cwd());

    options.headerSize = headerSizeArg.value();
    options.footerSize = footerSizeArg.value();

    options.outputFormat = formatArg.value().visit(
        [&](FormatInfer) -> Ref::Uti {
            if (output.path.suffix())
                return Ref::Uti::fromSuffix(output.path.suffix());
            return Ref::Uti::PUBLIC_PDF;
        },
        [](Ref::Uti uti) {
            return uti;
        }
    );
    options.flow = flowArg.value();
    options.extend = extendArg.value();

    auto client = PaperMuncher::defaultHttpClient(sandboxedArg.value());
    co_return co_await PaperMuncher::runBatchAsync(client, items, output, options, ct);
}
